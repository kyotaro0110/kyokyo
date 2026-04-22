#pragma once
#include "UDP.hpp"
#include "AudioConfig.hpp"
#include "stdafx.h"

class SoundReceiver : public UDP
{
private:
	float m_currentVolume = 0.0f;
	std::deque<float> m_volumeHistory;
	const size_t m_maxHistory = 200; //グラフの横幅

	PaStream* m_stream = nullptr;
	std::deque<float> m_playBuffer;
	std::mutex m_bufferMutex;

protected:
	
	std::vector<int16_t> m_buffer;

public:
	//親クラスのコンストラクタを呼ぶ
	SoundReceiver(const String& _ip, uint16_t _port) 
		: UDP(s3d::Unicode::ToUTF8(_ip), _port)
	{
		m_buffer.resize(AudioConfig::SAMPLE_COUNT);

		Pa_Initialize();
		//Pa_OpenDefaultStream(&m_stream, 0, 1, paFloat32, AudioConfig::SAMPLE_COUNT, paFramesPerBufferUnspecified, paCallback, this
	}

	virtual void update() {
		if (send_socket == INVALID_SOCKET) return;

		sockaddr_in from;
		int fromLen = sizeof(from);
		bool dataReceived = false;

		while (true) {
			int ret = recvfrom(send_socket, reinterpret_cast<char*>(m_buffer.data()), sizeof(int16_t) * AudioConfig::SAMPLE_COUNT, 0, (struct sockaddr*)&from, &fromLen);

			if (ret == SOCKET_ERROR) {
				// WSAEWOULDBLOCK は 「今読み取れるデータがない」だけなので正常
				break;
			}

			if (ret <= 0) break;

			dataReceived = true;
			int16_t maxAbs = 0;
			for (int i = 0; i < (ret / sizeof(int16_t)); ++i) {
				maxAbs = std::max<int16_t>(maxAbs, std::abs(m_buffer[i]));
			}

			m_currentVolume = static_cast<float>(maxAbs) / 32767.0f;

		}

		m_volumeHistory.push_back(m_currentVolume);

		if (m_volumeHistory.size() > m_maxHistory) {
			m_volumeHistory.pop_front();
		}		
	}

	
	float getVolume() const {
		return m_currentVolume;
	}

	

	const std::vector<int16_t>& getBuffer() const { return m_buffer; }

};

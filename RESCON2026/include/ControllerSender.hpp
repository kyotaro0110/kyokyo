#pragma once
#include "stdafx.h"
#include "UDP.hpp"
#include "Controller.hpp"
#pragma comment(lib, "ws2_32.lib")

class ControllerSender : public UDP, public Controller
{
protected:
	size_t playerIndex = 0;
	//通信に関わる変数は基底クラスにあるので不要
	const double SEND_INTERVAL = 0.1;
	Stopwatch m_stopwatch{ StartImmediately::No };
	Stopwatch m_timer{ StartImmediately::Yes };
	double m_lastReceiveTime = -10.0;
	char send_buf[64];
	uint8 max_output = 60;
	double threshold = 0.0;

	int32 m_lastError = 0;

	virtual void send(const char button, const char direction, uint8 magnitude)
	{
		//initialize()が成功してsend_socketが有効なら送信
		if (send_socket == INVALID_SOCKET) {
			return;
		}

		memset(send_buf, 0, sizeof(send_buf));
		send_buf[0] = button;
		send_buf[1] = direction;
		send_buf[2] = static_cast<char>(magnitude);
		int ret = sendto(send_socket, send_buf, 3, 0,
			(struct sockaddr*)&send_addr, sizeof(send_addr));

		if (ret == SOCKET_ERROR) {
			m_lastError = WSAGetLastError();
		}
		else {
			m_lastError = 0;
		}

	}

	void receiveCheck()
	{
		char buffer[256];
		sockaddr_in from;
		int fromlen = sizeof(from);

		int result = recvfrom(
			send_socket,
			buffer,
			sizeof(buffer),
			0,
			(sockaddr*)&from,
			&fromlen
		);

		if (result > 0) {
			Print << U"a";
			m_lastReceiveTime = m_timer.s();
		}
		else {
			int err = WSAGetLastError();
			if (err != WSAEWOULDBLOCK) {
				Print << U"recv error: " << err;
			}
		}
	}
//const Array<String> buttonnames = { U"A", U"B", U"X", U"Y", U"l", U"r", U"L", U"R", U"N", U"N", U"N", U"N", U"N", U"N", U"N", U"U", U"M", U"S" };
public:
	//コンストラクタ
	ControllerSender(const String& _ip, uint16_t _port) :
		UDP(s3d::Unicode::ToUTF8(_ip), _port),
		Controller(),
		send_buf{},
		max_output(60),
		threshold(0.0),
		m_lastError(0)
	{
		//std::memset(send_buf, 0, sizeof(send_buf));
		//UDPのコンストラクタはstd::stringなので変換
		if (initialize()) {
			m_stopwatch.start();
			//Print << U"UDP初期化成功!";
		}
		else {
			//Print << U"UDP初期化に失敗";
		}
	}
	virtual ~ControllerSender() = default;

	int32 getLastError() const { return m_lastError; }
	bool hasError() const { return m_lastError != 0; }

	void sendIdle() {
		send('k', 'k', 0);
		m_stopwatch.restart();
	}

	virtual void update(size_t MaxSpeed)
    {
		//receiveCheck();
        if (m_stopwatch.sF() < SEND_INTERVAL) return;

		std::memset(send_buf, 'k', 10);

        // button() が現在押されているボタンの名前を返すと想定
		// Comannd()はdirectionとmagnitudeを返す構造体関数
        char btnName = button();
		char direction = Command(MaxSpeed).direction;
		size_t magnitude = Command(MaxSpeed).magnitude;
		
        // コントローラーが接続されており、アクティブモードになっていたら送信
		if (btnName != 'z') {

			send(btnName, direction, magnitude);

			m_stopwatch.restart();
		}

		else {
			sendIdle();
		}
    }

	bool isConnected() {
		return (m_timer.s() - m_lastReceiveTime < 1.0);
	}

};

class ControllerSender_2th : public ControllerSender, public Controller_2th {
public:
	ControllerSender_2th(const String& _ip, uint16_t _port) :
		ControllerSender(_ip, _port), // ControllerSenderを初期化
		Controller_2th()               // Controller_2thを初期化
	{
		// 追加の初期化が必要ならここに記述
	}

	void send(char button, char direction, uint8 magnitude) override {
		//initialize()が成功してsend_socketが有効なら送信
		if (send_socket == INVALID_SOCKET) {
			return;
		}

		//Print << ServoCondition().x;

		memset(send_buf, 0, sizeof(send_buf));
		send_buf[0] = button;
		send_buf[1] = direction;
		send_buf[2] = static_cast<char>(magnitude);
		send_buf[3] = static_cast<char>(ServoCondition().x/2);
		send_buf[4] = static_cast<char>(ServoCondition().y/2);
		
		int ret = sendto(send_socket, send_buf, 5, 0,
			(struct sockaddr*)&send_addr, sizeof(send_addr));

		if (ret == SOCKET_ERROR) {
			m_lastError = WSAGetLastError();
		}
		else {
			m_lastError = 0;
		}
	}

};

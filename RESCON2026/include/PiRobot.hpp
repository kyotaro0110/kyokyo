#pragma once
#include "IRobot.hpp"
#include "DeviceConfig.h"
#include "ControllerSender.hpp"
#include "SoundReceiver.hpp"
#include "SoundAnalyzer.hpp"
#include "AudioConfig.hpp"


class PiRobot : public IRobot {
private:
	DeviceInfo m_info;
	std::unique_ptr<ControllerSender> m_sender;
	std::unique_ptr<Controller> m_controller;
	std::unique_ptr<FftAnalyzer> m_fft;
	std::unique_ptr<SoundReceiver> m_audioreceiver;
	Font m_font{ 24, Typeface::Black };
public:
	PiRobot(const DeviceInfo& info)
		: m_info(info)
	{}

	bool initialize() override {

		//ネットワーク機能全体の開始
		/*WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;*/

		g_robotInstanceCount++;
		//ControllerSenderを生成し、内部でinitialize()を呼ぶ
		if (m_info.name == U"二号機")
			m_sender = std::make_unique<ControllerSender_2th>(m_info.toipAddress, m_info.commandPort);
		else
			m_sender = std::make_unique<ControllerSender>(m_info.toipAddress, m_info.commandPort); //スマートポインターで自動手delete robotをしてくれる
		m_controller = std::make_unique<Controller>();
		

		//m_audioreceiver = std::make_unique<SoundReceiver>(m_info.ipAddress, m_info.audioPort);
		m_fft = std::make_unique<FftAnalyzer>(m_info.ipAddress, m_info.audioPort);

		bool sSuccess = m_sender->initialize();
		bool aSuccess = m_fft->initialize();
		//bool aSuccess = m_audioreceiver->initialize();

		return (sSuccess && aSuccess);
	}

	void update(size_t MaxSpeed) override {
		if (m_sender) m_sender->update(MaxSpeed);
		//if (m_audioreceiver) m_audioreceiver->update();
		if (m_fft) m_fft->update();
	}

	bool isConnected() override {
		return m_sender->isConnected();
		//return true;
	}


	void draw(const Vec2& pos = { 20, 20}) const override {
		if (m_fft) {
			const float vol = m_fft->getVolume();
			//const float vol = m_audioreceiver->getVolume();

			m_font(U"Mic Level {}"_fmt(vol)).draw(pos, Palette::Black);
			//m_audioreceiver->drawgraph(RectF{ pos.x, pos.y + 40, 300, 100 });
		}
	}

	void analyzerUI(const RectF& region) const override
	{
		if (!m_fft) return;

		region.drawFrame(2, Palette::Black);

		const auto& amplitudes = m_fft->getAmplitudes();
		if (amplitudes.empty()) return;

		// 設定：0Hzから2000Hzまで、200Hz刻み
		const double maxHz = 2000.0;
		const double stepHz = 200.0;

		LineString points;

		// 0, 200, 400, ..., 2000Hz の各地点の振幅を取得して点を作る
		for (double hz = 0; hz <= maxHz; hz += stepHz) {
			// Hzからamplitudesのインデックスを計算
			size_t idx = static_cast<size_t>(hz * (amplitudes.size() * 2) / AudioConfig::m_samplingRate);

			// 配列の外を参照しないようガード
			idx = Min(idx, amplitudes.size() - 1);

			double rawVal = amplitudes[idx];
			if (rawVal < 0.5) rawVal = 0.0;

			double h = Min(rawVal * 50, region.h);

			// 横軸の座標計算 (0〜maxHz を region.w にマッピング)
			double xPos = region.x + (hz / maxHz) * region.w;
			double yPos = region.y + region.h - h;

			points.emplace_back(xPos, yPos);
		}

		// 折れ線を描画
		points.draw(2.0, Palette::Red);

		// 下に目盛り（数字）を表示
		for (double hz = 0; hz <= maxHz; hz += stepHz) {
			double xPos = region.x + (hz / maxHz) * region.w;

			// ガイドライン
			Line{ xPos, region.y, xPos, region.y + region.h }.draw(1, ColorF{ 0, 0.1 });
			// 数字
			m_font(U"{}"_fmt(static_cast<int>(hz))).drawAt(10, xPos, region.y + region.h + 15, Palette::Black);
		}
	}

	~PiRobot() {
		m_sender.reset();
		//m_audioreceiver.reset();
		/*g_robotInstanceCount--;
		if (g_robotInstanceCount == 0){
		WSACleanup();
		}*/
	}
};



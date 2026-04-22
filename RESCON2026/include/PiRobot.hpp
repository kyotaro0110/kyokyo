#pragma once
#include "IRobot.hpp"
#include "DeviceConfig.h"
#include "ControllerSender.hpp"
#include "SoundReceiver.hpp"
#include "SoundAnalyzer.hpp"
#include "AudioConfig.hpp"
#include "Siv3D.hpp"
//ラズパイで動くロボットのクラス。コントローラーの状態をUDPで送る機能と、マイクから音を拾う機能を持つ。
//マイクからの音のUIの描画もこのクラス内の関数で行う。

struct SignalPoint {
	double time;
	int32 state;
};

class PiRobot : public IRobot {
private:
	DeviceInfo m_info;
	std::unique_ptr<ControllerSender> m_sender;
	std::unique_ptr<Controller> m_controller;
	std::unique_ptr<FftAnalyzer> m_fft;
	std::unique_ptr<SoundReceiver> m_audioreceiver;
	// 設定：0Hzから2000Hzまで、200Hz刻み
	const double minHz = 100.0;
	const double maxHz = 3000.0;
	//const double stepHz = 0.1;
	const double stepHz = 0.5;
	static Array<Array<double>> history;
	mutable Array<SignalPoint> m_signalHistory;
	const size_t maxHistory = 400; // 0.1秒ごとの更新なら12秒分
	const double m_displaySeconds = 5.0;
	mutable double m_spectrumScale = 0.5; //mutableはconstの状況でも変更を許可する
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

	bool isFrequencyActive(double targetHz) const
	{
		size_t idx = static_cast<size_t>(targetHz * AudioConfig::SAMPLE_COUNT / AudioConfig::m_samplingRate);
		const auto& amplitudes = m_fft->getAmplitudes();

		if (idx >= amplitudes.size()) return false;
		double threshold = 3.0;

		if (targetHz < 100.0) threshold = 30.0;
		else if (targetHz < 500.0) threshold = 30.0;
		else if(targetHz < 1000.0) threshold = 6.0;
		else if(targetHz < 2000.0) threshold = 3.0;
		else threshold = 1.0;
		return (amplitudes[idx] > threshold);
	}

	void drawSignalHistory(const RectF& region, int ActiveRobot, int index,size_t freq_index) const
	{
		if (ActiveRobot == index) {
			if (!m_fft) return;

			double targetHz = AudioConfig::frequencyValues[freq_index];

			int32 currentSignal = isFrequencyActive(targetHz) ? 1 : 0;
			double currentTime = Scene::Time();
			m_signalHistory << SignalPoint{ currentTime, currentSignal };
			m_signalHistory.remove_if([&](const SignalPoint& p) {
				return p.time < (currentTime - m_displaySeconds);
			});

			for (const auto& p : m_signalHistory)
			{
				//右端が現時刻(0s),左へ行くほど過去
				//秒数を座標に変換
				double x = region.x + region.w - (currentTime - p.time) * (region.w / m_displaySeconds);

				if (x < region.x) continue;

				if (p.state == 1)
				{
					RectF{ x, region.y, 2, region.h }.draw(Palette::Orange);
				}
			}

			for (int i = 0; i <= (int)m_displaySeconds; ++i) {
				double x = region.x + region.w - i * (region.w / m_displaySeconds);
				Line{ x, region.y + region.h, x, region.y + region.h + 5 }.draw(1, Palette::Gray);
				m_font(U"-{}s"_fmt(i)).drawAt(12, x, region.y + region.h + 15, Palette::Black);
			}
		}
	}

	void analyzerUI(const RectF& region, int ActiveRobot, int index) const override
	{
		SimpleGUI::VerticalSlider(m_spectrumScale, Vec2{ region.x + region.w + 20, region.y + region.h - 200 }, 200);
		if (ActiveRobot == index) {
			if (!m_fft) return;

			const auto& amplitudes = m_fft->getAmplitudes();
			if (amplitudes.empty()) return;

			LineString points;

			// 0, 200, 400, ..., 2000Hz の各地点の振幅を取得して点を作る
			for (double hz = minHz; hz <= maxHz; hz += stepHz) {
				// Hzからamplitudesのインデックスを計算
				size_t idx = static_cast<size_t>(hz * (amplitudes.size() * 2) / AudioConfig::m_samplingRate);

				// 配列の外を参照しないようガード
				idx = Min(idx, amplitudes.size() - 1);

				double rawVal = 0.0;

				for (int32 offset = -1; offset <= 1; ++offset) {
					size_t checkIdx = static_cast<size_t>(Max<int32>(0, static_cast<int32>(idx) + offset));
					checkIdx = Min(checkIdx, amplitudes.size() - 1);
					rawVal = Max(rawVal, amplitudes[checkIdx]);
				}
				double threshold = 3.0;
				
				if (rawVal < threshold) rawVal = 0.0;

				double h = Min(rawVal * m_spectrumScale * 25.0, region.h); //region.hは

				// 横軸の座標計算 (0〜maxHz を region.w にマッピング)
				double xPos = region.x + (hz / maxHz) * region.w;
				double yPos = region.y + region.h - h;

				points.emplace_back(xPos, yPos);
			}

			// 折れ線を描画
			points.draw(2.0, Palette::Red);

			// 下に目盛り（数字）を表示
			for (double hz = minHz; hz <= maxHz; hz += stepHz) {
				double xPos = region.x + (hz / maxHz) * region.w;

				if ((int)hz % 200 == 0) {
					// ガイドライン
					Line{ xPos, region.y, xPos, region.y + region.h }.draw(1, ColorF{ 0, 0.1 });
					// 数字
					m_font(U"{}"_fmt(static_cast<int>(hz))).drawAt(10, xPos, region.y + region.h + 15, Palette::Black);
				}
			}

			if (region.mouseOver())
			{
				double relativeX = (Cursor::Pos().x - region.x) / region.w;
				double cursorHz = relativeX * maxHz;

				// インデックスに変換
				size_t idx = static_cast<size_t>(cursorHz * AudioConfig::SAMPLE_COUNT / AudioConfig::m_samplingRate);
				idx = Min(idx, amplitudes.size() - 1);

				double preciseHz = idx * (AudioConfig::m_samplingRate / AudioConfig::SAMPLE_COUNT);

				//垂直線を「ビンの中心」に引き直す（オプション。これで見栄えがよくなる)
				double preciseX = region.x + (preciseHz / maxHz) * region.w;

				//そのインデックスが持つ「正確な周波数」を逆算
				Line{ preciseX, region.y, preciseX, region.y + region.h }.draw(1, Palette::Gray);

				const String label = U"{:.1f} Hz"_fmt(preciseHz);
				// 文字の表示領域を計算し、少し広げて白背景にする
				//const auto regionText = m_font(label).regionAt(18, Cursor::Pos().x, region.y - 20);
				//regionText.stretched(4, 2).draw(Palette::White);
				double rawVal = 0.0;

				for (int32 offset = -1; offset <= 1; ++offset) {
					size_t checkIdx = static_cast<size_t>(Max<int32>(0, static_cast<int32>(idx) + offset));
					checkIdx = Min(checkIdx, amplitudes.size() - 1);
					rawVal = Max(rawVal, amplitudes[checkIdx]);
				}
				m_font(label,rawVal).drawAt(18, preciseX, region.y+region.h/2, Palette::Black);
			}
			
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



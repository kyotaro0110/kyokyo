#pragma once
#include "Fft.hpp"
#include "SoundReceiver.hpp"

class FftAnalyzer : public SoundReceiver {
private:
	Fft m_fft; //継承せず、道具として「持っておく」
	// FFT解析用バッファ(実数部と虚数部)
	std::vector<double> m_fftReal;
	std::vector<double> m_fftImag;

	//解析後の振幅(大きさ)を格納する配列
	std::vector<double> m_amplitudes;

	//表示用設定
	
	Font m_font{ FontMethod::MSDF, 20, Typeface::Bold };

	const double m_samplingRate = 44100.0; //一般的なサンプリング周波数
public:
	//親クラス SoundReceiver のコンストラクタを呼ぶ
	FftAnalyzer(const String& _ip, uint16_t _port)
		: SoundReceiver(_ip, _port), m_fft(AudioConfig::SAMPLE_COUNT)
	{
		m_fftReal.resize(AudioConfig::SAMPLE_COUNT);
		m_fftImag.resize(AudioConfig::SAMPLE_COUNT);
		m_amplitudes.resize(AudioConfig::SAMPLE_COUNT / 2); //FFTの結果は半分で
	}

	void update() override
	{
		SoundReceiver::update();
		for (int i = 0; i < AudioConfig::SAMPLE_COUNT; ++i)
		{
			double window = 0.42 - 0.5 * std::cos(2.0 * Math::Pi * i / (AudioConfig::SAMPLE_COUNT - 1))
				+ 0.08 * std::cos(4.0 * Math::Pi * i / (AudioConfig::SAMPLE_COUNT - 1));
			m_fftReal[i] = static_cast<double>(m_buffer[i]) / 32767.0 * window; 
			m_fftImag[i] = 0.0; //音声信号に虚部はないので0固定
		}

		m_fft.transform(m_fftReal, m_fftImag);

		for (int i = 0; i < (AudioConfig::SAMPLE_COUNT / 2); ++i)
		{
			m_amplitudes[i] = std::sqrt(m_fftReal[i] * m_fftReal[i] + m_fftImag[i] * m_fftImag[i]);
		}

		double maxAmp = 0.0; //音の強さの最大値
		size_t peakIndex = 0; //そのときのインデックス

		//最大値の計算
		for (size_t i = 5; i < m_amplitudes.size(); ++i) {
			if (m_amplitudes[i] > maxAmp) {
				maxAmp = m_amplitudes[i];
				peakIndex = i;
			}
		}

		double peakHz = peakIndex * (AudioConfig::m_samplingRate / AudioConfig::SAMPLE_COUNT); //音の強さの最大値となるときのインデックスを使用して、周波数を特定

		//if (Scene::FrameCount() % 60 == 0) {

		//	if (maxAmp > 0.1)
		//		//Print << U"Peak: " << peakHz << U"Hz";
		//}


	}

	double getStrength(double targetHz) const
	{
		// index = Hz * N / SamplingRate
		size_t index = static_cast<size_t>(targetHz * AudioConfig::SAMPLE_COUNT / AudioConfig::m_samplingRate);
		if (index >= m_amplitudes.size()) return 0.0;
		return m_amplitudes[index];
	}


	// 拾った音の中で「一番強い周波数」を表示する
	void drawPeakFrequency(const RectF& region) const
	{
		double maxAmp = 0.0;
		int32 peakIndex = 0;

		// 低すぎるノイズ（インデックス0〜5程度）を除外して、一番強い場所を探す
		for (int32 i = 5; i < (int32)m_amplitudes.size(); ++i)
		{
			if (m_amplitudes[i] > maxAmp)
			{
				maxAmp = m_amplitudes[i];
				peakIndex = i;
			}
		}

		// インデックスをHzに変換 (公式: index * samplingRate / SAMPLE_COUNT)
		// SAMPLE_COUNT = 4096, m_samplingRate = 44100.0
		double peakHz = peakIndex * (m_samplingRate / AudioConfig::SAMPLE_COUNT);

		// 画面に描画
		// 一定以上の大きさの音が鳴っている時だけ表示するようにすると見やすい
		if (maxAmp > 0.1) // 閾値（感度）は環境に合わせて調整してください
		{
			Print << U"Peak: {:.1f} Hz (Amp: {:.4f}"_fmt(peakHz, maxAmp);

			// 音階（ドレミ）も知りたい場合は、簡易的に判定も可能
			if (peakHz > 430 && peakHz < 450) m_font(U"(A4 / ラ)").draw(region.x, region.y + 30);
		}
		else
		{
			m_font(U"待機中... (静かです)").draw(region.x, region.y, Palette::Gray);
		}
	}
	// FftAnalyzer.hpp の public: セクションに追加
	const std::vector<double>& getAmplitudes() const {
		return m_amplitudes;
	}
};

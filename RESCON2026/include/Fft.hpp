#pragma once
#include <vector>
#include <complex>
#include <stdexcept>

#ifndef M_PI
#define M_PI 3.1415926535323846
#endif

/*FFTの手順
1.ビット演算をして並び替えをする。（バタフライ演算をするために必要)
2.バタフライ演算(２つの入力データから、２つの出力データを同時に作り出す計算の最小単位)
*/
class Fft {
private:
	size_t m_n; //FFTで一度に処理するデータの個数
	//事前計算用のテーブル(回転因子)
	std::vector<double> m_cosTable;
	std::vector<double> m_sinTable;

public:
	//コンストラクタで解析サイズを指定し、テーブルを生成する
	explicit  Fft(size_t n) : m_n(n) {
		if (n == 0 || ((n & (n - 1)) != 0)) { //n==0かn(2) AND n-1(2) != 0なら(サイズが2のべき乗でないと高速フーリエ変換はできない（分割統治法）を採用するので
			throw std::invalid_argument("FFT size must be a power of 2."); //throwは問題があったときにreturnせずに一時止めて報告することができる
		}

		//回転因子の事前計算(ここでsin cosの計算をすることによって高速化できる
		m_cosTable.resize(n / 2); //resizeとは動的配列の要素数を、指定したサイズに強制的に変更する
		m_sinTable.resize(n / 2);

		for (size_t i = 0; i < n / 2; ++i) { //size_tは負の数を取らないためvectorと相性がいい
			m_cosTable[i] = std::cos(-2.0 * M_PI * i / n);
			m_sinTable[i] = std::sin(-2.0 * M_PI * i / n);
		}
	}

	//変換実行
	void transform(std::vector<double>& real, std::vector<double>& imag) const {
		if (real.size() != m_n || imag.size() != m_n) {
				throw std::invalid_argument("input vector size mismatch."); //invalid_argumentは関数の引数が不正であることを伝えるためのエラー専用の型
		}

		//1. ビット反転(バタフライ演算が効率よくできるようになる)
		size_t j = 0; //iをビット反転させた後のインデックス番号
		for (size_t i = 0; i < m_n; i++) {
			if (i < j) {
				std::swap(real[i], real[j]); //バラバラな順番で入ってくるデータをFFTが計算しやすい特定の順番に整理整頓するため
				std::swap(imag[i], imag[j]);
			}
			size_t m = m_n >> 1; //右ビットシフト演算
			while (m >= 1 && j >= m) {
				j -= m;
				m >>= 1;
			}
			j += m;
		}

		//2.バタフライ演算(事前計算テーブルを使用) サイズは２のべき乗しか値を取ることができない、理由は２つずつ分けていくから。
		for (size_t size = 2; size <= m_n; size *= 2) { //サイズを広げていく
			const size_t halfsize = size / 2;
			const size_t step = m_n / size; //何番目の箱にいるか

			for (size_t i = 0; i <  m_n; i += size) { //データ全体の端から端までをスキャンする。
				for (size_t k = 0; k < halfsize; k++) { //データの半分まででOK
					const double cos_v = m_cosTable[k * step];
					const double sin_v = m_sinTable[k * step];

					const size_t even = i + k;
					const size_t odd = i + k + halfsize;

					//回転因子 
					const double tr = real[odd] * cos_v - imag[odd] * sin_v; //実数部分
					const double ti = real[odd] * sin_v + imag[odd] * cos_v; //虚数部分

					real[odd] = real[even] - tr;
					imag[odd] = imag[even] - ti;
					real[even] += tr;
					imag[even] += ti;
				}
			}
		}
	}
};

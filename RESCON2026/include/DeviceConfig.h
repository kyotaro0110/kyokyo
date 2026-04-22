//PCのIPアドレス
//RRC23使用時：	白井：192.168.23.1
//				後藤：192.168.23.2
//イーサネット：白井：192.168.0.1
//				後藤：192.168.0.2

//ラズパイのIPアドレス
//RRC23使用時： 一号機：192.168.23.111
//				二号機：192.168.23.112
//イーサネット：一号機：192.168.0.111
//				二号機：192.168.0.112

#pragma once
#include "stdafx.h"

inline int g_robotInstanceCount = 0;

// デバイスの種類を識別するためのenum
enum class DeviceType
{
	RaspberryPi,
	TPIP4,
};

// 各デバイスの接続情報を保持する構造体
struct DeviceInfo
{
	s3d::String name;        // 識別用の名前
	DeviceType type;         // デバイス種別
	s3d::String ipAddress;   // IPアドレス
	s3d::String toipAddress; // 送信先IPアドレス
	uint16 videoPort;        // 映像ポート
	uint16 audioPort;        // 音声ポート
	uint16 commandPort;      // コマンド送信用ポート
};

// アプリケーションで使用する全デバイスのリスト
namespace DeviceConfig_RRC23
{
	//RRC23使用時
	const s3d::Array<DeviceInfo> g_devices_RRC23 =
	{
		{ U"一号機", DeviceType::RaspberryPi, U"192.168.23.1", U"192.168.23.111", 8081, 8091, 9001},//IPアドレス：192.168.23.1と送受信
		{ U"二号機", DeviceType::RaspberryPi, U"192.168.23.1", U"192.168.23.111", 8082, 8092, 9002 },//IPアドレス：192.168.23.2と送受信
		{ U"三号機", DeviceType::RaspberryPi, U"192.168.23.1", U"192.168.23.113", 0, 8091, 9003},
		{ U"四号機", DeviceType::RaspberryPi, U"192.168.23.1", U"192.168.23.113", 0, 0, 9004},
	};

	//イーサネット使用時(有線)
	const s3d::Array<DeviceInfo> g_devices_eth =
	{
		{ U"一号機", DeviceType::RaspberryPi, U"192.168.0.111", U"172.20.10.4", 8081, 8091, 9001 },//IPアドレス：192.168.0.1と送受信
		{ U"二号機", DeviceType::RaspberryPi, U"192.168.0.112", U"172.20.10.4", 8082, 8092, 9002 },//IPアドレス：192.168.0.2と送受信
		{ U"三号機", DeviceType::RaspberryPi, U"192.168.0.200", U"10.160.177.54", 0, 0, 9001 },
		{ U"四号機", DeviceType::RaspberryPi,       U"192.168.0.200", U"-", 0, 0, 9001 },
	};

}

//イーサネット使用時
//namespace DeviceConfig_eth
//{
//	// アプリケーションで使用する全デバイスのリスト
//	// 添付いただいたPNG画像の情報を反映
//	const s3d::Array<DeviceInfo> g_devices =
//	{
//		{ U"一号機 (RPi)", DeviceType::RaspberryPi, U"192.168.0.111", 8081, 8091, 9001 },//IPアドレス：192.168.0.2と送受信（予定）
//		{ U"二号機 (RPi)", DeviceType::RaspberryPi, U"192.168.0.112", 8082, 8092, 9001 },//IPアドレス：192.168.0.1と送受信
//		{ U"三号機 (TPIP4)", DeviceType::TPIP4,       U"192.168.0.200", 0, 0, 9001 },
//		{ U"四号機 (TPIP4)", DeviceType::TPIP4,       U"192.168.0.200", 0, 0, 9001 },
//	};
//}

#pragma once
#include "stdafx.h"

class UDP {
protected:
	std::string ip;
	uint16_t port;
	struct sockaddr_in send_addr,recv_addr;
	SOCKET send_socket;

public:
	UDP(const std::string& _ip, uint16_t _port)
		: ip(_ip), port(_port), send_socket(INVALID_SOCKET) {
		memset(&send_addr, 0, sizeof(send_addr));
	}

	virtual ~UDP() {
		if (send_socket != INVALID_SOCKET) {
			closesocket(send_socket);
		}
		//WSACleanup();
	}

	virtual bool initialize()
	{
		//WSADATA wsaData;
		/*if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			return false;
		}*/
		//ソケットの作成
		send_socket = socket(AF_INET, SOCK_DGRAM, 0);
		if (send_socket == INVALID_SOCKET) {
			return false;
		}

		int yes = 1;
		if (setsockopt(send_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes)) == SOCKET_ERROR) {
			return false;	
		}
		//送信先アドレスの設定
		memset(&send_addr, 0, sizeof(send_addr));
		send_addr.sin_family = AF_INET;
		send_addr.sin_port = htons(port);
		send_addr.sin_addr.s_addr = inet_addr(ip.c_str());
		//自分の待ち受け設定
		memset(&recv_addr, 0, sizeof(recv_addr));
		recv_addr.sin_family = AF_INET;
		recv_addr.sin_port = htons(port);
		recv_addr.sin_addr.s_addr = INADDR_ANY;

		//接続できなかったらエラー表示
		if (bind(send_socket, (struct sockaddr*)&recv_addr, sizeof(recv_addr)) == SOCKET_ERROR) {
			//Print << U"bind failure: " << WSAGetLastError();
			return false;
		}

		//ノンブロッキング設定
		u_long val = 1;
		ioctlsocket(send_socket, FIONBIO, &val);

		return true;

	}
};



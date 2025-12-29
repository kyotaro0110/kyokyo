#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<stdio.h>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<string.h>
#include<windows.h>
#include<iostream>
#include<vector>

#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "winmm.lib")

using namespace std;
//送信側のソース
char ip[16] = "192.168.23.113";
int port = 9600;
char send_buf[16];


int main() {
	JOYINFOEX JoyInfoEx;
	JoyInfoEx.dwSize = sizeof(JOYINFOEX);
	JoyInfoEx.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY;



	for (unsigned int i = 0; i < joyGetNumDevs(); i++) {//サポートされているジョイスティックの数を返す
		if (JOYERR_NOERROR == joyGetPosEx(i, &JoyInfoEx))
			printf("ジョイスティック No.%d　接続されています\n", i);
	}
	Sleep(1000);

	WSADATA wsa_data;
	if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0) {
		printf("初期化失敗");
	}
	BOOL yes = 1;
	struct sockaddr_in send_addr;//送信用ソケット設定
	memset(&send_addr, 0, sizeof(send_addr));
	send_addr.sin_port = htons(port);
	send_addr.sin_family = AF_INET;//IPv4
	send_addr.sin_addr.s_addr = inet_addr(ip);
	SOCKET send_socket = socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(send_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));
	if (send_socket < 0) {
		printf("dst didnt make");
		return -1;
	}

	/*	int num;
		while (true) {
			//printf("入力待ち\n");
			//scanf_s("%s", send_buf, 16);

			//num = sendto(send_socket, send_buf, 16, 0, (LPSOCKADDR)&send_addr, sizeof(send_addr));
			if (num < 0) {
				printf("送信失敗\n");
				break;
			}

		}*/

	while (true) {
		printf("入力待ち\n");
		Sleep(1000);
		
		if (JOYERR_NOERROR == joyGetPosEx(0, &JoyInfoEx)) {
			if (JoyInfoEx.dwXpos < 31487) {
				if (JoyInfoEx.dwYpos < 32511) {
					printf("northwest");
					Sleep(200);
				}
				else if (JoyInfoEx.dwYpos > 32511) {
					printf("southwest");
					Sleep(200);
				}
				else if (JoyInfoEx.dwYpos = 32511) {
					printf("west");
					Sleep(200);
				}
			}
			else if (JoyInfoEx.dwXpos > 31487) {
				if (JoyInfoEx.dwYpos < 32511) {
					printf("northeast");
					Sleep(200);
				}
				else if (JoyInfoEx.dwYpos > 32511) {
					printf("southeast");
					Sleep(200);
				}
				else if (JoyInfoEx.dwYpos = 32511) {
					printf("east");
					Sleep(200);
				}
			}
			else if (JoyInfoEx.dwXpos = 31487) {
				if (JoyInfoEx.dwYpos < 32511) {
					printf("north");
					Sleep(200);
				}
				else if (JoyInfoEx.dwYpos > 32511) {
					printf("south");
					Sleep(200);
				}
				else if (JoyInfoEx.dwYpos == 32511) {
					printf("center");
					Sleep(200);
				}
			}
			
		}
	}
}

	
	

		
	

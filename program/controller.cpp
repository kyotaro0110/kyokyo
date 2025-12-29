#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<stdio.h>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<string.h>
#include<windows.h>
#include<iostream>
#include<vector>
#include<bitset>

#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#define centerx 30463
#define centerx1 30719
#define centery 32767
#define centerz 31999
#define centerr 32511
#define uppery 10000
#define downy  60000
#define westx  5000
#define eastx  60000


using namespace std;
//送信側のソース
char ip[16] = "192.168.3.104";
int port = 8080;
//char send_buf[16];

void printbutton(int num) {
	const char* character[8] = { "□","×","◯","△","L1","R1","L2","R2" };
	for (int i = 0; i < 8; i++) {
		if (num & (1 << i)) {
			cout << character[i] << " ";

		}


	}
}

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

	/* int num;
		while (true) {
			//printf("入力待ち\n");
			scanf_s("%s", send_buf, 16);

			num = sendto(send_socket, send_buf, 16, 0, (LPSOCKADDR)&send_addr, sizeof(send_addr));
			if (num < 0) {
				printf("送信失敗\n");
				break;
			}

		}*/

	while (true) {
		printf("入力待ち\n");
		Sleep(1000);

		if (JOYERR_NOERROR == joyGetPosEx(0, &JoyInfoEx)) {
			const char* direction = nullptr;

			if (JoyInfoEx.dwYpos < uppery) {
				direction = "W";

			}
			else if (JoyInfoEx.dwYpos > downy) {
				direction = "S";
			}
			else if (JoyInfoEx.dwXpos < westx) {
				direction = "A";
			}
			else if (JoyInfoEx.dwXpos > eastx) {
				direction = "D";
			}

			if (direction) {
				cout << direction << " ";
			}
			printbutton(JoyInfoEx.dwButtons);
			cout << endl;

			const char* character[8] = { "SQUARE", "CROSS", "CIRCLE", "TRIANGLE","L1","R1","L2","R2" };
			char send_buf[64] = "";  

			if (direction) {
				strcpy_s(send_buf, direction);
				strcat_s(send_buf, " ");
			}
			else {
				strcpy_s(send_buf, " ");
			}

			for (int i = 0; i < 8; i++) {
				if (JoyInfoEx.dwButtons & (1 << i)) {
					strcat_s(send_buf, character[i]);
					strcat_s(send_buf, " ");
				}
			}


			int ret = sendto(send_socket, send_buf, strlen(send_buf), 0,
				(LPSOCKADDR)&send_addr, sizeof(send_addr));
			if (ret < 0) {
				perror("送信失敗");
			}
		}
	}
}







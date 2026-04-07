# pragma once

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

//ネットワーク関連
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<windows.h>

#include <opencv2/opencv.hpp>
#include <portaudio.h>
#include<string.h>
#include<vector>
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"winmm.lib")






//# define NO_S3D_USING
# include <Siv3D.hpp>

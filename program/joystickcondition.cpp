#pragma comment(lib, "winmm.lib")

#include <stdio.h>
#include<iostream>
#include <windows.h>
#include<vector>

using namespace std;

char a[4] = { 'a','b','c','d' };
int main() {

	JOYINFOEX JoyInfoEx;
	JoyInfoEx.dwSize = sizeof(JOYINFOEX);
	JoyInfoEx.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY;

	for (unsigned int i = 0; i < joyGetNumDevs(); i++) {//サポートされているジョイスティックの数を返す
		if (JOYERR_NOERROR == joyGetPosEx(i, &JoyInfoEx))
			printf("ジョイスティック No.%d　接続されています\n", i);
	}
	Sleep(1000);

	while (1) {
		if (JOYERR_NOERROR == joyGetPosEx(0, &JoyInfoEx)) { //0番のジョイスティックの情報を見る
			printf("dwXpos = %d\t"      //10進数表記
				"dwYpos = %d\t"
				"dwZpos = %d\t"
				"dwRpos = %d\t"
				"dwButtons = %d\t"
				"dwPOV = %d\n",
				JoyInfoEx.dwXpos,
				JoyInfoEx.dwYpos,
				JoyInfoEx.dwZpos,
				JoyInfoEx.dwRpos,
				JoyInfoEx.dwButtons,
				JoyInfoEx.dwPOV);
			for (int i = 0; i < 4; i++) {
				if (JoyInfoEx.dwButtons & 1 << i) {
					cout << a[i] << ' ';

				}

			}



			/*printf("dwXpos = 0x%x\t"  //16進数表記
				"dwYpos = 0x%x\t"
				"dwZpos = 0x%x\t"
				"dwRpos = 0x%x\t"
				"dwButtons = 0x%x\n",
				JoyInfoEx.dwXpos,
				JoyInfoEx.dwYpos,
				JoyInfoEx.dwZpos,
				JoyInfoEx.dwRpos,
				JoyInfoEx.dwButtons);
			*/
		}
		else {
			printf("エラー\n");
		}
		Sleep(100);
	}
	return 0;
}

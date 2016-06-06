//105/05/07 完成fps.h
#pragma once
#include <stdio.h>
class FPS {
private:
	DWORD start, last, cur;
	int count, fps;
	char fps_char[12];
public:
	//建構式
	FPS() { last = cur = start = GetTickCount(); count = 0; fps = 0; }
	//放在迴圈中，計算每秒執行了幾次這個迴圈
	void counter() {
		last = cur;
		cur = GetTickCount();
		count++;
		if (cur - start >= 1000) {
			fps = count;
			count = 0;
			start += 1000;
		}
	}
	//回傳fps
	int getFPS() const { return fps; }
	//計算執行兩次 counter 所經過的時間
	DWORD getDur() const { return cur - last; }
	//在螢幕上顯示fps
	void draw(HDC& hdc, int x, int y) {
		sprintf_s(fps_char, "fps:%d", fps);
		TextOut(hdc, x, y, fps_char, strlen(fps_char));
	}
};

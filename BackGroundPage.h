#ifndef BACKGROUNDPAGE_H
#define BACKGROUNDPAGE_H
class BackGroundPage {
private:
	//背景頁的高度及寬度
	int BACK_DC_W, BACK_DC_H;
	//宣告背景畫布
	HDC backDC;
	//宣告背景點陣圖
	HBITMAP backBMP;
	//白色筆刷
	HBRUSH whiteBrush;
public:

	//使用 BackGroundPage 類別前一定要先呼叫一次，需要設定背景頁大小
	void init(int w, int h) {
		BACK_DC_W = w;
		BACK_DC_H = h;
		//準備背景頁
		HDC screenDC = GetDC(NULL);
		backDC = CreateCompatibleDC(screenDC);
		ReleaseDC(NULL, screenDC);
		backBMP = CreateBitmap(BACK_DC_W, BACK_DC_H, 1, 32, NULL);
		SelectObject(backDC, backBMP);
		//準備白色筆刷
		whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
	}

	//將背景頁洗白
	void clear() {
		SelectObject(backDC, whiteBrush);
		Rectangle(backDC, 0, 0, BACK_DC_W, BACK_DC_H);
	}
	//將 backDC 的內容畫到 hdc 上
	void draw(HDC hdc) {
		BitBlt(hdc, 0, 0, BACK_DC_W, BACK_DC_H, backDC, 0, 0, SRCCOPY);
	}
	//回傳 backDC 參照，用以接收其他畫圖函式所畫的圖
	HDC& getbackDC() {
		return this->backDC;
	}
};
#endif

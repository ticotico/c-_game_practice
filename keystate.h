//105/05/07 完成keystate.h
//105/05/27 加入GetMousePos
#pragma once
class KeyState {
private:
	int key;		//按鍵編號
	bool pre_press;	//上一次按鍵狀態
	bool cur_press;	//這一次按鍵狀態
public:
	KeyState() : key(VK_SPACE) { init(); }
	KeyState(int k) : key(k) { init(); }
	void init()
	{
		pre_press = FALSE;
		cur_press = FALSE;
	}
	void SetKey(int k) { key = k; }
	//測試並記錄按鍵目前的狀態
	void Test()
	{
		pre_press = cur_press;
		cur_press = (GetAsyncKeyState(key) < 0);
	}
	//測試時是否為按鍵壓下去的瞬間
	bool IsPressDownMoment()
	{return (pre_press == FALSE && cur_press == TRUE);}
	//測試時是否為按鍵放開的瞬間
	bool IsReleaseMoment()
	{return (pre_press == TRUE && cur_press == FALSE);}
	//測試時是否壓著按鍵
	bool IsPressNow()
	{return cur_press;}
};

class KeyContinuous : public KeyState {
private:
	DWORD prev_t;		//上一次檢查按鍵狀態的時間
	DWORD cur_t;		//這一次檢查按鍵狀態的時間
public:
	KeyContinuous() { init(); }
	KeyContinuous(int k) { init(); SetKey(k); }
	void init()
	{
		prev_t = GetTickCount();
		cur_t = prev_t;
	}
	bool isContinuous(DWORD t)
	{
		KeyState::Test();
		if (IsPressNow() == FALSE) return FALSE;
		if (IsPressDownMoment())
		{
			prev_t = GetTickCount();
			return TRUE;
		}
		cur_t = GetTickCount();

		if (cur_t - prev_t >= t) {
			prev_t += t;
			return TRUE;
		}
		else return FALSE;
	}


};

//取得滑鼠在視窗hWnd的座標，輸出mx和my
void GetMousePos(HWND hWnd, int& mx, int& my)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);
	mx = pt.x;
	my = pt.y;
}

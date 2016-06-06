#ifndef WINDOW_MAIN_H
#define WINDOW_MAIN_H
#include <cassert>
#include "window.h"
#include "windowMessage.h"

class WindowMain {
private:
	WindowMessage wMainMsg; //視窗訊息
	typedef std::list<Window*> VP_window;
	VP_window vpwindow; //存放視窗指標的容器
	VP_window::iterator pWorking;//紀錄拖曳中的視窗
	int premx, premy;//滑鼠上一次的座標
	KeyState mousel;//滑鼠左鍵的狀態
	Window* tracingWindow;//正在追蹤的視窗
public:
	WindowMain() : tracingWindow(NULL) {}
	void init() {
		premx = 0;
		premy = 0;

		mousel.init();
		mousel.SetKey(MK_LBUTTON);
	}
	void work(HWND hWnd) {
		mousel.Test();
		int mx, my;//紀錄目前滑鼠座標
		GetMousePos(hWnd, mx, my);

		switch (wMainMsg.getmsg()) {
		case WMSG_IDLE:
			//所有視窗閒置中
			if (mousel.IsPressNow()) {
				//按下滑鼠的瞬間，檢查滑鼠所在位置是否有視窗
				find_working_window(mx, my);
			}
			//只要IDLE狀態，就記錄滑鼠位置
			premx = mx;
			premy = my;
			break;
		case WMSG_DRAG:
			//視窗拖曳中
			if (!mousel.IsPressNow()) {
				//放開滑鼠，結束拖曳
				wMainMsg.setDrag(FALSE);
			}
			else if ((*pWorking)->isDragArea(premx, premy)) {
				//移動視窗
				int dx = mx - premx;
				int dy = my - premy;
				(*pWorking)->shift(dx, dy);
				premx = mx;
				premy = my;
			}
			break;
		case WMSG_TRACING:
			//視窗追蹤中
			if (!mousel.IsPressNow()) {
				//放開滑鼠，結束追蹤
				wMainMsg.setTracing(FALSE);
				tracingWindow = NULL;
			}
			else {
				tracingWindow = wMainMsg.getWorkingWindow();
				tracingWindow->tracing(mx, my);
			}
			break;
		default:
			//例外狀況
			assert(TRUE);
		}
	}
	void draw(HDC hdc) {
		VP_window::reverse_iterator drawit = vpwindow.rbegin();
		while (drawit != vpwindow.rend()) {
			(*drawit)->draw(hdc, 0, 0);
			++drawit;
		}
	}

	void addWindow(Window* pwindow) {
		vpwindow.push_back(pwindow);
	}

private:
	void find_working_window(int mx, int my) {
		pWorking = vpwindow.begin();
		while (pWorking != vpwindow.end()) {
			if ((*pWorking)->checkInside(premx, premy)) {
				//滑鼠的位置有視窗
				wMainMsg.setDrag(TRUE);//預設回傳拖曳訊息
				(*pWorking)->onClick(mx, my, wMainMsg);
				Window* tmp = *pWorking;
				vpwindow.erase(pWorking);
				vpwindow.push_front(tmp);
				pWorking = vpwindow.begin();
				break;
			}
			++pWorking;
		}
	}
};

#endif

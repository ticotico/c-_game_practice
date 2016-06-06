#ifndef _WINDOW_H_
#define _WINDOW_H_
#include <list>
#include <vector>
#include <windows.h>
#include <cstring>
#include <cassert>
#include "windowMessage.h"

class Window {
private:
	bool bVisible;	//視窗的能見度
	bool frameVisible; //視窗邊框的能見度
	typedef std::list<Window*> VP_window;
	VP_window VChild; //存放子視窗的容器
	Window* pParent; //指向自己的父視窗
protected:
	int x, y, w, h;//視窗的xy座標和寬度、高度
	

	//滑鼠座標轉視窗座標
	void CursorToWindow(int& mx, int& my) {
		mx -= x;
		my -= y;
	}
	//設定自己的父視窗
	void setParentWindow(Window* p) {
		pParent = p;
	}
	//將滑鼠座標轉視窗中的滑鼠座標
	void getWindowMousePos(int& mx, int &my) {
		mx -= x; my -= y;
		if (pParent == NULL) return;
		pParent->getWindowMousePos(mx, my);
	}
public:
	virtual ~Window() {}
	Window() : bVisible(TRUE), frameVisible(TRUE), pParent(NULL) {}
	//設定視窗的座標與大小
	virtual void init(int _x, int _y, int _w, int _h) {
		x = _x;
		y = _y;
		w = _w;
		h = _h;
	}

	//檢查滑鼠是否在視窗裡面(使用滑鼠相對於上一層視窗的座標)
	bool checkInside(int mx, int my) {
		CursorToWindow(mx, my);
		return mx >= 0 && mx <= w && my >= 0 && my <= h;
	}
	//移動視窗
	void shift(int dx, int dy) {
		x += dx; y += dy;
	}
	//設定視窗座標
	void setPos(int _x, int _y) {
		x = _x; y = _y;
	}
	//設定視窗大小
	void setSize(int _w, int _h) {
		w = _w; h = _h;
	}
	//畫出視窗
	virtual void draw(HDC hdc, int ox, int oy) {
		if (!isVisible()) return;
		if (isFrameVisible())
			Rectangle(hdc, ox + x, oy + y, ox + x + w, oy + y + h);
		VP_window::iterator it = VChild.begin();
		while (it != VChild.end()) {
			(*it)->draw(hdc, ox + x, oy + y);
			++it;
		}
	}

	//滑鼠是否在視窗拖曳區
	virtual bool isDragArea(int mx, int my) {
		return FALSE;
	}

	//增加子視窗
	void addChhild(Window* child) {
		assert(child->x >= 0);
		assert(child->y >= 0);
		assert(child->x + child->w <= w && child->y + child->h <= h);
		child->setParentWindow(this);
		VChild.push_back(child);
	}

	//滑鼠左鍵對視窗做點擊動作
	virtual void onClick(int mx, int my, WindowMessage& wmsg) {
		if (!isVisible()) return;
		//內容為空，只檢查是否點子視窗
		CursorToWindow(mx, my);
		VP_window::iterator it = VChild.begin();
		while (it != VChild.end()) {
			if ((*it)->checkInside(mx, my)) {
				//如果滑鼠在子視窗上
				(*it)->onClick(mx, my, wmsg);
				break;
			}
			++it;
		}
	}
	//視窗是否可見
	bool isVisible() {
		return bVisible;
	}
	//設定視窗的能見與否
	void setVisible(bool b) {
		bVisible = b;
	}
	//回傳視窗的邊框是否能見
	bool isFrameVisible() {
		return frameVisible;
	}
	//設定視窗邊框的能見度
	void setFrameVisible(bool b) {
		frameVisible = b;
	}
	//追蹤視窗時的工作函數
	virtual void tracing(int mx, int my) {
		//預設為空
	}
	//回傳父親視窗
	Window* getParentWindow() {
		return pParent;
	}
	int getH() {
		return h;
	}
	int getW() {
		return w;
	}
	int getX() {
		return x;
	}
	int getY() {
		return y;
	}

	virtual void update() {
		VP_window::iterator it = VChild.begin();
		while (it != VChild.end()) {
			(*it)->update();
			++it;
		}
	}
};

class TextWindow : public Window {
private:
	enum { textsize = 128 };
	char text[textsize];
public:
	//初始建構式，設定文字內容為空
	TextWindow() { text[0] = '\0'; setFrameVisible(FALSE); }
	//畫出文字視窗
	void draw(HDC hdc, int ox, int oy) {
		if (!Window::isVisible()) return;
		if (isFrameVisible())
			Window::draw(hdc, ox, oy);
		TextOut(hdc, ox + x + 2, oy + y + 2, text, strlen(text));
	}
	//設定文字視窗的文字以及是否顯示邊框
	void setText(const char* source, bool bShowFrame) {
		setFrameVisible(bShowFrame);
		strncpy_s(text, source, textsize);
	}
	//清除文字視窗內的文字
	void clear() {
		text[0] = '\0';
	}
};

class TabWindow : public Window {
private:
	//一個分頁的單位
	class SubTab {
	public:
		TextWindow info;	//分頁上方的文字視窗
		Window* psub;		//下方的分頁視窗
	public:
		//加入視窗時刷新info資訊
		void updateInfo(int _x, int _y, int _w, int _h) {
			info.setPos(_x, _y);
			info.setSize(_w, _h);
			info.setFrameVisible(TRUE);
			info.setVisible(TRUE);
		}
		//加入視窗時刷新sub資訊
		void updateSub() {
			psub->setFrameVisible(FALSE);
			psub->setVisible(FALSE);
		}
		//這個分頁會顯示
		void enable() {
			info.setFrameVisible(FALSE);
			psub->setVisible(TRUE);
		}
		//這個分頁不顯示
		void disable() {
			info.setFrameVisible(TRUE);
			psub->setVisible(FALSE);
		}
	};
	typedef std::list<SubTab> V_TabWindow;
	V_TabWindow vsub; //存放分頁的容器
	int pageH;//info視窗高度
	V_TabWindow::iterator pageNow;//現在作用中分頁(只有這頁會顯示)

protected:
	void update() {
		int subinfoW = w / vsub.size();//分頁info寬度，隨info數量動態調整
		int nth = 0;
		V_TabWindow::iterator iter = vsub.begin();
		while (iter != vsub.end()) {
			iter->updateInfo(nth*subinfoW, 0, subinfoW, pageH);
			iter->updateSub();
			++iter;
			++nth;
		}
		pageNow->enable();
		Window::update();
	}
	
public:
	//初始化tab主視窗大小以及tab頁高度
	void init(int _x, int _y, int _w, int _h, int _pageH)//初始化tab視窗參數，其中pageH為分頁info視窗高度
	{
		pageH = _pageH;
		Window::init(_x, _y, _w, _h);
	}
	//加入一個文字欄以及一個對應文字欄的視窗
	void addSub(const char* infoname, Window* pWind) {
		SubTab tmp;
		tmp.info.init(0, 0, 10, 10);
		tmp.info.setText(infoname, TRUE);
		pWind->setPos(0, pageH);
		tmp.psub = pWind;
		vsub.push_back(tmp);
		pageNow = vsub.begin();
		SubTab* tmp2 = &(vsub.back());
		Window::addChhild(&(tmp2->info));
		Window::addChhild(tmp2->psub);
		update();
	}
	//執行滑鼠左鍵tab視窗的點擊動作
	
	void onClick(int mx, int my, WindowMessage& wmsg) {
		if (!isVisible()) return;
		Window::onClick(mx, my, wmsg);
		CursorToWindow(mx, my);
		V_TabWindow::iterator iter = vsub.begin();
		while (iter != vsub.end()) {
			if (iter->info.checkInside(mx, my)) {
				iter->info.onClick(mx, my, wmsg);
				if (iter != pageNow) {
					pageNow->disable();
					iter->enable();
					pageNow = iter;
				}
				break;
			}
			if (iter == pageNow && iter->psub->checkInside(mx, my)) {
				iter->psub->onClick(mx, my, wmsg);
				break;
			}
			++iter;
		}
	}

	bool isDragArea(int mx, int my) {
		CursorToWindow(mx, my);
		V_TabWindow::iterator pcur = vsub.begin();
		while (pcur != vsub.end()) {
			if (pcur->info.checkInside(mx, my)) return TRUE;
			++pcur;
		}
		return FALSE;
	}
};

class ListWindow :public Window {
private:
	int startUnit; //清單視窗顯示的第一個 Unit 數
	enum { startSel = 0 };
	int unitH;	//Unit 視窗的高度

	enum {textMaxLen = 16};
	class Unit {
	public:
		char text[textMaxLen];
		bool bSel;
		Unit() : bSel(FALSE) {}
	};
	typedef std::vector<Unit> V_Unit;
	V_Unit UnitVolume;

private:
	class TextWindow2 : public TextWindow {
	public:
		bool bSel; //這個文字視窗是否被選取
	public:
		TextWindow2() : bSel(FALSE) {}
		void draw(HDC hdc, int ox, int oy) {
			if (bSel) {
				SetDCPenColor(hdc, RGB(0, 0, 255));
				SelectObject(hdc, GetStockObject(DC_PEN));
				TextWindow::draw(hdc, ox, oy);
				SelectObject(hdc, GetStockObject(BLACK_PEN));
			}
			else
				TextWindow::draw(hdc, ox, oy);
		}
		void onClick(int mx, int my, WindowMessage& wmsg) {
			if (!isVisible()) return;
			Window::onClick(mx, my, wmsg);
			if (checkInside(mx, my))
				bSel = !bSel;
		}
	};

	typedef std::vector<TextWindow2> V_TextWindow;
	V_TextWindow TextWindowVolume;

private:
	void updateStartSel() {
		if (startUnit + TextWindowVolume.size() >= UnitVolume.size())
			startUnit = UnitVolume.size() - TextWindowVolume.size();
		if (startUnit < startSel)
			startUnit = startSel;
	}
	void update() {
		updateStartSel();
		//根據 startUnit 刷新 lsit 存放視窗內的文字
		V_Unit::iterator UnitIter = UnitVolume.begin() + startUnit;
		V_TextWindow::iterator TextWindowIter = TextWindowVolume.begin();
		while (UnitIter != UnitVolume.end() &&
		TextWindowIter != TextWindowVolume.end()) {
			TextWindowIter->setText(UnitIter->text, TRUE);
			TextWindowIter->bSel = UnitIter->bSel;
			++UnitIter;
			++TextWindowIter;
		}
		Window::update();
	}
public:
	ListWindow() : startUnit(startSel) {}
	void init(int _x, int _y, int _w, int text_count, int text_h) {
		Window::init(_x, _y, _w, text_count * text_h);
		unitH = text_h;
		TextWindowVolume.reserve(text_count);
		//UnitVolume.reserve(text_count);
		for (int i = 0; i < text_count; i++) {
			TextWindow2 txtWndTmp;
			txtWndTmp.init(0, i*text_h, _w, text_h);
			txtWndTmp.clear();
			TextWindowVolume.push_back(txtWndTmp);
			addChhild(&TextWindowVolume.back());
		}

	}

	void onClick(int mx, int my, WindowMessage& wmsg) {
		if (!isVisible()) return;
		Window::onClick(mx, my, wmsg);
		CursorToWindow(mx, my);
		for (int i = 0; i < (int)TextWindowVolume.size(); i++) {
			UnitVolume[i+startUnit].bSel = TextWindowVolume[i].bSel;
		}
		update();
	}

	void addText(const char* txt) {
		Unit tmp;
		strncpy_s(tmp.text, txt, textMaxLen);
		UnitVolume.push_back(tmp);
		update();
	}

	void moveUp() {
		startUnit--;
		updateStartSel();
		update();
	}
	void moveDown() {
		startUnit++;
		updateStartSel();
		update();
	}
};

class ScrollBarWindow : public Window {
public:
	void setScale(int _beg, int _end, int _cur, int _Nunit) {
		btnMid.beg = _beg;
		btnMid.end = _end;
		btnMid.cur = _cur;
		btnMid.Nunit = _Nunit;
		update();
	}
private:
	class BtnMid : public Window {
		friend void ScrollBarWindow::setScale(int, int ,int, int);
	private:
		void updateH() {
			int parentH = getParentWindow()->getH();
			h = (Nunit * (parentH - 2 * w)) / (end - beg + 1);
		}
		void updateY() {
			int parentH = getParentWindow()->getH();
			if (cur == end - Nunit + 1)
				y = parentH - h - w;
			else {
				double unitH = (double)((parentH - 2.0 * w) / (end - beg + 1.0));
				y = (int)(w + (double)((cur - beg) * unitH));
			}
			
		}
	private:
		int Nunit;
		int beg, end, cur; //刻度
	public:
		BtnMid() : Nunit(0) { Window::Window(); }
		void update() {
			//根據 cur 刻度 換算 btnMid 的 位置
			if (cur < beg) cur = beg;
			if (cur > end) cur = end;
			updateH();
			updateY();
			setSize(w, h);
			setPos(0, y);

			Window::update();
		}
		
		void moveUp() {
			--cur;
			if (cur < beg) cur = beg;
		}
		void moveDown() {
			++cur;
			if (cur > end - Nunit + 1) cur = end - Nunit + 1;
		}
		/*
		//for debug
		void draw(HDC hdc, int ox, int oy) {
		Window::draw(hdc, ox, oy);
		char buf[48];
		sprintf_s(buf, "x = %d y = %d w = %d h = %d", x, y, w, h);
		TextOut(hdc, 0, 0, buf, strlen(buf));
		sprintf_s(buf, "beg = %d end = %d cur = %d Nunit = %d", beg, end, cur, Nunit);
		TextOut(hdc, 0, 20, buf, strlen(buf));
		}
		*/
	};
private:
	Window btnUp, btnDown;
	BtnMid btnMid;

public:
	void init(int _x, int _y, int _w, int _h, int _beg, int _end) {
		Window::init(_x, _y, _w, _h);
		assert(_h > 2 * _w);
		Window::addChhild(&btnUp);
		Window::addChhild(&btnDown);
		Window::addChhild(&btnMid);

		btnUp.init(0, 0, _w, _w);

		btnDown.init(0, _h - _w, _w, _w);

		setScale(_beg, _end, _beg, 1);
		btnMid.init(0, _w, _w, _w);
		btnMid.update();
	}

	void onClick(int mx, int my, WindowMessage& wmsg) {
		if (!isVisible()) return;
		Window::onClick(mx, my, wmsg);
		CursorToWindow(mx, my);
		if (btnUp.checkInside(mx, my))
			btnMid.moveUp();
		if (btnDown.checkInside(mx, my))
			btnMid.moveDown();
		if (btnMid.checkInside(mx, my)) {
			wmsg.startTracing(this);
		}
		btnMid.update();
	}
	
	void tracing(int mx, int my) {
		getWindowMousePos(mx, my);
		if (isBtnMidMoveUp(mx ,my))
			btnMid.moveUp();
		else if (isBtnMidMoveDown(mx, my))
			btnMid.moveDown();
		btnMid.update();
	}
	bool isBtnMidMoveUp(int mx, int my) {
		return btnMid.getY() > my;
	}
	bool isBtnMidMoveDown(int mx, int my) {
		return (btnMid.getY() + btnMid.getH() < my);
	}
	bool isOnClickbtnUp(int mx, int my) {
		return btnUp.checkInside(mx, my);
	}
	bool isOnClickbtnDown(int mx, int my) {
		return btnDown.checkInside(mx, my);
	}
	bool isOnClickbtnMid(int mx, int my) {
		return btnMid.checkInside(mx, my);
	}
	
};


//v1 一個空白視窗包含兩個子視窗
class ScrollListWindow : public Window {
private:
	ListWindow listWnd;
	ScrollBarWindow scrollBarWnd;
	int listCount;
	int memberCount;
public:

	void init(int _x, int _y, int _w, int _h,
		int _listCount, int barW) {
		memberCount = 0;
		listCount = _listCount;
		Window::init(_x, _y, _w, _h);
		listWnd.init(0, 0, _w - barW, _listCount, _h / listCount);
		scrollBarWnd.init(_w - barW, 0, barW, _h, 0, 0);
		Window::addChhild(&listWnd);
		Window::addChhild(&scrollBarWnd);
		update();
	}
	void addText(const char* txt) {
		listWnd.addText(txt);
		memberCount++;
		int curListUnit = (listCount > memberCount) ? memberCount : listCount;
		scrollBarWnd.setScale(0, memberCount-1, 0, curListUnit);
	}

	void onClick(int mx, int my, WindowMessage& wmsg) {
		Window::onClick(mx, my, wmsg);
		CursorToWindow(mx, my);
		//視窗座標轉ScrollBar座標
		mx = mx - w + scrollBarWnd.getW();
		if (scrollBarWnd.isOnClickbtnUp(mx, my)) {
			listWnd.moveUp();
		}
		else if (scrollBarWnd.isOnClickbtnDown(mx, my)) {
			listWnd.moveDown();
		}
		else if (scrollBarWnd.isOnClickbtnMid(mx, my)) {
			wmsg.startTracing(this);
		}
	}

	void tracing(int mx, int my) {
		//視窗座標轉ScrollBar座標
		int toScrollBarWndx = mx - w + scrollBarWnd.getW();
		int toScrollBarWndy = my;
		//因為使用 isBtnMidMoveUp/Down 必須用另一個座標系統
		//作法為先將 ScrollBarWnd的原點 平移到 ScrollListWindow的原點
		//然後將平移過的系統轉成絕對座標
		int tx = toScrollBarWndx;
		int ty = toScrollBarWndy;
		getWindowMousePos(tx, ty);
		if (scrollBarWnd.isBtnMidMoveUp(tx, ty)) {
			listWnd.moveUp();
		}
		else if (scrollBarWnd.isBtnMidMoveDown(tx, ty)) {
			listWnd.moveDown();
		}
		//直接用scrollBarWnd的Tracing 做移動
		//判斷list是否移動與ScrollBar的位置有關
		//所以先處理lust的移動在處理scrollBar的移動
		scrollBarWnd.tracing(toScrollBarWndx, toScrollBarWndy);
	}
};

/*
class ScrollListWindow : public ScrollBarWindow {
private:
	ListWindow listWnd;
public:
	void init(int _x, int _y, int _w, int _h,
		int _listCount, int barW) {
		ScrollBarWindow::init()
	}
	void addText(const char* txt) {
		listWnd.addText(txt);
	}
	void draw(HDC ddc, int ox, int oy) {

	}
};
*/
#endif

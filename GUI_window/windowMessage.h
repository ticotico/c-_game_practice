#ifndef WINDOW_MESSAGE_H
#define WINDOW_MESSAGE_H
enum wmsgBit {
	WMSG_IDLE = 0,
	WMSG_DRAG = 1 << 1,
	WMSG_TRACING = 1 << 2
};
class Window;
class WindowMessage {
private:
	int msg;
	Window* pWorkingWnd;
public:
	WindowMessage() : msg(0), pWorkingWnd(NULL) {}
	void clear() {
		msg = 0;
		pWorkingWnd = NULL;
	}
	bool isDrag() {
		return (msg & WMSG_DRAG) == 0;
	}
	void setDrag(bool b) {
		if (b)
			msg |= WMSG_DRAG;
		else
			msg &= ~WMSG_DRAG;
	}
	bool isTracing() {
		return (msg & WMSG_TRACING) == 0;
	}
	void setTracing(bool b) {
		if (b)
			msg |= WMSG_TRACING;
		else
			msg &= ~WMSG_TRACING;
	}
	int getmsg() {
		return msg;
	}
	void setWorkingWindow(Window* pw) {
		pWorkingWnd = pw;
	}
	Window* getWorkingWindow() {
		return pWorkingWnd;
	}
	void startTracing(Window* target) {
		clear();
		setTracing(TRUE);
		setWorkingWindow(target);
	}
};

#endif

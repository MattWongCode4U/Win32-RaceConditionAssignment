//Matthew Wong
//A00925594
//Asn5
#include <Windows.h>

LRESULT CALLBACK HelloWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK childWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

WNDPROC SubProc;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCMLine, int iCmdShow) {
	static TCHAR szAppName[] = TEXT("HelloApplication");
	static HWND	hwnd;
	MSG		msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = HelloWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = NULL;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	if (!RegisterClass(&wndclass)) {
		MessageBox(NULL, TEXT("This program requires Windows 95/98/NT"),
			szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName,
		TEXT("PART TWO"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		300,
		200,
		NULL,
		NULL,
		hInstance,
		NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK HelloWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static HWND hwndButton;
	static int cxChar, cyChar;
	HDC hdc;
	PAINTSTRUCT ps;
	HPEN pen = CreatePen(PS_SOLID, 1, (RGB(0, 0, 0)));
	RECT rect;
	POINT pt, pt2;
	POINT point, mousePos;
	BOOL movingL = FALSE;
	BOOL movingR = FALSE;
	HANDLE ev = CreateEvent(NULL, FALSE, FALSE, TEXT("EventTwo"));
	HANDLE quitEv = CreateEvent(NULL, FALSE, FALSE, TEXT("EventQuit"));
	DWORD dwEvent;

	switch (message) {
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());
		//create button
		hwndButton = CreateWindow(TEXT("button"),
			TEXT("PRESS ME"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			cxChar, cyChar,
			20 * cxChar, 7 * cyChar / 4,
			hwnd, NULL,
			((LPCREATESTRUCT)lParam)->hInstance, NULL);

		//save wndprocs of radiobuttons into array
		SubProc = (WNDPROC)
			SetWindowLong(hwndButton, GWL_WNDPROC, (LONG)childWndProc);
		return 0;

	case WM_PAINT:
		
		return 0;

	case WM_COMMAND:
		switch (HIWORD(wParam)) {
			case BN_CLICKED:
				//SetWindowText(hwnd, TEXT("Button Click"));
				SetEvent(ev);
				dwEvent = WaitForSingleObject(quitEv, INFINITE);
				if (dwEvent == WAIT_OBJECT_0) {
					PostQuitMessage(0);
				}
				return 0;
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK childWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	return CallWindowProc(SubProc, hwnd, message, wParam, lParam);
}
//Matthew Wong
//A00925594
//Asn5
#include <Windows.h>;
LRESULT CALLBACK HelloWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK childWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI MyThreadFunction(LPVOID lpParam);
void winner(HWND hwnd);

WNDPROC SubProc[5];
TCHAR* button[] = { TEXT("Red Car"), TEXT("Blue Car"), TEXT("Green Car"), TEXT("Orange Car"), TEXT("Black Car") };
BOOLEAN raceWon = FALSE;
BOOLEAN synch = FALSE;
CRITICAL_SECTION myCS;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCMLine, int iCmdShow) {
	static TCHAR szAppName[] = TEXT("HelloApplication");
	static HWND	hwnd;
	MSG		msg;
	WNDCLASS wndclass;
	DWORD dwEvent;
	HANDLE hEvents[2];
	InitializeCriticalSection(&myCS);

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = HelloWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	if (!RegisterClass(&wndclass)) {
		MessageBox(NULL, TEXT("This program requires Windows 95/98/NT"),
			szAppName, MB_ICONERROR);
		return 0;
	}

	hEvents[0] = CreateEvent(NULL, FALSE, FALSE, TEXT("EventTwo"));
	hEvents[1] = CreateEvent(NULL, FALSE, FALSE, TEXT("EventThree"));
	dwEvent = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);

	if (dwEvent == WAIT_OBJECT_0) {
		hwnd = CreateWindow(szAppName,
			TEXT("PROGRAM TWO"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			800,
			600,
			NULL,
			NULL,
			hInstance,
			NULL);
		ShowWindow(hwnd, iCmdShow);
		UpdateWindow(hwnd);
	} else if(dwEvent == (WAIT_OBJECT_0 + 1)){
		hwnd = CreateWindow(szAppName,
			TEXT("PROGRAM THREE"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			800,
			600,
			NULL,
			NULL,
			hInstance,
			NULL);
		ShowWindow(hwnd, iCmdShow);
		UpdateWindow(hwnd);
	}

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK HelloWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	static TCHAR* button[] = { TEXT("Red Car"), TEXT("Blue Car"), TEXT("Green Car"), TEXT("Orange Car"), TEXT("Black Car") };
	static HWND hwndButton[5];
	static int cxChar, cyChar;
	HDC hdc;
	PAINTSTRUCT ps;
	HPEN pen = CreatePen(PS_SOLID, 1, (RGB(0, 0, 0)));
	RECT rect;
	POINT pt, pt2;
	HANDLE quitEv = CreateEvent(NULL, FALSE, FALSE, TEXT("EventQuit"));
	HANDLE hThreadArray[5];
	DWORD dwThreadIdArray[5];

	switch (message) {
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		//create push buttons
		for (int i = 0; i < 5; i++) {
			hwndButton[i] = CreateWindow(TEXT("button"),
				button[i],
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				cxChar, cyChar * (1 + 5 * i) + 20,
				20 * cxChar, 7 * cyChar / 4,
				hwnd, NULL,
				((LPCREATESTRUCT)lParam)->hInstance, NULL);

			//save wndprocs of radiobuttons into array
			SubProc[i] = (WNDPROC)
				SetWindowLong(hwndButton[i], GWL_WNDPROC, (LONG)childWndProc);
		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SelectObject(hdc, pen);

		GetWindowRect(hwnd, &rect);
		
		pt.x = rect.left;
		pt.y = rect.right;
		pt2.y = rect.bottom;

		ScreenToClient(hwnd, (LPPOINT)&pt);
		ScreenToClient(hwnd, (LPPOINT)&pt2);

		MoveToEx(hdc, (pt.y - pt.x) / 2, 0, NULL);
		LineTo(hdc, (pt.y - pt.x) / 2, pt2.y);
		return 0;

	case WM_LBUTTONUP:
		synch = FALSE;
		for (int i = 0; i < 5; i++) {
			hThreadArray[i] = CreateThread(NULL, 0, MyThreadFunction, hwndButton[i], 0, &dwThreadIdArray[i]);
		}
		return 0;

	case WM_RBUTTONUP:
		synch = TRUE;
		for (int i = 0; i < 5; i++) {
			hThreadArray[i] = CreateThread(NULL, 0, MyThreadFunction, hwndButton[i], 0, &dwThreadIdArray[i]);
		}
		return 0;

	case WM_DESTROY:
		SetEvent(quitEv);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK childWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	
	return CallWindowProc(SubProc[0], hwnd, message, wParam, lParam);
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam) {

	winner((HWND)lpParam);

	return 0;
}

void winner(HWND hwnd) {
	POINT pt, pt2;
	RECT rc, rc2;
	HWND hwndParent = GetParent(hwnd);
	int inc = 10;
	TCHAR text[1024];
	
	GetWindowText(hwnd, text, 1024);

	while (!raceWon) {
		GetWindowRect(hwnd, &rc);
		pt.x = rc.left;
		pt.y = rc.top;

		ScreenToClient(hwndParent, (LPPOINT)&pt);

		SetWindowPos(hwnd, HWND_TOP, pt.x + inc, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

		GetWindowRect(hwndParent, &rc2);
		pt2.x = rc2.left;
		pt2.y = rc2.right;

		ScreenToClient(hwndParent, (LPPOINT)&pt2);
		if (synch) {
			EnterCriticalSection(&myCS);
		}
		if (pt.x > (pt2.y - pt2.x) / 2) {
			//with race condition code
			if (!raceWon) {
				MessageBox(hwnd, text, TEXT("WINNER"), MB_OK);
				raceWon = TRUE;
			}
		}
		if (synch) {
			LeaveCriticalSection(&myCS);
		}
	}
}
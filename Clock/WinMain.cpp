#include <windows.h>
#include <cmath>
#include "resource1.h"
#include <tchar.h>

int gcnt;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define PRT_CLASS "ClockApp"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrecInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wc;
	HWND hWnd;
	MSG msg;

#pragma region Window class Setup
	wc.cbSize = sizeof WNDCLASSEX;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = TEXT(PRT_CLASS);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hbrBackground = (HBRUSH)(COLOR_GRAYTEXT);
	wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ARROW));
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
#pragma endregion

	RegisterClassEx(&wc);


#pragma region Window handle Setup
	hWnd = CreateWindow(TEXT(PRT_CLASS),
		TEXT("Clock"),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		460, 480,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hWnd == NULL) return FALSE;
#pragma endregion

	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

struct Points{ int x, y; };

void CalculatePoints(Points points[60], int radius)
{
	const auto pi = acos(-1.0);

	for (int i = 0; i < 60; i++)
	{
		double a = (2.0 * pi * i / 60) - (pi/2);
		points[i].x = 220 + radius * cos(a);
		points[i].y = 220 + radius * sin(a);
	}
}
void CalculateHours(Points points[12], int radius)
{
	const auto pi = acos(-1.0);

	for (int i = 0; i < 12; i++)
	{
		double a = (2.0 * pi * i / 12) - (pi / 2);
		points[i].x = 220 + radius * cos(a);
		points[i].y = 220 + radius * sin(a);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int iRet;
	PAINTSTRUCT ps;
	HDC hdc;
	HPEN hPen, hMinpen, hHourpen;
	//HBRUSH hBrush;
	RECT rc;
	GetClientRect(hWnd, &rc);
	Points points[60], minutes[60], hours[12];
	TCHAR s[5],m[5],h[5];
	static int scnd, min, hour;
	SYSTEMTIME time;
	GetLocalTime(&time);
	POINT pt;

	HINSTANCE hInst;

	static TCHAR stringRC1[80];
	static TCHAR stringRC2[80];
	static TCHAR stringRC3[80];

	
	switch (message)
	{

	case WM_CREATE:	

		scnd = time.wSecond;
		min = time.wMinute;
		hour = time.wHour % 12;
		//hInst = (HINSTANCE)GetWindowInfo(hWnd, GWLP_HINSTANCE);

		SetTimer(hWnd, 1, 1000, NULL);
		

		break;

	case WM_TIMER:
		scnd++;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	


	
	case WM_PAINT:

		hdc = BeginPaint(hWnd, &ps);
		
		Ellipse(hdc, 10, 10, 430, 430);

		hPen = CreatePen(PS_SOLID, 2, RGB(232, 82, 82));
		hMinpen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		hHourpen = CreatePen(PS_SOLID, 4, RGB(100, 100, 100));
		SelectObject(hdc, hPen);

		CalculatePoints(points, 200);
		CalculatePoints(minutes, 150);
		CalculateHours(hours, 100);

		if (scnd > 59)
		{
			scnd = 0;
			min++;
		}

		if (min > 59)
		{
			min = 0;
			hour++;
		}

		if (hour > 11) hour = 0;



		MoveToEx(hdc, 220, 220, NULL);
		LineTo(hdc, points[scnd].x, points[scnd].y);

		SelectObject(hdc, hMinpen);

		MoveToEx(hdc, 220, 220, NULL);
		LineTo(hdc, minutes[min].x, minutes[min].y);
		
		SelectObject(hdc, hHourpen);

		MoveToEx(hdc, 220, 220, NULL);
		LineTo(hdc, hours[hour].x, hours[hour].y);
		
		for (int i = 0; i < 60; i++)
		{
			//SetPixel(hdc, points[i].x, points[i].y, RGB(0, 0, 0));
			Ellipse(hdc, points[i].x - 1, points[i].y - 1, points[i].x + 1, points[i].y + 1);
		}

		CalculateHours(hours, 200);
		for (int i = 0; i < 12; i++)
		{
			//SetPixel(hdc, points[i].x, points[i].y, RGB(0, 0, 0));
			Ellipse(hdc, hours[i].x - 4, hours[i].y - 3, hours[i].x + 3, hours[i].y + 3);
		}

		CalculateHours(hours, 180);
		for (int i = 0; i < 12; i++)
		{
			if(i == 0) wsprintf(h, TEXT("%d"), 12);
			else wsprintf(h, TEXT("%d"), i);

			TextOut(hdc, hours[i].x - 10, hours[i].y - 10, h, lstrlen(h));
			
		}



		DeleteObject(hPen);
		DeleteObject(hMinpen);
		DeleteObject(hHourpen);
		
		EndPaint(hWnd, &ps);
		
		break;
		
	case WM_CLOSE:
		iRet = MessageBox(hWnd, TEXT("Do you want to Exit"), TEXT("Exit"), MB_YESNO);
		if (iRet == IDYES) SendMessage(hWnd, WM_DESTROY, 0, 0);
		break;
		break;
	case WM_DESTROY:

		PostQuitMessage(0);

		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

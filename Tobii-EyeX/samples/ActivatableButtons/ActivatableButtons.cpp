/*
 * ActivatableButtons sample:
 * This is an example that demonstrates the Activatable behavior.
 * It features two buttons that can be clicked by looking at the button and pressing the space bar.
 *
 * Copyright 2013 Tobii Technology AB. All rights reserved.
 */

#include "stdafx.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "resource.h"
#include "EyeXHost.h"

#pragma comment (lib, "Gdiplus.lib")

// window messages used for notifications from the EyeXHost.
#define WM_EYEX_HOST_STATUS_CHANGED     WM_USER + 0
#define WM_REGION_GOT_ACTIVATION_FOCUS  WM_USER + 1
#define WM_REGION_ACTIVATED             WM_USER + 2

// constants.
static const Gdiplus::Color OriginalBackgroundColor(66, 173, 209);
static const Gdiplus::Color OtherBackgroundColor(89, 169, 108);
static const int ChangeButtonLeft = 50;
static const int ResetButtonLeft = 260;
static const int ButtonTop = 100;
static const int ButtonWidth = 160;
static const int ButtonHeight = 100;
static const TCHAR* g_szWindowClass = _T("ActivatableButtons");

// global variables.
static EyeXHost g_eyeXHost;
static HWND g_hWnd;
static HWND g_hChangeButton;
static HWND g_hResetButton;
static bool g_useOriginalColor = true;

// gets the bounds of a button in screen coordinates.
RECT GetScreenBounds(HWND hButton)
{
	POINT point = { 0, 0 };
	ClientToScreen(hButton, &point);

	RECT bounds;
	bounds.left = point.x;
	bounds.top = point.y; 
	bounds.right = bounds.left + ButtonWidth;
	bounds.bottom = bounds.top + ButtonHeight;

	return bounds;
}

// reports the buttons as activatable regions to the EyeX host.
void UpdateActivatableRegions()
{
	std::vector<EyeXHost::ActivatableRegion> regions;
   
	regions.push_back(EyeXHost::ActivatableRegion(IDC_CHANGE_COLOR_BUTTON, GetScreenBounds(g_hChangeButton)));
	regions.push_back(EyeXHost::ActivatableRegion(IDC_RESET_COLOR_BUTTON, GetScreenBounds(g_hResetButton)));

	g_eyeXHost.SetActivatableRegions(regions);
}

// event handler invoked when a "status changed" event has been received from the EyeX host.
void OnStatusChanged(bool engineConnectionIsFunctional)
{
	// update the window title to reflect the engine connection state.
	if (engineConnectionIsFunctional)
	{
		SetWindowText(g_hWnd, _T("ActivatableButtons - Use Ur Eyez! :)"));
	}
	else
	{
	    SetWindowText(g_hWnd, _T("ActivatableButtons - Mouse Only :("));
	}
}

// event handler invoked when a region has received the activation focus.
void OnRegionGotActivationFocus(UINT regionId)
{
	// set the keyboard focus to the corresponding button.
	switch(regionId)
	{
	case IDC_CHANGE_COLOR_BUTTON:
		SetFocus(g_hChangeButton);
		break;

	case IDC_RESET_COLOR_BUTTON:
		SetFocus(g_hResetButton);
		break;
	}
}

// event handler invoked when the "change color" button has been activated.
void OnChangeColorActivated()
{
	g_useOriginalColor = false;
	InvalidateRect(g_hWnd, 0, FALSE);
}

// event handler invoked when the "reset color" button has been activated.
void OnResetColorActivated()
{
	g_useOriginalColor = true;
	InvalidateRect(g_hWnd, 0, FALSE);
}

// event handler invoked when a region has received an activation event.
void OnRegionActivated(UINT regionId)
{
	// route the event to the corresponding button.
	switch(regionId)
	{
	case IDC_CHANGE_COLOR_BUTTON:
		OnChangeColorActivated();
		break;

	case IDC_RESET_COLOR_BUTTON:
		OnResetColorActivated();
		break;
	}
}

void OnDraw(HDC hdc)
{
	Gdiplus::Graphics graphics(hdc);

	RECT rect;
	GetClientRect(g_hWnd, &rect);

	Gdiplus::Color backgroundColor(OriginalBackgroundColor);
	if (!g_useOriginalColor)
	{
		backgroundColor = OtherBackgroundColor;
	}

	Gdiplus::SolidBrush backgroundBrush(backgroundColor);
	graphics.FillRectangle(&backgroundBrush, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	const TCHAR* message = _T("Click the buttons with your eyes! Look at a button and press the space bar to click. ")
		_T("(Make sure the window has input focus.)");
	Gdiplus::Font font(_T("Arial"), 10);
	Gdiplus::SolidBrush textBrush(Gdiplus::Color::Black);
	Gdiplus::RectF textRect((Gdiplus::REAL)10, 
							(Gdiplus::REAL)(ButtonTop + ButtonHeight + 20), 
							(Gdiplus::REAL)(rect.right - 20), 
							(Gdiplus::REAL)(rect.bottom - rect.top));
	Gdiplus::StringFormat textFormat;
	graphics.DrawString(message, (INT)_tcslen(message), &font, textRect, &textFormat, &textBrush);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDC_CHANGE_COLOR_BUTTON:
			OnChangeColorActivated();
			break;

		case IDC_RESET_COLOR_BUTTON:
			OnResetColorActivated();
			break;

		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_CREATE:
		{
			g_hChangeButton = CreateWindow(
				_T("BUTTON"), 
				_T("Change color"),
				WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON, 
				ChangeButtonLeft, ButtonTop, ButtonWidth, ButtonHeight, 
				hWnd, 
				(HMENU)IDC_CHANGE_COLOR_BUTTON, 
				GetModuleHandle(NULL), 
				NULL); 
			g_hResetButton = CreateWindow(
				_T("BUTTON"), 
				_T("Reset color"), 
				WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON, 
				ResetButtonLeft, ButtonTop, ButtonWidth, ButtonHeight, 
				hWnd, 
				(HMENU)IDC_RESET_COLOR_BUTTON, 
				GetModuleHandle(NULL), 
				NULL); 
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnDraw(hdc);
		EndPaint(hWnd, &ps);
		break;

	case WM_ERASEBKGND:
		// no background erasing needed since our OnDraw method draws the entire window.
		return TRUE;

	case WM_EYEX_HOST_STATUS_CHANGED:
		OnStatusChanged(wParam != FALSE);
		break;

	case WM_REGION_GOT_ACTIVATION_FOCUS:
		OnRegionGotActivationFocus((UINT)wParam);
		break;

	case WM_REGION_ACTIVATED:
		OnRegionActivated((UINT)wParam);
		break;

	case WM_WINDOWPOSCHANGED:
		UpdateActivatableRegions();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		// trigger an activation command when space is pressed.
		if (VK_SPACE == wParam)
		{
			g_eyeXHost.TriggerActivation();
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ACTIVATABLEBUTTONS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= 0;
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_ACTIVATABLEBUTTONS);
	wcex.lpszClassName	= g_szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   g_hWnd = CreateWindow(g_szWindowClass, _T(""), WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 490, 380, NULL, NULL, hInstance, NULL);

   if (!g_hWnd)
   {
      return FALSE;
   }

   ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);

	// initialize the EyeX host and the activatable regions.
   g_eyeXHost.Init(g_hWnd, WM_EYEX_HOST_STATUS_CHANGED, WM_REGION_GOT_ACTIVATION_FOCUS, WM_REGION_ACTIVATED);
   UpdateActivatableRegions();

   return TRUE;
}

// Application entry point.
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize gdiplus
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ACTIVATABLEBUTTONS));

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Shutdown:
	Gdiplus::GdiplusShutdown(gdiplusToken);

	return (int) msg.wParam;
}

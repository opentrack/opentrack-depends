/*
 * ActivatableBoardGame sample:
 * This is an example that demonstrates the use of Activatable interactors in the context of a board game.
 *
 * Copyright 2013 Tobii Technology AB. All rights reserved.
 */

#include "stdafx.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "Resource.h"
#include "Board.h"
#include "BoardWindow.h"

#pragma comment (lib, "Gdiplus.lib")

// Application entry point.
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                       _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine,
                       _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// initialize the GDI+ library.
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// initialize the Board and the BoardWindow.
	// these will be un-initialized automatically when they fall out of scope.
	Board board(8);
	BoardWindow::RegisterWindowClass(hInstance);
	BoardWindow window(board, hInstance, nCmdShow);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GOBANG));

	// Main message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);

	return (int) msg.wParam;
}

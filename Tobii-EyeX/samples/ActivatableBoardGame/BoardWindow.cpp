/*
 * ActivatableBoardGame sample:
 * This is an example that demonstrates the use of Activatable interactors in the context of a board game.
 *
 * Copyright 2013 Tobii Technology AB. All rights reserved.
 */

#include "stdafx.h"
#include "BoardWindow.h"
#include "Board.h"
#include "resource.h"

// window messages used for notifications from the EyeXHost.
#define WM_EYEX_HOST_STATUS_CHANGED   WM_USER + 0
#define WM_FOCUSED_REGION_CHANGED     WM_USER + 1
#define WM_REGION_ACTIVATED           WM_USER + 2

// color scheme.
const Gdiplus::Color BoardWindow::BackgroundGazeInteractionFunctionalColor = Gdiplus::Color(134, 152, 172);      // blue
const Gdiplus::Color BoardWindow::BackgroundMouseOnlyColor                 = Gdiplus::Color(164, 156, 153);      // grey
const Gdiplus::Color BoardWindow::GridColor                                = Gdiplus::Color(110, 57, 88, 109);   // dark blue
const Gdiplus::Color BoardWindow::MarkerColor                              = Gdiplus::Color(242, 233, 216);      // off white
const Gdiplus::Color BoardWindow::HighlightColor                           = Gdiplus::Color(20, 242, 233, 216);  // transparent off white
const Gdiplus::Color BoardWindow::WonMessageColor                          = Gdiplus::Color(200, 64, 82, 60);    // transparent dark green
const Gdiplus::Color BoardWindow::LostMessageColor                         = Gdiplus::Color(200, 155, 109, 135); // transparent grape

// constants.
const int BoardWindow::SquareSize = 100;
const int BoardWindow::MarkerMargin = 14;
const int BoardWindow::MarkerDiameter = SquareSize - MarkerMargin * 2;
const TCHAR* BoardWindow::WindowClassName = _T("GobangBoard");

BoardWindow::BoardWindow(Board& board, HINSTANCE hInstance, int nCmdShow)
	: _board(board), _hInstance(hInstance), _hWnd(0), _hMenu(0), _leftMargin(0), _upperMargin(0)
{
	// create the window instance.
	_hWnd = CreateWindow(
		WindowClassName, 
		_T("Gobang!"), 
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		(_board.Size() + 1) * SquareSize, 
		(_board.Size() + 1) * SquareSize + 40,
		NULL, 
		NULL, 
		hInstance, 
		NULL);

	if (!_hWnd)
	{
		throw new std::runtime_error("Could not create main window.");
	}

	_hMenu = GetMenu(_hWnd);

	// write the "this" pointer to the extra window memory so that we can reference back to this 
	// object from the window function.
	SetWindowLongPtr(_hWnd, 0, (LONG_PTR)this);

	ShowWindow(_hWnd, nCmdShow);
	UpdateWindow(_hWnd);

	_board.RegisterBoardChangedObserver(this);
	
	// initialize the EyeX host and the activatable regions.
	_eyeXHost.Init(_hWnd, WM_EYEX_HOST_STATUS_CHANGED, WM_FOCUSED_REGION_CHANGED, WM_REGION_ACTIVATED);
	UpdateActivatableRegions();
}

BoardWindow::~BoardWindow()
{
}

void BoardWindow::RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(void*); // make room for the "this" pointer.
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GOBANG));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= 0;
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GOBANG);
	wcex.lpszClassName	= WindowClassName;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassEx(&wcex);
}

// Callback function invoked when the Board has changed.
void BoardWindow::SubjectChanged()
{
	InvalidateRect(_hWnd, NULL, FALSE);
	UpdateActivatableRegions();
}

// Callback function invoked from the message loop to process a Windows message.
LRESULT CALLBACK BoardWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	// read the "this" pointer from extra window memory.
	BoardWindow* instance = reinterpret_cast<BoardWindow*>(GetWindowLongPtr(hWnd, 0));

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_NEWGAME:
			instance->OnNewGameClicked();
			break;

		case IDM_ABOUT:
			DialogBox(instance->_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;

		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_LBUTTONDOWN:
		{
			// mouse button pressed; possibly on a board square.
			auto point = MAKEPOINTS(lParam);
			instance->OnSquareClicked(point);
		}
		break;

	case WM_PAINT:
		instance->UpdateMargins();
		hdc = BeginPaint(hWnd, &ps);
		instance->OnDraw(hdc);
		EndPaint(hWnd, &ps);
		break;

	case WM_ERASEBKGND:
		// no background erasing needed since our OnDraw method draws the entire window.
		return TRUE;

	case WM_EYEX_HOST_STATUS_CHANGED:
		// the background color of the board indicates whether we have a working connection to the engine or not.
		// so, when the status changes, we force a redraw of the window.
		InvalidateRect(hWnd, 0, FALSE);
		break;

	case WM_FOCUSED_REGION_CHANGED:
		// redraw to display the new focus state.
		InvalidateRect(hWnd, 0, FALSE);
		break;

	case WM_REGION_ACTIVATED:
		instance->OnSquareActivated((UINT)wParam);
		break;

	case WM_WINDOWPOSCHANGED:
		// the window was moved, so we need to refresh the screen bounds of our activatable regions.
		instance->UpdateActivatableRegions();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
    case WM_KEYUP:
		// trigger an activation command when space is released.
		if (VK_SPACE == wParam)
		{
			instance->_eyeXHost.TriggerActivation();
		}
        break;

    case WM_KEYDOWN:
        // trigger activation mode on when space is pressed down (so activation focus changed events are received, and a highlight can be drawn).
		if (VK_SPACE == wParam)
		{
            instance->_eyeXHost.TriggerActivationModeOn();
		}
		break;


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void BoardWindow::OnDraw(HDC hdc)
{
	// use double-buffer drawing to speed up the drawing and reduce flicker.
	// (the drawing is performed on a bitmap, which is then blitted to the screen in one fell swoop.)

	RECT rect;
	GetClientRect(_hWnd, &rect);
	Gdiplus::Bitmap bitmap(rect.right - rect.left, rect.bottom - rect.top);
	Gdiplus::Graphics graphics(&bitmap);

	// draw the background in a color which depends on the state of the connection to the engine.
	// blue if the connection is functional, gray otherwise.
	Gdiplus::Color backgroundColor(BackgroundGazeInteractionFunctionalColor);
	if (!_eyeXHost.IsFunctional())
	{
		backgroundColor = BackgroundMouseOnlyColor;
	}
	Gdiplus::SolidBrush backgroundBrush(backgroundColor);
	graphics.FillRectangle(&backgroundBrush, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	// draw the grid
	DrawGrid(&graphics);

	// draw the markers
	Gdiplus::Pen markerPen(MarkerColor, 5.0F);
	for (int row = 0; row < _board.Size(); row++)
	{
		for (int col = 0; col < _board.Size(); col++)
		{
			switch (_board.GetMarkerAt(Board::Position(row, col)))
			{
			case Board::Marker::X:
				DrawXMarker(&graphics, &markerPen, row, col);
				break;

			case Board::Marker::O:
				DrawOMarker(&graphics, &markerPen, row, col);
				break;
			}
		}
	}

	// draw the focus highlight
	int focusedRegionId = _eyeXHost.GetFocusedRegionId();
	if (focusedRegionId >= 0)
	{
		DrawHighlight(&graphics, RegionIdToBoardRow(focusedRegionId), RegionIdToBoardColumn(focusedRegionId));
	}

	// draw the game over message
	if (_board.IsGameOver())
	{
		WCHAR* message;
		Gdiplus::Color messageColor(0,0,0);
		switch (_board.GetWinner())
		{
		case Board::Marker::X:
			message = L"YOU WON :)";
			messageColor = WonMessageColor;
			break;

		case Board::Marker::O:
			message = L"YOU LOST :(";
			messageColor = LostMessageColor;
			break;

		default:
			message = L"DRAW";
			messageColor = LostMessageColor;
			break;
		}

		DrawGameOver(&graphics, message, messageColor);
	}

	Gdiplus::Graphics screen(hdc);
	screen.DrawImage(&bitmap, 0, 0);
}

void BoardWindow::DrawGrid(Gdiplus::Graphics *graphics)
{
	const int lineMargin = 14;

	Gdiplus::Pen gridPen(GridColor);
	gridPen.SetDashStyle(Gdiplus::DashStyleDash);

	// horizontal lines
	auto x0 = _leftMargin - lineMargin;
	auto x1 = _leftMargin + _board.Size() * SquareSize + lineMargin;
	for (int i = 0; i <= _board.Size(); i++)
	{
		auto y = _upperMargin + i * SquareSize;
		graphics->DrawLine(&gridPen, x0, y, x1, y);
	}

	// vertical lines
	auto y0 = _upperMargin - lineMargin;
	auto y1 = _upperMargin + _board.Size() * SquareSize + lineMargin;
	for (int j = 0; j <= _board.Size(); j++)
	{
		auto x = _leftMargin + j * SquareSize;
		graphics->DrawLine(&gridPen, x, y0, x, y1);
	}

	// dots that guide the eye to the center of the square
	Gdiplus::SolidBrush gridBrush(GridColor);
	for (int i = 0; i <= _board.Size(); i++)
	{
		for (int j = 0; j <= _board.Size(); j++)
		{
			auto x = _leftMargin + (j + 0.5) * SquareSize;
			auto y = _upperMargin + (i + 0.5) * SquareSize;
			graphics->FillEllipse(&gridBrush, (Gdiplus::REAL)(x - 1), (Gdiplus::REAL)(y - 1), (Gdiplus::REAL)3, (Gdiplus::REAL)3);
		}
	}
}

void BoardWindow::DrawHighlight(Gdiplus::Graphics *graphics, INT row, INT column)
{
	Gdiplus::SolidBrush highlightBrush(HighlightColor);

	int x = BoardToClientAreaX(column) + 1;
	int y = BoardToClientAreaY(row) + 1;

	graphics->FillRectangle(&highlightBrush, x, y, SquareSize - 1, SquareSize - 1);
}

void BoardWindow::DrawOMarker(Gdiplus::Graphics *graphics, Gdiplus::Pen *markerPen, INT row, INT column)
{
	int x = BoardToClientAreaX(column) + MarkerMargin;
	int y = BoardToClientAreaY(row) + MarkerMargin;

	graphics->DrawEllipse(markerPen, x, y, MarkerDiameter, MarkerDiameter);
}

void BoardWindow::DrawXMarker(Gdiplus::Graphics *graphics, Gdiplus::Pen *markerPen, INT row, INT column)
{
	int x = BoardToClientAreaX(column) + MarkerMargin;
	int y = BoardToClientAreaY(row) + MarkerMargin;

	graphics->DrawLine(markerPen, x, y + MarkerDiameter, x + MarkerDiameter, y);
	graphics->DrawLine(markerPen, x, y, x + MarkerDiameter, y + MarkerDiameter);
}

void BoardWindow::DrawGameOver(Gdiplus::Graphics *graphics, WCHAR* message, const Gdiplus::Color& messageColor)
{
   Gdiplus::Font myFont(L"Arial", 72);

   RECT rect;
   GetClientRect(_hWnd, &rect);
   Gdiplus::RectF layoutRect((float)rect.left, (float)rect.top, (float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
   
   Gdiplus::StringFormat format;
   format.SetAlignment(Gdiplus::StringAlignmentCenter);
   format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

   Gdiplus::SolidBrush messageBrush(messageColor);

   graphics->DrawString(
	   message,
	   (INT)wcslen(message),
	   &myFont,
	   layoutRect,
	   &format,
	   &messageBrush);
}

INT BoardWindow::BoardToClientAreaX(INT column)
{
	return column * SquareSize + _leftMargin;
}

INT BoardWindow::BoardToClientAreaY(INT row)
{
	return row * SquareSize + _upperMargin;
}

INT BoardWindow::ClientAreaToBoardColumn(INT x)
{
	return (x - _upperMargin) / SquareSize;
}

INT BoardWindow::ClientAreaToBoardRow(INT y)
{
	return (y - _leftMargin) / SquareSize;
}

int BoardWindow::BoardPositionToRegionId(int row, int column)
{
	return column + row * _board.Size();
}

int BoardWindow::RegionIdToBoardRow(int regionId)
{
	return regionId / _board.Size();
}

int BoardWindow::RegionIdToBoardColumn(int regionId)
{
	return regionId % _board.Size();
}

void BoardWindow::UpdateMargins()
{
	RECT rect;
	GetClientRect(_hWnd, &rect);
	_upperMargin = ((rect.bottom - rect.top) - _board.Size() * SquareSize) / 2;
	_leftMargin = ((rect.right - rect.left) - _board.Size() * SquareSize) / 2;
}

void BoardWindow::OnSquareClicked(POINTS point)
{
	MakeHumanPlayerMove(ClientAreaToBoardRow(point.y), ClientAreaToBoardColumn(point.x));
}

void BoardWindow::OnSquareActivated(UINT regionId)
{
	MakeHumanPlayerMove(RegionIdToBoardRow(regionId), RegionIdToBoardColumn(regionId));
}

void BoardWindow::MakeHumanPlayerMove(int row, int column)
{
	Board::Position position(row, column);
	if (_board.CanMakeMoveAt(position))
	{
		_board.MakeHumanPlayerMove(position);
	}
}

void BoardWindow::UpdateActivatableRegions()
{
	std::vector<EyeXHost::ActivatableRegion> regions;

	if (!_board.IsGameOver())
	{
		for (int row = 0; row < _board.Size(); row++)
		{
			for (int column = 0; column < _board.Size(); column++)
			{
				POINT point;
				point.x = BoardToClientAreaX(column);
				point.y = BoardToClientAreaY(row);
				ClientToScreen(_hWnd, &point);

				RECT bounds;
				bounds.left = point.x;
				bounds.top = point.y;
				bounds.right = point.x + SquareSize;
				bounds.bottom = point.y + SquareSize;

				regions.push_back(EyeXHost::ActivatableRegion(BoardPositionToRegionId(row, column), bounds));
			}
		}
	}

	_eyeXHost.SetActivatableRegions(regions);
}

void BoardWindow::OnNewGameClicked()
{
	_board.BeginNewGame();
	UpdateActivatableRegions();
}

// Message handler for about box.
INT_PTR CALLBACK BoardWindow::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}

	return (INT_PTR)FALSE;
}

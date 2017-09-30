/*
 * BoardWindow class: Responsible for the game window and the GDI+ drawing.
 * Listens to notifications from the Board.
 * Owns the EyeX host.
 *
 * Copyright 2013 Tobii Technology AB. All rights reserved.
 */

#pragma once

#include <Windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "Observer.h"
#include "EyeXHost.h"

class Board;

class BoardWindow : Observer
{
public:
	BoardWindow(Board& board, HINSTANCE hInstance, int nCmdShow);
	virtual ~BoardWindow();

	// registers the window class; must be called once at application initialization.
	static void RegisterWindowClass(HINSTANCE hInstance);

	// From Observer
	void SubjectChanged();

private:
	// window procedure for the main window.
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	// window procedure for the About dialog.
	static INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

	// GDI+ drawing methods.
	void OnDraw(HDC hdc);
	void DrawGrid(Gdiplus::Graphics *graphics);
	void DrawHighlight(Gdiplus::Graphics* graphics, int row, int column);
	void DrawXMarker(Gdiplus::Graphics* graphics, Gdiplus::Pen* pen, int row, int column);
	void DrawOMarker(Gdiplus::Graphics* graphics, Gdiplus::Pen* pen, int row, int column);
	void DrawGameOver(Gdiplus::Graphics* graphics, WCHAR* message, const Gdiplus::Color& messageColor);

	// coordinate/board position mapping methods.
	int	BoardToClientAreaX(int column);
	int	BoardToClientAreaY(int row);
	int	ClientAreaToBoardColumn(int x);
	int	ClientAreaToBoardRow(int y);

	// region id/board position mapping methods.
	int	BoardPositionToRegionId(int row, int column);
	int	RegionIdToBoardRow(int regionId);
	int	RegionIdToBoardColumn(int regionId);

	// re-calculates the margins based on the size of the window.
	void UpdateMargins();

	// reports all vacant board positions as activatable regions to the EyeX host.
	void UpdateActivatableRegions();

	// makes a move at the given position.
	void MakeHumanPlayerMove(int row, int column);

	// event handlers.
	void OnSquareClicked(POINTS point);
	void OnSquareActivated(UINT regionId);
	void OnNewGameClicked();

	static const Gdiplus::Color BackgroundGazeInteractionFunctionalColor;
	static const Gdiplus::Color BackgroundMouseOnlyColor;
	static const Gdiplus::Color GridColor;
	static const Gdiplus::Color MarkerColor;
	static const Gdiplus::Color HighlightColor;
	static const Gdiplus::Color WonMessageColor;
	static const Gdiplus::Color LostMessageColor;
	static const int SquareSize;
	static const int MarkerMargin;
	static const int MarkerDiameter;
	static const TCHAR* WindowClassName;

	Board& _board;
	EyeXHost _eyeXHost;
	HINSTANCE _hInstance;
	HWND _hWnd;
	HMENU _hMenu;
	int _leftMargin;
	int _upperMargin;
	
	// private copy constructor and operator making the class non-copyable (declared but not implemented).
	BoardWindow(const BoardWindow&);
	BoardWindow& operator = (const BoardWindow&);
};

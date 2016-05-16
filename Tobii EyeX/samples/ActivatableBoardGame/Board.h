/*
 * Board class: Implements the game logic and the not-so-clever AI player for the exciting Gobang game.
 *
 * Copyright 2013 Tobii Technology AB. All rights reserved.
 */

#pragma once

#include <random>

class Observer;

class Board
{
public:
	// Represents a position on the board.
	struct Position
	{
		int row;
		int column;

		Position(int paramRow, int paramColumn) : row(paramRow), column(paramColumn) { }
	};

	// Possible contents of a board position.
	enum Marker
	{
		None,
		X,
		O
	};

	Board(int size);
	virtual ~Board();

	// gets the size of the board, which is assumed to be square: the number of positions on each side.
	int Size() const { return _size; }

	// indicates whether the game is over.
	bool IsGameOver() const { return _playerInTurn == Marker::None; }

	// gets the "name" of the lucky winner.
	Marker GetWinner() const { return _winner; }

	// gets what's on the board at a given position.
	Marker GetMarkerAt(Position position) const;

	// indicates whether it is possible to place a marker at a given position.
	bool CanMakeMoveAt(Position position) const;

	// makes a move for the human player (which will also trigger an AI move).
	void MakeHumanPlayerMove(Position position);

	// restarts the game.
	void BeginNewGame();

	// registers an observer that is notified when the board has changed.
	void RegisterBoardChangedObserver(Observer* boardChangedObserver);

private:
	enum Orientation
	{
		North,
		East,
		Northeast,
		Southeast,
		OrientationMaxValue
	};

	// tests whether a position is on the board.
	bool IsValidPosition(Position position) const;

	// gets a position adjacent to the given one, in a particular direction.
	Position GetAdjacentPosition(Position position, Orientation orientation, bool forward) const;

	// prepares the board for a new, exciting game.
	void InitBoard();

	// lets the miserable AI player make a move.
	void MakeAIPlayerMove();

	// makes a move: places a marker and checks whether anyone has won or if it's a draw game.
	void MakeMove(Position position);

	// places a marker at the given position.
	void SetMarkerAt(Position position, Marker marker);

	// checks if the given position is part of a winning sequence.
	void DetectWinner(Position position);

	// checks if the game is a draw.
	void DetectDrawGame();

	// notifies the observer, if any, that the board has changed.
	void NotifyObserver();

	static const int WinningSequenceLength = 5;

	static std::mt19937 _randomNumberGenerator;

	int _size;
	Marker* _board;
	Marker _playerInTurn;
	Marker _winner;
	Observer* _boardChangedObserver;

	// private copy constructor and operator making the class non-copyable (declared but not implemented).
	Board(const Board&);
	Board& operator = (const Board&);
};

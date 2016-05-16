/*
 * ActivatableBoardGame sample:
 * This is an example that demonstrates the use of Activatable interactors in the context of a board game.
 *
 * Copyright 2013 Tobii Technology AB. All rights reserved.
 */

#include "stdafx.h"
#include "Board.h"
#include <cassert>
#include <vector>
#include "Observer.h"

// initialize the random number generator.
auto const seed = std::random_device()();
std::mt19937 Board::_randomNumberGenerator(seed);

Board::Board(int size)
	: _size(size), _board(nullptr), _boardChangedObserver(nullptr)
{
	_board = new Marker[_size * _size];
	BeginNewGame();
}

Board::~Board()
{
	delete[] _board;
}

void Board::InitBoard()
{
	for (int i = 0; i < _size; ++i)
	{
		for (int j = 0; j < _size; ++j)
		{
			SetMarkerAt(Position(i, j), Marker::None);
		}
	}

	SetMarkerAt(Position((_size - 1) / 2, (_size - 1) / 2), Marker::X);
	SetMarkerAt(Position((_size - 1) / 2, (_size - 1) / 2 + 1), Marker::O);
	SetMarkerAt(Position((_size - 1) / 2 + 1, (_size - 1) / 2), Marker::O);
	SetMarkerAt(Position((_size - 1) / 2 + 1, (_size - 1) / 2 + 1), Marker::X);
}

Board::Marker Board::GetMarkerAt(Position position) const
{
	assert(IsValidPosition(position));

    return _board[position.column + position.row * _size];
}

void Board::SetMarkerAt(Position position, Marker marker)
{
	assert(IsValidPosition(position));

	_board[position.column + position.row * _size] = marker;

	NotifyObserver();
}

void Board::MakeHumanPlayerMove(Position position)
{
	if (_playerInTurn == Marker::X && 
		!IsGameOver() && 
		GetMarkerAt(position) == Marker::None)
	{
		MakeMove(position);

		if (!IsGameOver())
		{
			MakeAIPlayerMove();
		}
	}
}

bool Board::CanMakeMoveAt(Position position) const
{
	return IsValidPosition(position) &&
		GetMarkerAt(position) == Marker::None;
}

void Board::BeginNewGame()
{
	InitBoard();
	_playerInTurn = Marker::X; 
	_winner = Marker::None;
}

bool Board::IsValidPosition(Position position) const
{
	return 0 <= position.row && position.row < _size &&
		0 <= position.column && position.column < _size;
}

void Board::MakeAIPlayerMove()
{
	// generate the list of all possible moves. not that they are good or anything, just possible.
	std::vector<Position> possibleMoves;
	for (int i = 0; i < _size; i++)
	{
		for (int j = 0; j < _size; j++)
		{
			Position position(i, j);
			if (GetMarkerAt(position) == Marker::None)
			{
				possibleMoves.push_back(position);
			}
		}
	}

	// since we check whether the game is a draw before calling this method, there should always be at least one possible move.
	assert(possibleMoves.size() > 0);

	// pick a move at random.
	std::uniform_int_distribution<int> idist(0, (int)possibleMoves.size() - 1); 
	int randomMoveIndex = idist(_randomNumberGenerator);
	MakeMove(possibleMoves[randomMoveIndex]);
}

void Board::MakeMove(Position position)
{
	SetMarkerAt(position, _playerInTurn);
	_playerInTurn = (_playerInTurn == Marker::O) ? Marker::X : Marker::O;

	DetectWinner(position);
	DetectDrawGame();
}

void Board::DetectWinner(Position position)
{
	auto marker = GetMarkerAt(position);

	for (int i = 0; i < OrientationMaxValue; ++i)
	{
		Orientation orientation = (Orientation)i;
		int sequenceLength(1);

		bool directions[] = { true, false };

		for each (auto forward in directions)
		{
			auto neighbor = GetAdjacentPosition(position, orientation, forward);
			while (IsValidPosition(neighbor) && GetMarkerAt(neighbor) == marker)
			{
				sequenceLength++;
				neighbor = GetAdjacentPosition(neighbor, orientation, forward);
			}
		}

		if (sequenceLength >= WinningSequenceLength)
		{
			_winner = marker;
			_playerInTurn = Marker::None;
			NotifyObserver();
		}
	}
}

void Board::DetectDrawGame()
{
	// if there is an empty position, the game isn't a draw (yet).
	for (int i = 0; i < _size; ++i)
	{
		for (int j = 0; j < _size; ++j)
		{
			if (GetMarkerAt(Position(i, j)) == Marker::None)
			{
				return;
			}
		}
	}

	// no empty positions found: it's a draw.
	_playerInTurn = Marker::None;
	NotifyObserver();
}

Board::Position Board::GetAdjacentPosition(Position position, Orientation orientation, bool forward) const
{
	Position delta(-1, -1);
	switch (orientation)
	{
	case Orientation::North:
		delta = Position(+1, 0);
		break;
	case Orientation::East:
		delta = Position(0, +1);
		break;
	case Orientation::Northeast:
		delta = Position(+1, +1);
		break;
	case Orientation::Southeast:
		delta = Position(-1, +1);
		break;
	default:
		assert(false);
	}

	if (!forward)
	{
		delta.row = -delta.row;
		delta.column = -delta.column;
	}

	return Position(position.row + delta.row, position.column + delta.column);
}

void Board::RegisterBoardChangedObserver(Observer* boardChangedObserver)
{
	_boardChangedObserver = boardChangedObserver;
}

void Board::NotifyObserver()
{
	if (_boardChangedObserver)
	{
		_boardChangedObserver->SubjectChanged();
	}
}

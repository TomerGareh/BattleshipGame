#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

using std::string;
using std::unordered_map;
using std::pair;
using std::shared_ptr;

namespace battleship
{
	const size_t BOARD_SIZE = 10;
	const pair<int, int> FORFEIT = std::make_pair<int, int>(-1, -1);

	/* Forward Declarations */
	class BoardBuilder;

	using GamePiecesDict = unordered_map<pair<int, int>, shared_ptr<GamePiece>>;

	enum class ShipType: char
	{
		Empty = ' ',
		Hit = '*',
		RubberBoat = 'B',
		RocketShip = 'P',
		Submarine = 'M',
		Battleship = 'D'
	};

	enum class Orientation: bool
	{
		HORIZONTAL,
		VERTICAL
	};

	enum class PlayerEnum : bool
	{
		A,
		B
	};

	struct GamePiece
	{
	public:
		int _firstX = 0;
		int _firstY = 0;
		int _size = 0;
		Orientation _orient = Orientation::HORIZONTAL;
		PlayerEnum _player = PlayerEnum::A;

		int _lifeLeft = 0;

		GamePiece(int firstX, int firstY, int size, PlayerEnum player, Orientation orientation);
		virtual ~GamePiece();
	};

	class BattleBoard
	{
	public:
		virtual ~BattleBoard();

		const shared_ptr<const GamePiece> executeAttack(pair<int, int> target);

		const char** getBoardMatrix() const;
		const int getPlayerAShipCount() const;
		const int getPlayerBShipCount() const;

		friend class BoardBuilder;

	private:
		char** _matrix = NULL;
		GamePiecesDict _gamePieces;
		int _playerAShipCount = 0;
		int _playerBShipCount = 0;

		BattleBoard();

		/* Edits the board, without adding any game-pieces */
		void initSquare(int x, int y, char type);

		/* Called when the board-matrix is initialized, to assemble game pieces list */
		void addGamePiece(int firstX, int firstY, int size, PlayerEnum player, Orientation orientation);

		void sinkShip(GamePiece* pieceToRemove);
	};
}
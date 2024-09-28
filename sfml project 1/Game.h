#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <thread>
#include <string>

class Game
{
private:
	sf::RenderWindow* window = nullptr;
	sf::VideoMode videoMode;
	sf::Event event;
	/*
	FILE* pipe;
	FILE* wpipe;
	std::string command, response;
	*/
	
	const float squareSize = 75.f;
	sf::Color white, black, backgroundColor;
	sf::RectangleShape squares[8][8];				// The board itself, damn i thought bring it from [64] to [8][8] will be harder
	sf::RenderTexture backgroundRenderTexture;
	sf::Sprite backgroundSprite;
	sf::Texture piecesTexture;
	int piecesSubTextureSize;
	sf::RectangleShape pieces[32];					// P:
	bool piecesAlive[32];							// For me to not render dead pieces
	char piecesIndexes[8][8] = {
		{'R', 'P', 'e', 'e', 'e', 'e', 'p', 'r'},
		{'N', 'P', 'e', 'e', 'e', 'e', 'p', 'n'},
		{'B', 'P', 'e', 'e', 'e', 'e', 'p', 'b'},
		{'Q', 'P', 'e', 'e', 'e', 'e', 'p', 'q'},
		{'K', 'P', 'e', 'e', 'e', 'e', 'p', 'k'},
		{'B', 'P', 'e', 'e', 'e', 'e', 'p', 'b'},
		{'N', 'P', 'e', 'e', 'e', 'e', 'p', 'n'},
		{'R', 'P', 'e', 'e', 'e', 'e', 'p', 'r'}
	};
	bool firstMoves[32];
	sf::CircleShape positionSelection[32];
	int positionSelectionAmount = 0;
	bool viableMoves[8][8];
	char char1, char2, char3, char4;
	bool enPassant[16];								// represent two lines, 3 for white (0 - 7) and 6 for black (8 - 15)
	
	sf::RectangleShape boardBorders;
	sf::RectangleShape boardFrame;
	sf::RectangleShape varRect;
	sf::Texture texture;

	const int gridSize = 8;

	int var1, var2, var3, var4, heldPieceIndex, whiteCount = 0, blackCount = 0;						// Variables for all the functions to use
	bool boardInitialized, lmbHeldOnPiece = false, currentPieceDraw1 = true, currentPieceDraw2 = false, firstMoveDone = false, whiteMove = true;
	bool gameEnd = false, viableMove = true, showViableMoves = false, found, toCheckForViableMoveClick = false, toShowCircles = true;
	sf::Vector2i lastHeldPieceIndex;

	sf::Vector2f center;							// Is a const which is initialized in constructor
	sf::Vector2f boardSize;							// Same
	sf::Vector2i mousePos;
	sf::Vector2f heldPiecePos;
	sf::Vector2i heldPiecePos1, heldPiecePos2, lastMovePos1, lastMovePos2;
	sf::Vector2i vector2iVar;						// A varibles, created in order to reduce the amount of repeated function calls
	float r, g, b, modification, modScale, positionSelectionRadius1, positionSelectionRadius2, boardFrameSize;			// For color modification

	void initWindow();
	void initVarsAndConsts();
	void boardInitializingWithAnimation();
	void initPieces();
public:
	Game();
	~Game();

	sf::Color modifyColor(const sf::Color& color, float factor);

	const bool isRunning() const;

	sf::Vector2f getSquareCoords(sf::Vector2i squareIndex);	// Probably is not needed
	sf::Vector2i getSquareUnderMouse();
	bool viableMovesShow(int pieceIndex, bool toShow, bool& showViableMoves, int& positionSelectionAmount);
	bool checkForAttack(int j, int i, bool& toShowCircles, sf::RectangleShape piece);
	void placeCircles(int var1, int var2, int& positionSelectionAmount, bool toEat);
	void pawn(int j, int i, bool white, int& positionSelectionAmount);
	void kingKnight(int var1, int var2, int j, int i, bool white, int& positionSelectionAmount);
	void rookBishop(int var1, int var2, int j, int i, bool white, int& positionSelectionAmount);
	void castling(int j, int i, bool white, int& positionSelectionAmount);
	void getMove(sf::Vector2i pos1, sf::Vector2i pos2);
	void count(int& whiteCount, int& blackCount, bool& gameEnd);
	void move(bool& whiteMove, int& heldPieceIndex, bool& gameEnd, bool piecesAlive[32], char piecesIndexes[8][8], bool firstMoves[32], sf::RectangleShape pieces[32], sf::RectangleShape squares[8][8]);
	void pollEvents();

	void update();
	void render();
};


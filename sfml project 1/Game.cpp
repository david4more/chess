#include "Game.h"
/*
		CTRL+M CTRL+O and CTRL+M CTRL+L			for opening and closing everything
		CTRL+M CTRL+M							for closing active block
		CTRL+K CTRL+K and CTRL+K CTRL+W			for bookmarking
		CTRL+K CTRL+D							for autoalignment

		So how will my fucking game logic work?

		There we go!

		TODO:
			- stockfish
			- timer
			- choose for side
			- check, mate
			- castling to depend on whether each of squares is being attacked or not
			- check for draw (probably will never add)
*/
#include <cstdio>
#include <string>
#include <fcntl.h>

// xD 
void Game::initVarsAndConsts()
{
	/*
		Were initialized in the header file:
		gridSize
		squares
	*/

	/*
	if (viableMove)
	{
		for (int i = (whiteMove? 0 : 16); i < (whiteMove? 16 : 32); i++)
		{
			if (getSquareCoords(heldPiecePos2).x == pieces[i].getGlobalBounds().left && getSquareCoords(heldPiecePos2).y == pieces[i].getGlobalBounds().top)
				if (i != heldPieceIndex)
				{
					std::cout << "FRIENDLY FIRE!\n";
					pieces[heldPieceIndex].setPosition(getSquareCoords(heldPiecePos1));
					viableMove = false;
					break;
				}
		}
	}
	*/

	lmbHeldOnPiece = false;
	boardInitialized = false;
	heldPieceIndex = -1;

	center = sf::Vector2f(videoMode.width / 2, videoMode.height / 2);

	boardBorders.setSize(sf::Vector2f(squareSize * gridSize, squareSize * gridSize));
	boardBorders.setPosition(sf::Vector2f(center.x - (4 * squareSize), center.y - (4 * squareSize)));

	boardSize = sf::Vector2f(boardBorders.getGlobalBounds().width, boardBorders.getGlobalBounds().height);

	backgroundColor = sf::Color(60, 122, 137);

	piecesTexture.loadFromFile("files/Chess_Pieces_Sprite.png");

	std::fill(std::begin(piecesAlive), std::end(piecesAlive), true);
	std::fill(std::begin(enPassant), std::end(enPassant), false);
	std::fill(firstMoves, firstMoves + 32, true);

	modScale = 0.25f;

	heldPiecePos1 = sf::Vector2i(0, 0);
	heldPiecePos2 = sf::Vector2i(0, 0);
	lastMovePos1 = sf::Vector2i(0, 0);
	lastMovePos2 = sf::Vector2i(0, 0);

	if (piecesTexture.getSize().x / 6 == piecesTexture.getSize().y / 2)
		piecesSubTextureSize = piecesTexture.getSize().x / 6;
	else
		std::cerr << "Texture division error.\n";


	for (int i = 0; i < 32; i++)
	{
		positionSelection[i].setOutlineThickness(squareSize * (0.2f / 2));
	}

	positionSelectionRadius1 = squareSize * 0.25f;
	positionSelectionRadius2 = squareSize * 0.4f;

	boardFrameSize = 45.f;
	boardFrame.setPosition(sf::Vector2f(center.x - (4 * squareSize) - (boardFrameSize / 2), center.y - (4 * squareSize) - (boardFrameSize / 2)));
	boardFrame.setSize(sf::Vector2f((squareSize * gridSize) + boardFrameSize, (squareSize * gridSize) + boardFrameSize));
	/*
	pipe = _popen("stockfish-windows-x86-64-avx2.exe", "r");
	wpipe = _popen("stockfish-windows-x86-64-avx2.exe", "w");

	if (!pipe || !wpipe) {
		std::cerr << "Stockfish load error.\n";
	}

	fprintf(wpipe, command.c_str());
	fflush(wpipe); 

	char buffer[128];
	while (fgets(buffer, sizeof(buffer), pipe) {
		response += buffer;
		if (response.find("bestmove") != std::string::npos) {
			break; // Закриваємо читання, коли отримуємо найкращий хід
		}
	}
	*/
	// position startpos, go movetime 2000
}

Game::Game()
{
	srand(time(NULL));
	initWindow();
	initVarsAndConsts();
	boardInitializingWithAnimation();
	squares[0][0].setFillColor(black);		// In order to not to check, if it is the first moove every time i render,  i will just
	// show the [0][0] square before the first moove and then set fill color to what i need
	initPieces();
}

Game::~Game()
{
	if (window != nullptr)
	{
		delete window;
		window = nullptr;
	}
}

void Game::getMove(sf::Vector2i pos1, sf::Vector2i pos2)
{
	switch (pos1.x)
	{
	case 0:
		char1 = 'a';
		break;
	case 1:
		char1 = 'b';
		break;
	case 2:
		char1 = 'c';
		break;
	case 3:
		char1 = 'd';
		break;
	case 4:
		char1 = 'e';
		break;
	case 5:
		char1 = 'f';
		break;
	case 6:
		char1 = 'g';
		break;
	case 7:
		char1 = 'h';
		break;
	default:
		char1 = 'x';
		break;
	}

	char2 = '1' + pos1.y;

	switch (pos2.x)
	{
	case 0:
		char3 = 'a';
		break;
	case 1:
		char3 = 'b';
		break;
	case 2:
		char3 = 'c';
		break;
	case 3:
		char3 = 'd';
		break;
	case 4:
		char3 = 'e';
		break;
	case 5:
		char3 = 'f';
		break;
	case 6:
		char3 = 'g';
		break;
	case 7:
		char3 = 'h';
		break;
	default:
		char3 = 'x';
		break;
	}

	char4 = '1' + pos2.y;
	/*
	command += char1;
	command += char2;
	command += char3;
	command += char4;
	command = { char1, char2, char3, char4 };
	std::cout << command << '\n';
	*/
	//  fwrite(command.c_str(), sizeof(char), command.size(), pipe);
}

sf::Color Game::modifyColor(const sf::Color& color, float factor)
{
	factor = std::max(-1.f, std::min(1.f, factor));

	modification = factor * 255.f;

	r = color.r + modification;
	g = color.g + modification;
	b = color.b + modification;

	if (r > 255)
		r = 255;
	if (r < 0)
		r = 0;
	if (g > 255)
		g = 255;
	if (g < 0)
		g = 0;
	if (b > 255)
		b = 255;
	if (b < 0)
		b = 0;

	return sf::Color(r, g, b, 255);
}

void Game::update()
{
	pollEvents();
}

void Game::render()
{
	window->clear(backgroundColor);
	window->draw(boardFrame);
	window->draw(backgroundSprite);
	window->draw(squares[lastMovePos1.x][lastMovePos1.y]);
	window->draw(squares[lastMovePos2.x][lastMovePos2.y]);
	if (currentPieceDraw1)
		window->draw(squares[heldPiecePos1.x][heldPiecePos1.y]);

	for (int i = 0; i < 32; i++)
	{
		if (piecesAlive[i])
			window->draw(pieces[i]);
	}
	if (showViableMoves)
	{
		for (int i = 0; i < positionSelectionAmount; i++)
			window->draw(positionSelection[i]);
	}

	if (heldPieceIndex != -1)
		window->draw(pieces[heldPieceIndex]);		// To overlap other pieces with the current
	window->display();
}

sf::Vector2f Game::getSquareCoords(sf::Vector2i squareIndex)
{
	if (squareIndex.x < 0 || squareIndex.x >= 8 || squareIndex.y < 0 || squareIndex.y >= 8)
		return sf::Vector2f(-1.f, -1.f);

	var1 = static_cast<float>(squares[squareIndex.x][squareIndex.y].getGlobalBounds().left);
	var2 = static_cast<float>(squares[squareIndex.x][squareIndex.y].getGlobalBounds().top);

	return sf::Vector2f(var1, var2);
}

sf::Vector2i Game::getSquareUnderMouse()
{
	if (!boardBorders.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
		return sf::Vector2i(-1, -1);

	var1 = static_cast<int>((mousePos.x - boardBorders.getGlobalBounds().left) / squareSize);
	var2 = static_cast<int>((videoMode.height - mousePos.y - boardBorders.getGlobalBounds().top) / squareSize);

	return sf::Vector2i(var1, var2);
}

bool Game::viableMovesShow(int pieceIndex, bool toShow, bool& showViableMoves, int& positionSelectionAmount)
{
	for (int i = 0; i < 32; i++)
		positionSelection[i].setPosition(-squareSize, -squareSize);

	std::fill(&viableMoves[0][0], &viableMoves[0][0] + gridSize * gridSize, false);

	if (!toShow)
	{
		showViableMoves = false;
		return false;
	}

	found = false;
	positionSelectionAmount = 0;

	for (int i = 0; i < gridSize && !found; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			if (pieces[heldPieceIndex].getGlobalBounds().contains(sf::Vector2f(squares[j][i].getGlobalBounds().left, squares[j][i].getGlobalBounds().top)))
			{
				switch (piecesIndexes[j][i])
				{
				case 'k':
				{
					castling(j, i, false, positionSelectionAmount);

					kingKnight(1, 1, j, i, false, positionSelectionAmount);
					kingKnight(-1, 1, j, i, false, positionSelectionAmount);

					kingKnight(1, -1, j, i, false, positionSelectionAmount);
					kingKnight(-1, -1, j, i, false, positionSelectionAmount);

					kingKnight(1, 0, j, i, false, positionSelectionAmount);
					kingKnight(-1, 0, j, i, false, positionSelectionAmount);

					kingKnight(0, 1, j, i, false, positionSelectionAmount);
					kingKnight(0, -1, j, i, false, positionSelectionAmount);
				}
				break;
				case 'q':
				{
					rookBishop(0, 1, j, i, false, positionSelectionAmount);

					rookBishop(0, -1, j, i, false, positionSelectionAmount);

					rookBishop(1, 0, j, i, false, positionSelectionAmount);

					rookBishop(-1, 0, j, i, false, positionSelectionAmount);

					rookBishop(1, 1, j, i, false, positionSelectionAmount);

					rookBishop(1, -1, j, i, false, positionSelectionAmount);

					rookBishop(-1, 1, j, i, false, positionSelectionAmount);

					rookBishop(-1, -1, j, i, false, positionSelectionAmount);
				}
				break;
				case 'r':
				{
					rookBishop(0, 1, j, i, false, positionSelectionAmount);

					rookBishop(0, -1, j, i, false, positionSelectionAmount);

					rookBishop(1, 0, j, i, false, positionSelectionAmount);

					rookBishop(-1, 0, j, i, false, positionSelectionAmount);

				}
				break;
				case 'b':
				{
					rookBishop(1, 1, j, i, false, positionSelectionAmount);

					rookBishop(1, -1, j, i, false, positionSelectionAmount);

					rookBishop(-1, 1, j, i, false, positionSelectionAmount);

					rookBishop(-1, -1, j, i, false, positionSelectionAmount);
				}
				break;
				case 'n':
				{
					kingKnight(1, 2, j, i, false, positionSelectionAmount);
					kingKnight(-1, 2, j, i, false, positionSelectionAmount);

					kingKnight(1, -2, j, i, false, positionSelectionAmount);
					kingKnight(-1, -2, j, i, false, positionSelectionAmount);

					kingKnight(2, 1, j, i, false, positionSelectionAmount);
					kingKnight(-2, 1, j, i, false, positionSelectionAmount);

					kingKnight(2, -1, j, i, false, positionSelectionAmount);
					kingKnight(-2, -1, j, i, false, positionSelectionAmount);
				}
				break;
				case 'p':
					pawn(j, i, false, positionSelectionAmount);
					break;
				case 'K':
				{
					castling(j, i, true, positionSelectionAmount);

					kingKnight(1, 1, j, i, true, positionSelectionAmount);
					kingKnight(-1, 1, j, i, true, positionSelectionAmount);

					kingKnight(1, -1, j, i, true, positionSelectionAmount);
					kingKnight(-1, -1, j, i, true, positionSelectionAmount);

					kingKnight(1, 0, j, i, true, positionSelectionAmount);
					kingKnight(-1, 0, j, i, true, positionSelectionAmount);

					kingKnight(0, 1, j, i, true, positionSelectionAmount);
					kingKnight(0, -1, j, i, true, positionSelectionAmount);
				}
				break;
				case 'Q':
				{
					rookBishop(0, 1, j, i, true, positionSelectionAmount);

					rookBishop(0, -1, j, i, true, positionSelectionAmount);

					rookBishop(1, 0, j, i, true, positionSelectionAmount);

					rookBishop(-1, 0, j, i, true, positionSelectionAmount);

					rookBishop(1, 1, j, i, true, positionSelectionAmount);

					rookBishop(1, -1, j, i, true, positionSelectionAmount);

					rookBishop(-1, 1, j, i, true, positionSelectionAmount);

					rookBishop(-1, -1, j, i, true, positionSelectionAmount);
				}
				break;
				case 'R':
				{
					rookBishop(0, 1, j, i, true, positionSelectionAmount);

					rookBishop(0, -1, j, i, true, positionSelectionAmount);

					rookBishop(1, 0, j, i, true, positionSelectionAmount);

					rookBishop(-1, 0, j, i, true, positionSelectionAmount);
				}
				break;
				case 'B':
				{
					rookBishop(1, 1, j, i, true, positionSelectionAmount);

					rookBishop(1, -1, j, i, true, positionSelectionAmount);

					rookBishop(-1, 1, j, i, true, positionSelectionAmount);

					rookBishop(-1, -1, j, i, true, positionSelectionAmount);
				}
				break;
				case 'N':
				{
					//	Wow this shit is about get yeastless (бездріжжовим)

					// Just kidding

					kingKnight(1, 2, j, i, true, positionSelectionAmount);
					kingKnight(-1, 2, j, i, true, positionSelectionAmount);

					kingKnight(1, -2, j, i, true, positionSelectionAmount);
					kingKnight(-1, -2, j, i, true, positionSelectionAmount);

					kingKnight(2, 1, j, i, true, positionSelectionAmount);
					kingKnight(-2, 1, j, i, true, positionSelectionAmount);

					kingKnight(2, -1, j, i, true, positionSelectionAmount);
					kingKnight(-2, -1, j, i, true, positionSelectionAmount);
				}
				break;
				case 'P':
					pawn(j, i, true, positionSelectionAmount);
					break;
				default:
					break;
				}
				found = true;
				break;
			}
		}
	}
	showViableMoves = true;
	return true;
}

void Game::placeCircles(int var1, int var2, int& positionSelectionAmount, bool toEat)
{
	if (toShowCircles)
	{
		if (toEat)
		{
			positionSelection[positionSelectionAmount].setFillColor(sf::Color::Transparent);
			positionSelection[positionSelectionAmount].setRadius(positionSelectionRadius2);
			positionSelection[positionSelectionAmount].setOutlineColor(sf::Color(0, 0, 0, 75));
			positionSelection[positionSelectionAmount].setPosition(pieces[heldPieceIndex].getGlobalBounds().left + (var1 * squareSize) + (positionSelectionRadius2 / 4), pieces[heldPieceIndex].getGlobalBounds().top - (var2 * squareSize) + (positionSelectionRadius2 / 4));
			positionSelectionAmount++;
		}
		else
		{
			positionSelection[positionSelectionAmount].setFillColor(sf::Color(0, 0, 0, 75));
			positionSelection[positionSelectionAmount].setRadius(positionSelectionRadius1);
			positionSelection[positionSelectionAmount].setOutlineColor(sf::Color::Transparent);
			positionSelection[positionSelectionAmount].setPosition(pieces[heldPieceIndex].getGlobalBounds().left + (var1 * squareSize) + (positionSelectionRadius1), pieces[heldPieceIndex].getGlobalBounds().top - (var2 * squareSize) + (positionSelectionRadius1));
			positionSelectionAmount++;
		}
	}
}

//						TODO
bool Game::checkForAttack(int x, int y, bool& toShowCircles, sf::RectangleShape piece)
{
	std::fill(&viableMoves[0][0], &viableMoves[0][0] + gridSize * gridSize, false);

	var1 = static_cast<int>(((piece.getGlobalBounds().left + (squareSize / 2)) - boardBorders.getGlobalBounds().left) / squareSize);
	var2 = static_cast<int>((videoMode.height - (piece.getGlobalBounds().top + (squareSize / 2)) - boardBorders.getGlobalBounds().top) / squareSize);

	toShowCircles = false;
	for (int i = 0; i < gridSize && !found; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			switch (piecesIndexes[var1][var2])
			{
			case 'k':
			{
				castling(j, i, false, positionSelectionAmount);

				kingKnight(1, 1, j, i, false, positionSelectionAmount);
				kingKnight(-1, 1, j, i, false, positionSelectionAmount);

				kingKnight(1, -1, j, i, false, positionSelectionAmount);
				kingKnight(-1, -1, j, i, false, positionSelectionAmount);

				kingKnight(1, 0, j, i, false, positionSelectionAmount);
				kingKnight(-1, 0, j, i, false, positionSelectionAmount);

				kingKnight(0, 1, j, i, false, positionSelectionAmount);
				kingKnight(0, -1, j, i, false, positionSelectionAmount);
			}
			break;
			case 'q':
			{
				rookBishop(0, 1, j, i, false, positionSelectionAmount);

				rookBishop(0, -1, j, i, false, positionSelectionAmount);

				rookBishop(1, 0, j, i, false, positionSelectionAmount);

				rookBishop(-1, 0, j, i, false, positionSelectionAmount);

				rookBishop(1, 1, j, i, false, positionSelectionAmount);

				rookBishop(1, -1, j, i, false, positionSelectionAmount);

				rookBishop(-1, 1, j, i, false, positionSelectionAmount);

				rookBishop(-1, -1, j, i, false, positionSelectionAmount);
			}
			break;
			case 'r':
			{
				rookBishop(0, 1, j, i, false, positionSelectionAmount);

				rookBishop(0, -1, j, i, false, positionSelectionAmount);

				rookBishop(1, 0, j, i, false, positionSelectionAmount);

				rookBishop(-1, 0, j, i, false, positionSelectionAmount);

			}
			break;
			case 'b':
			{
				rookBishop(1, 1, j, i, false, positionSelectionAmount);

				rookBishop(1, -1, j, i, false, positionSelectionAmount);

				rookBishop(-1, 1, j, i, false, positionSelectionAmount);

				rookBishop(-1, -1, j, i, false, positionSelectionAmount);
			}
			break;
			case 'n':
			{
				kingKnight(1, 2, j, i, false, positionSelectionAmount);
				kingKnight(-1, 2, j, i, false, positionSelectionAmount);

				kingKnight(1, -2, j, i, false, positionSelectionAmount);
				kingKnight(-1, -2, j, i, false, positionSelectionAmount);

				kingKnight(2, 1, j, i, false, positionSelectionAmount);
				kingKnight(-2, 1, j, i, false, positionSelectionAmount);

				kingKnight(2, -1, j, i, false, positionSelectionAmount);
				kingKnight(-2, -1, j, i, false, positionSelectionAmount);
			}
			break;
			case 'p':
				pawn(j, i, false, positionSelectionAmount);
				break;
			case 'K':
			{
				castling(j, i, true, positionSelectionAmount);

				kingKnight(1, 1, j, i, true, positionSelectionAmount);
				kingKnight(-1, 1, j, i, true, positionSelectionAmount);

				kingKnight(1, -1, j, i, true, positionSelectionAmount);
				kingKnight(-1, -1, j, i, true, positionSelectionAmount);

				kingKnight(1, 0, j, i, true, positionSelectionAmount);
				kingKnight(-1, 0, j, i, true, positionSelectionAmount);

				kingKnight(0, 1, j, i, true, positionSelectionAmount);
				kingKnight(0, -1, j, i, true, positionSelectionAmount);
			}
			break;
			case 'Q':
			{
				rookBishop(0, 1, j, i, true, positionSelectionAmount);

				rookBishop(0, -1, j, i, true, positionSelectionAmount);

				rookBishop(1, 0, j, i, true, positionSelectionAmount);

				rookBishop(-1, 0, j, i, true, positionSelectionAmount);

				rookBishop(1, 1, j, i, true, positionSelectionAmount);

				rookBishop(1, -1, j, i, true, positionSelectionAmount);

				rookBishop(-1, 1, j, i, true, positionSelectionAmount);

				rookBishop(-1, -1, j, i, true, positionSelectionAmount);
			}
			break;
			case 'R':
			{
				rookBishop(0, 1, j, i, true, positionSelectionAmount);

				rookBishop(0, -1, j, i, true, positionSelectionAmount);

				rookBishop(1, 0, j, i, true, positionSelectionAmount);

				rookBishop(-1, 0, j, i, true, positionSelectionAmount);
			}
			break;
			case 'B':
			{
				rookBishop(1, 1, j, i, true, positionSelectionAmount);

				rookBishop(1, -1, j, i, true, positionSelectionAmount);

				rookBishop(-1, 1, j, i, true, positionSelectionAmount);

				rookBishop(-1, -1, j, i, true, positionSelectionAmount);
			}
			break;
			case 'N':
			{
				kingKnight(1, 2, j, i, true, positionSelectionAmount);
				kingKnight(-1, 2, j, i, true, positionSelectionAmount);

				kingKnight(1, -2, j, i, true, positionSelectionAmount);
				kingKnight(-1, -2, j, i, true, positionSelectionAmount);

				kingKnight(2, 1, j, i, true, positionSelectionAmount);
				kingKnight(-2, 1, j, i, true, positionSelectionAmount);

				kingKnight(2, -1, j, i, true, positionSelectionAmount);
				kingKnight(-2, -1, j, i, true, positionSelectionAmount);
			}
			break;
			case 'P':
				pawn(j, i, true, positionSelectionAmount);
				break;
			default:
				break;
			}
			found = true;
			break;
		}
	}
	toShowCircles = true;

	std::cout << var1 << ' ' << var2 << ' ' << piecesIndexes[var1][var2] << '\n';

	std::fill(&viableMoves[0][0], &viableMoves[0][0] + gridSize * gridSize, false);

	if (viableMoves[x][y])
	{
		std::cout << "Attacker: " << var1 << ' ' << var2 << ' ' << piecesIndexes[var1][var2] << '\n';
		return true;
	}
	return false;
}

void Game::castling(int j, int i, bool white, int& positionSelectionAmount)
{
	if (firstMoves[heldPieceIndex])
	{
		/*
		for (int k = (white ? 16 : 0); k < (white ? 32 : 16); k++)
		{
			if (piecesAlive[k])
			{
				if (checkForAttack(5, 0, toShowCircles, pieces[k]))
				{
					std::cout << "50 is under attack!\n";
					break;
				}
			}
		}
		*/

		if (firstMoves[(white ? 0 : 24)] && piecesIndexes[j - 1][i] == 'e' && piecesIndexes[j - 2][i] == 'e' && piecesIndexes[j - 3][i] == 'e' && piecesIndexes[j - 4][i] == (white ? 'R' : 'r'))
		{
			placeCircles(-2, 0, positionSelectionAmount, false);
			positionSelectionAmount++;
			viableMoves[j - 2][i] = true;
		}
		if (firstMoves[(white ? 7 : 31)] && piecesIndexes[j + 1][i] == 'e' && piecesIndexes[j + 2][i] == 'e' && piecesIndexes[j + 3][i] == (white ? 'R' : 'r'))
		{
			placeCircles(2, 0, positionSelectionAmount, false);
			positionSelectionAmount++;
			viableMoves[j + 2][i] = true;
		}
	}
}

void Game::pawn(int j, int i, bool white, int& positionSelectionAmount)
{
	var1 = white ? 1 : -1;

	if (piecesIndexes[j][i + var1] == 'e')
	{
		placeCircles(0, var1, positionSelectionAmount, false);
		viableMoves[j][i + var1] = true;
	}
	if (firstMoves[heldPieceIndex] && piecesIndexes[j][i + var1] == 'e' && piecesIndexes[j][i + (2 * var1)] == 'e')
	{
		placeCircles(0, (2 * var1), positionSelectionAmount, false);
		viableMoves[j][i + (2 * var1)] = true;
	}
	if (j > 0)
	{
		if (piecesIndexes[j - 1][i + var1] == (white ? 'p' : 'P') || piecesIndexes[j - 1][i + var1] == (white ? 'n' : 'N') || piecesIndexes[j - 1][i + var1] == (white ? 'b' : 'B') || piecesIndexes[j - 1][i + var1] == (white ? 'r' : 'R') || piecesIndexes[j - 1][i + var1] == (white ? 'q' : 'Q') || piecesIndexes[j - 1][i + var1] == (white ? 'k' : 'K'))
		{
			placeCircles(-1, var1, positionSelectionAmount, true);
			viableMoves[j - 1][i + var1] = true;
		}
		else if (enPassant[(white ? j + 7 : j - 1)] && i == (white ? 4 : 3))
		{
			placeCircles(-1, var1, positionSelectionAmount, true);
			viableMoves[j - 1][i + var1] = true;
		}
	}
	if (j < 7)
	{
		if (piecesIndexes[j + 1][i + var1] == (white ? 'p' : 'P') || piecesIndexes[j + 1][i + var1] == (white ? 'n' : 'N') || piecesIndexes[j + 1][i + var1] == (white ? 'b' : 'B') || piecesIndexes[j + 1][i + var1] == (white ? 'r' : 'R') || piecesIndexes[j + 1][i + var1] == (white ? 'q' : 'Q') || piecesIndexes[j + 1][i + var1] == (white ? 'k' : 'K'))
		{
			placeCircles(1, var1, positionSelectionAmount, true);
			viableMoves[j + 1][i + var1] = true;
		}
		else if (enPassant[(white ? j + 9 : j + 1)] && i == (white ? 4 : 3))
		{
			placeCircles(1, var1, positionSelectionAmount, true);
			viableMoves[j + 1][i + var1] = true;
		}
	}
}

void Game::kingKnight(int var1, int var2, int j, int i, bool white, int& positionSelectionAmount)
{
	if (j + var1 >= 0 && j + var1 < gridSize && i + var2 >= 0 && i + var2 < gridSize)
	{
		if (piecesIndexes[j + var1][i + var2] == 'e')
		{
			placeCircles(var1, var2, positionSelectionAmount, false);
			viableMoves[j + var1][i + var2] = true;
		}
		else if (piecesIndexes[j + var1][i + var2] == (white ? 'p' : 'P') || piecesIndexes[j + var1][i + var2] == (white ? 'n' : 'N') || piecesIndexes[j + var1][i + var2] == (white ? 'b' : 'B') || piecesIndexes[j + var1][i + var2] == (white ? 'r' : 'R') || piecesIndexes[j + var1][i + var2] == (white ? 'q' : 'Q') || piecesIndexes[j + var1][i + var2] == (white ? 'k' : 'K'))
		{
			placeCircles(var1, var2, positionSelectionAmount, true);
			viableMoves[j + var1][i + var2] = true;
		}
	}
}

void Game::rookBishop(int var1, int var2, int j, int i, bool white, int& positionSelectionAmount)
{
	var3 = var1, var4 = var2;
	while (j + var1 >= 0 && j + var1 < gridSize && i + var2 >= 0 && i + var2 < gridSize)
	{
		if (piecesIndexes[j + var1][i + var2] == 'e')
		{
			placeCircles(var1, var2, positionSelectionAmount, false);
			viableMoves[j + var1][i + var2] = true;
			var1 += var3, var2 += var4;
		}
		else if (piecesIndexes[j + var1][i + var2] == (white ? 'p' : 'P') || piecesIndexes[j + var1][i + var2] == (white ? 'n' : 'N') || piecesIndexes[j + var1][i + var2] == (white ? 'b' : 'B') || piecesIndexes[j + var1][i + var2] == (white ? 'r' : 'R') || piecesIndexes[j + var1][i + var2] == (white ? 'q' : 'Q') || piecesIndexes[j + var1][i + var2] == (white ? 'k' : 'K'))
		{
			placeCircles(var1, var2, positionSelectionAmount, true);
			viableMoves[j + var1][i + var2] = true;
			break;
		}
		else
		{
			break;
		}
	}
}

void Game::move(bool& whiteMove, int& heldPieceIndex, bool& gameEnd, bool piecesAlive[32], char piecesIndexes[8][8], bool firstMoves[32], sf::RectangleShape pieces[32], sf::RectangleShape squares[8][8])
{
	getMove(lastMovePos1, lastMovePos2);

	firstMoves[heldPieceIndex] = false;

	// castling
	if (piecesIndexes[lastMovePos1.x][lastMovePos1.y] == (whiteMove ? 'K' : 'k'))
	{
		if (lastMovePos2.x == lastMovePos1.x - 2)
		{
			pieces[(whiteMove ? 0 : 24)].setPosition(getSquareCoords(sf::Vector2i(lastMovePos2.x + 1, lastMovePos2.y)));
			piecesIndexes[0][(whiteMove ? 0 : 7)] = 'e';
			piecesIndexes[3][(whiteMove ? 0 : 7)] = (whiteMove ? 'R' : 'r');
		}
		if (lastMovePos2.x == lastMovePos1.x + 2)
		{
			pieces[(whiteMove ? 7 : 31)].setPosition(getSquareCoords(sf::Vector2i(lastMovePos2.x - 1, lastMovePos2.y)));
			piecesIndexes[7][(whiteMove ? 0 : 7)] = 'e';
			piecesIndexes[5][(whiteMove ? 0 : 7)] = (whiteMove ? 'R' : 'r');
		}
	}

	// en passant
	if (piecesIndexes[lastMovePos1.x][lastMovePos1.y] == (whiteMove ? 'P' : 'p'))
	{
		if (lastMovePos2.y == lastMovePos1.y + (whiteMove ? 1 : -1) && lastMovePos2.x == lastMovePos1.x + 1)
		{
			if (piecesIndexes[lastMovePos2.x][lastMovePos2.y] == 'e')
			{
				std::cout << piecesIndexes[lastMovePos2.x][lastMovePos2.y + (whiteMove ? -1 : 1)] << '\n';
				piecesIndexes[lastMovePos2.x][lastMovePos2.y + (whiteMove ? -1 : 1)] = 'e';
				pieces[heldPieceIndex + (whiteMove ? 9 : -7)].setPosition(-squareSize, -squareSize);
				whiteMove ? whiteCount++ : blackCount++;
				count(whiteCount, blackCount, gameEnd);
			}
		}
		if (lastMovePos2.y == lastMovePos1.y + (whiteMove ? 1 : -1) && lastMovePos2.x == lastMovePos1.x - 1)
		{
			if (piecesIndexes[lastMovePos2.x][lastMovePos2.y] == 'e')
			{
				std::cout << piecesIndexes[lastMovePos2.x][lastMovePos2.y + (whiteMove ? -1 : 1)] << '\n';
				piecesIndexes[lastMovePos2.x][lastMovePos2.y + (whiteMove ? -1 : 1)] = 'e';
				pieces[heldPieceIndex + (whiteMove ? 7 : -9)].setPosition(-squareSize, -squareSize);
				whiteMove ? whiteCount++ : blackCount++;
				count(whiteCount, blackCount, gameEnd);
			}
		}
	}

	std::fill(std::begin(enPassant), std::end(enPassant), false);

	if (piecesIndexes[lastMovePos1.x][lastMovePos1.y] == (whiteMove ? 'P' : 'p') && lastMovePos2.y == lastMovePos1.y + (whiteMove ? 2 : -2))
		enPassant[(whiteMove ? lastMovePos1.x : (8 + lastMovePos1.x))] = true;


	for (int i = 0; i < 32; i++)
	{
		if (squares[lastMovePos2.x][lastMovePos2.y].getGlobalBounds().contains(sf::Vector2f(pieces[i].getGlobalBounds().left, pieces[i].getGlobalBounds().top)))
		{
			if (i != heldPieceIndex)
			{
				count(whiteCount, blackCount, gameEnd);
				piecesAlive[i] = false;
				pieces[i].setPosition(-squareSize, -squareSize);
				break;
			}
		}
	}

	piecesIndexes[lastMovePos2.x][lastMovePos2.y] = piecesIndexes[lastMovePos1.x][lastMovePos1.y];
	piecesIndexes[lastMovePos1.x][lastMovePos1.y] = 'e';

	// Needed for optimization
	if (!firstMoveDone)
		squares[0][0].setFillColor(modifyColor(black, modScale));
	firstMoveDone = true;

	whiteMove = !whiteMove;
	heldPieceIndex = -1;
}

void Game::count(int& whiteCount, int& blackCount, bool& gameEnd)
{
	switch (piecesIndexes[lastMovePos2.x][lastMovePos2.y])
	{
	case 'k':
		whiteCount += 100500;
		gameEnd = true;
		break;
	case 'q':
		whiteCount += 9;
		break;
	case 'r':
		whiteCount += 5;
		break;
	case 'b':
		whiteCount += 3;
		break;
	case 'n':
		whiteCount += 3;
		break;
	case 'p':
		whiteCount += 1;
		break;
	case 'K':
		blackCount += 100500;
		gameEnd = true;
		break;
	case 'Q':
		blackCount += 9;
		break;
	case 'R':
		blackCount += 5;
		break;
	case 'B':
		blackCount += 3;
		break;
	case 'N':
		blackCount += 3;
		break;
	case 'P':
		blackCount += 1;
		break;
	default:
		break;
	}

	std::cout << "White count: " << whiteCount << "\nBlack count: " << blackCount << "\n\n";
}

void Game::pollEvents()
{
	mousePos = sf::Mouse::getPosition(*window);
	while (window->pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			window->close();
			break;
		case sf::Event::KeyPressed:
			switch (event.key.code)		// Internal switch for all keyboard inputs
			{
			case sf::Keyboard::Escape:
				window->close();
				break;
			}
			break;
		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				for (int i = (whiteMove ? 0 : 16); i < (whiteMove ? 16 : 32); i++)
				{
					if (pieces[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
					{
						heldPieceIndex = i;
						heldPiecePos1 = getSquareUnderMouse();
						heldPiecePos = sf::Vector2f(squares[heldPiecePos1.x][heldPiecePos1.y].getGlobalBounds().left, squares[heldPiecePos1.x][heldPiecePos1.y].getGlobalBounds().top);
						lmbHeldOnPiece = true;

						viableMovesShow(heldPieceIndex, true, showViableMoves, positionSelectionAmount);

						if (!currentPieceDraw1 && currentPieceDraw2)
						{
							currentPieceDraw1 = true;
							currentPieceDraw2 = false;
						}

						toCheckForViableMoveClick = false;

						break;
					}
				}
				if (toCheckForViableMoveClick)
				{
					if (viableMoves[getSquareUnderMouse().x][getSquareUnderMouse().y])
					{
						viableMovesShow(heldPieceIndex, false, showViableMoves, positionSelectionAmount);

						lastMovePos1 = heldPiecePos1;
						lastMovePos2 = getSquareUnderMouse();
						pieces[heldPieceIndex].setPosition(squares[lastMovePos2.x][lastMovePos2.y].getGlobalBounds().left, squares[lastMovePos2.x][lastMovePos2.y].getGlobalBounds().top);

						move(whiteMove, heldPieceIndex, gameEnd, piecesAlive, piecesIndexes, firstMoves, pieces, squares);
					}
				}

				toCheckForViableMoveClick = true;
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if (lmbHeldOnPiece)
				{
					if (getSquareUnderMouse().x == -1)
					{
						pieces[heldPieceIndex].setPosition(heldPiecePos);
					}
					else
					{
						heldPiecePos2 = getSquareUnderMouse();
						pieces[heldPieceIndex].setPosition(squares[heldPiecePos2.x][heldPiecePos2.y].getGlobalBounds().left, squares[heldPiecePos2.x][heldPiecePos2.y].getGlobalBounds().top);

						if (currentPieceDraw1)
						{
							if (currentPieceDraw2 && heldPiecePos1 == heldPiecePos2)
								if (lastHeldPieceIndex == heldPiecePos2)
								{
									currentPieceDraw1 = false;
									viableMovesShow(heldPieceIndex, false, showViableMoves, positionSelectionAmount);
								}
							if (currentPieceDraw1)
							{
								lastHeldPieceIndex = getSquareUnderMouse();
								currentPieceDraw2 = true;
							}
						}

						if (heldPiecePos1 != heldPiecePos2) // Must have different x or y axis 
						{
							for (int i = 0; i < 32; i++)
							{
								if (viableMoves[heldPiecePos2.x][heldPiecePos2.y])
								{
									break;
								}
								else if (i == 31)
								{
									pieces[heldPieceIndex].setPosition(getSquareCoords(heldPiecePos1));
									viableMove = false;
									break;
								}
							}

							viableMovesShow(heldPieceIndex, false, showViableMoves, positionSelectionAmount);

							currentPieceDraw1 = false;

							if (viableMove) // (this means that move has been successfully made)
							{
								lastMovePos1 = heldPiecePos1;
								lastMovePos2 = heldPiecePos2;

								move(whiteMove, heldPieceIndex, gameEnd, piecesAlive, piecesIndexes, firstMoves, pieces, squares);
							}
							viableMove = true;
						}
					}
					// heldPieceIndex = -1;
				}
				lmbHeldOnPiece = false;
			}
			break;
		}
	}
	if (lmbHeldOnPiece)
		pieces[heldPieceIndex].setPosition(static_cast<float>(mousePos.x) - (squareSize / 2), static_cast<float>(mousePos.y) - (squareSize / 2));
}

void Game::initPieces()
{
	var1 = 0;
	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			if (piecesIndexes[j][i] == 'e')
				continue;
			pieces[var1].setSize(sf::Vector2f(squareSize, squareSize));
			pieces[var1].setTexture(&piecesTexture);
			pieces[var1].setPosition(sf::Vector2f(center.x - ((4 - j) * squareSize), center.y + ((3 - i) * squareSize)));
			switch (piecesIndexes[j][i])
			{
			case 'k':
				pieces[var1].setTextureRect(sf::IntRect(piecesSubTextureSize * 0, piecesSubTextureSize, piecesSubTextureSize, piecesSubTextureSize)); break;
			case 'q':
				pieces[var1].setTextureRect(sf::IntRect(piecesSubTextureSize * 1, piecesSubTextureSize, piecesSubTextureSize, piecesSubTextureSize)); break;
			case 'b':
				pieces[var1].setTextureRect(sf::IntRect(piecesSubTextureSize * 2, piecesSubTextureSize, piecesSubTextureSize, piecesSubTextureSize)); break;
			case 'n':
				pieces[var1].setTextureRect(sf::IntRect(piecesSubTextureSize * 3, piecesSubTextureSize, piecesSubTextureSize, piecesSubTextureSize)); break;
			case 'r':
				pieces[var1].setTextureRect(sf::IntRect(piecesSubTextureSize * 4, piecesSubTextureSize, piecesSubTextureSize, piecesSubTextureSize)); break;
			case 'p':
				pieces[var1].setTextureRect(sf::IntRect(piecesSubTextureSize * 5, piecesSubTextureSize, piecesSubTextureSize, piecesSubTextureSize)); break;
			case 'K':
				pieces[var1].setTextureRect(sf::IntRect(piecesSubTextureSize * 0, piecesSubTextureSize * 0, piecesSubTextureSize, piecesSubTextureSize)); break;
			case 'Q':
				pieces[var1].setTextureRect(sf::IntRect(piecesSubTextureSize * 1, piecesSubTextureSize * 0, piecesSubTextureSize, piecesSubTextureSize)); break;
			case 'B':
				pieces[var1].setTextureRect(sf::IntRect(piecesSubTextureSize * 2, piecesSubTextureSize * 0, piecesSubTextureSize, piecesSubTextureSize)); break;
			case 'N':
				pieces[var1].setTextureRect(sf::IntRect(piecesSubTextureSize * 3, piecesSubTextureSize * 0, piecesSubTextureSize, piecesSubTextureSize)); break;
			case 'R':
				pieces[var1].setTextureRect(sf::IntRect(piecesSubTextureSize * 4, piecesSubTextureSize * 0, piecesSubTextureSize, piecesSubTextureSize)); break;
			case 'P':
				pieces[var1].setTextureRect(sf::IntRect(piecesSubTextureSize * 5, piecesSubTextureSize * 0, piecesSubTextureSize, piecesSubTextureSize)); break;
			}
			var1++;
		}
	}
}

void Game::boardInitializingWithAnimation()
{
	// Randomly choosing a set of classic board colors
	switch (rand() % 4)
	{
	case 0:
		black = sf::Color(118, 150, 86);
		white = sf::Color(238, 238, 210);
		boardFrame.setFillColor(sf::Color(76, 39, 25));
		break;
	case 1:
		black = sf::Color(180, 136, 97);
		white = sf::Color(237, 217, 188);
		boardFrame.setFillColor(sf::Color(86, 44, 44));
		break;
	case 2:
		black = sf::Color(112, 134, 184);
		white = sf::Color(238, 238, 236);
		boardFrame.setFillColor(sf::Color(70, 63, 58));
		break;
	case 3:
		black = sf::Color(88, 111, 124);
		white = sf::Color(244, 244, 249);
		boardFrame.setFillColor(sf::Color(33, 37, 41));
		break;
	}

	// Initializing board with a cute animation
	backgroundRenderTexture.create(window->getSize().x, window->getSize().y);
	vector2iVar = sf::Vector2i(center.x - (4 * squareSize), center.y + (3 * squareSize));

	for (int i = 0; i < gridSize && !boardInitialized; i++)
	{
		for (int j = 0; j < gridSize && !boardInitialized; j++)
		{
			if ((j + i) % 2 == 0)
				squares[j][i].setFillColor(black);
			else
				squares[j][i].setFillColor(white);

			squares[j][i].setSize(sf::Vector2f(squareSize, squareSize));
			squares[j][i].setPosition(sf::Vector2f(vector2iVar.x + (j * squareSize), vector2iVar.y - (i * squareSize)));

			window->clear(backgroundColor);
			backgroundRenderTexture.draw(squares[j][i]);
			backgroundRenderTexture.display();										// new line idk what for, but it works!
			backgroundSprite.setTexture(backgroundRenderTexture.getTexture());
			window->draw(backgroundSprite);
			window->display();

			if ((j + i) % 2 == 0)
				squares[j][i].setFillColor(modifyColor(black, modScale));
			else
				squares[j][i].setFillColor(modifyColor(white, -modScale));

			// sf::sleep(sf::milliseconds(200));

			if (i == gridSize - 1 && j == gridSize - 1)
				boardInitialized = true;
		}
	}
}

void Game::initWindow()
{
	videoMode.width = 1366;		// 1366
	videoMode.height = 768;		// 768
	window = new sf::RenderWindow(videoMode, "The BUTT (_|_)");
	window->setFramerateLimit(60);
}

const bool Game::isRunning() const
{
	return window->isOpen();
}

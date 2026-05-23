#include <iostream>
#include "board.h"
#include "move.h"
#include "movegen.h"
#include <chrono>
#include "evaluate.h"
#include "search.h"

uint64_t perft(Board &b, int depth){
	if(depth == 0) return 1ULL;
	uint64_t nodes = 0;
	std::vector<Move> moves = generateLegalMoves(b);
	for(Move m: moves){
		UndoInfo undo = makeMove(b, m);
		nodes += perft(b, depth - 1);
		unMakeMove(b, m, undo);
	}
	return nodes;
}

void runPerft(Board &b, int depth){
	auto start = std::chrono::high_resolution_clock::now();

	uint64_t nodes = perft(b, depth);

	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed = end - start;
	double seconds = elapsed.count();

	uint64_t nps = (seconds > 0) ? (uint64_t)(nodes/seconds) : nodes;
	// Output results
    std::cout << "\n--- Perft Results ---" << std::endl;
    std::cout << "Depth:  " << depth << std::endl;
    std::cout << "Nodes:  " << nodes << std::endl;
    std::cout << "Time:   " << seconds << "s" << std::endl;
    std::cout << "NPS:    " << nps << std::endl;
}

int main(void){
	std::cout << "Chess Engine Alpha\n";

	Board myBoard = generateBoard();
	initKnightMoveTable();
	initBishopMoveTable();
	initRookMoveTable();
	initKingMoveTable();

	int searchDepth = 4;

	while(true){
		std::vector<Move> legalMoves = generateLegalMoves(myBoard);
		if(legalMoves.empty()){
			int kingSq = std::countr_zero(myBoard.pieces[(myBoard.whiteMove ? KING_W : KING_B)]);
			if(isSquareAttacked(myBoard, kingSq, (myBoard.whiteMove ? BLACK : WHITE))){
				std::cout << "Checkmate! " << (myBoard.whiteMove ? "Black" : "White") << " wins!" << std::endl;
			}else{
				std::cout << "Stalemate!" << std::endl;
			}
			break;
		}
		printBitboard(myBoard.allPieces);

		if (myBoard.whiteMove){
			std::string input;
			std::cout << "Your move (e.g. e2e4): ";
			std::cin >> input;
			if(input == "quit") break;
			Move m  = parseMove(input, myBoard);
			if(m.data == 0){
				std::cout << "Invalid Move!" << std::endl;
				continue;
			}
			makeMove(myBoard, m);
		}else{
			std::cout << "Engine is thinking at depth " << searchDepth << std::endl;
			Move bestMove = searchBestMove(myBoard, searchDepth);
			makeMove(myBoard, bestMove);

			char f1 = 'a' + (bestMove.getFrom() % 8);
            char r1 = '1' + (bestMove.getFrom() / 8);
            char f2 = 'a' + (bestMove.getTo() % 8);
            char r2 = '1' + (bestMove.getTo() / 8);
            std::cout << "Engine played: " << f1 << r1 << f2 << r2 << "\n";
		}
	}

	//runPerft(myBoard, 5);

	// while (true)
	// {
	// 	std::vector<Move> moveList = generateLegalMoves(myBoard);

	// 	if(moveList.empty()){
	// 		std::cout << "GAME OVER!" << std::endl;
	// 		break;
	// 	}

	// 	for(const Move& m : moveList){
	// 		std::cout << "Move: " << m.getFrom() << " to " << m.getTo() << std::endl; 
	// 	}
	// 	printBitboard(myBoard.allPieces);
	// 	std::string input;
	// 	std::cout << (myBoard.whiteMove ? "White" : "Black") << " to move: ";
	// 	std::cin >> input;

	// 	if(input == "quit") break;

	// 	Move m  = parseMove(input, myBoard);
	// 	if(m.data == 0){
	// 		std::cout << "Invalid Move!" << std::endl;
	// 		continue;
	// 	}
	// 	makeMove(myBoard, m);

	// 	std::cout << "Evaluation: " << evaluate(myBoard) << std::endl;
	// }
	

	return 0;
}

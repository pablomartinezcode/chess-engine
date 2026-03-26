#include <iostream>
#include "board.h"
#include "move.h"
#include "movegen.h"
#include <chrono>

uint64_t perft(Board &b, int depth){
	if(depth == 0) return 1ULL;
	uint64_t nodes = 0;
	std::vector<Move> moves = generateLegalMoves(b);
	for(Move m: moves){
		Board temp = b;
		makeMove(temp, m);
		nodes += perft(temp, depth - 1);
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

	runPerft(myBoard, 6);

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
	// }
	

	return 0;
}

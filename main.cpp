#include <iostream>
#include "board.h"
#include "move.h"
#include "movegen.h"


int main(void){
	std::cout << "Chess Engine Alpha\n";

	Board myBoard = generateBoard();
	


	
	
	while (true)
	{
		std::vector<Move> moveList;
		generatePawnMoves(myBoard, moveList);
		for(const Move& m : moveList){
			std::cout << "Move: " << m.getFrom() << " to " << m.getTo() << std::endl; 
		}
		printBitboard(myBoard.allPieces);
		std::string input;
		std::cout << (myBoard.whiteMove ? "White" : "Black") << " to move: ";
		std::cin >> input;

		if(input == "quit") break;

		Move m  = parseMove(input);
		makeMove(myBoard, m);
	}
	

	return 0;
}

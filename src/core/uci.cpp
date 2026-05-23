#include "uci.h"
#include "search.h"
#include "movegen.h"
#include "tt.h"
#include "zobrist.h"
#include <iostream>
#include <string>
#include <sstream>

// Helper to convert our internal Move to a UCI string (e.g., e2e4, e7e8q)
std::string moveToUCI(Move m) {
    if (m.data == 0) return "0000";
    std::string s = "";
    s += (char)('a' + (m.getFrom() % 8));
    s += (char)('1' + (m.getFrom() / 8));
    s += (char)('a' + (m.getTo() % 8));
    s += (char)('1' + (m.getTo() / 8));
    
    int f = m.getFlags();
    // Append promotion piece if applicable
    if (f == PROMOTION_Q || f == PROMO_CAPTURE_Q) s += 'q';
    else if (f == PROMOTION_R || f == PROMO_CAPTURE_R) s += 'r';
    else if (f == PROMOTION_B || f == PROMO_CAPTURE_B) s += 'b';
    else if (f == PROMOTION_N || f == PROMO_CAPTURE_N) s += 'n';
    
    return s;
}

void uciLoop(Board& b) {
    std::string line;
    
    // Critical: Disable output buffering so the GUI receives messages instantly
    std::cout.setf(std::ios::unitbuf); 

    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string command;
        ss >> command;

        if (command == "uci") {
            // Identify the engine to the GUI
            std::cout << "id name Chess Engine Alpha" << std::endl;
            std::cout << "id author You" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (command == "isready") {
            // GUI checking if we are initialized
            std::cout << "readyok" << std::endl;
        } else if (command == "ucinewgame") {
            // Clear the Transposition Table memory for a fresh game
            clearTT();
        } else if (command == "position") {
            std::string posType;
            ss >> posType;
            
            // Re-initialize the board state
            if (posType == "startpos") {
                b = generateBoard();
                b.hashKey = generateHash(b); // Crucial: Rebuild the starting hash
            }
            
            // If the GUI sends moves, loop through and play them on the board
            std::string token;
            ss >> token;
            if (token == "moves") {
                std::string moveStr;
                while (ss >> moveStr) {
                    Move m = parseMove(moveStr, b);
                    if (m.data != 0) {
                        makeMove(b, m);
                    }
                }
            }
        } else if (command == "go") {
            // GUI told us to think. For now, hardcode depth 6. 
            // Later we will parse 'go wtime X btime Y' for clock management.
            Move bestMove = searchBestMove(b, 6);
            std::cout << "bestmove " << moveToUCI(bestMove) << std::endl;
        } else if (command == "quit") {
            break;
        }
    }
}
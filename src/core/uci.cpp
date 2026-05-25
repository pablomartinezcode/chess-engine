#include "uci.h"
#include "search.h"
#include "movegen.h"
#include "tt.h"
#include "zobrist.h"
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>

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
            
            if (posType == "startpos") {
                b = generateBoard();
                b.hashKey = generateHash(b); 
            } else if (posType == "fen") {
                // PyChess sends FEN as 6 separate string tokens, we must read and combine them
                std::string fenPart, color, castling, ep, half, full;
                ss >> fenPart >> color >> castling >> ep >> half >> full;
                std::string fullFen = fenPart + " " + color + " " + castling + " " + ep + " " + half + " " + full;
                
                parseFEN(fullFen, b);
                b.hashKey = generateHash(b);
            }
            
            // Check if the GUI appended "moves" to the end of the position string
            std::string token;
            if (ss >> token && token == "moves") {
                std::string moveStr;
                while (ss >> moveStr) {
                    Move m = parseMove(moveStr, b);
                    if (m.data != 0) {
                        makeMove(b, m);
                    }
                }
            }
        } else if (command == "go") {
            int movetimeMs = -1;
            int depth = -1;
            int wtime = -1, btime = -1;
            int winc = 0, binc = 0;
            int movestogo = -1;

            std::string token;
            while (ss >> token) {
                if (token == "movetime") ss >> movetimeMs;
                else if (token == "depth") ss >> depth;
                else if (token == "wtime") ss >> wtime;
                else if (token == "btime") ss >> btime;
                else if (token == "winc") ss >> winc;
                else if (token == "binc") ss >> binc;
                else if (token == "movestogo") ss >> movestogo;
            }

            Move bestMove;
            if (movetimeMs > 0) {
                bestMove = searchBestMoveTimed(b, std::chrono::milliseconds(movetimeMs));
            } else if (depth > 0) {
                bestMove = searchBestMove(b, depth);
            } else if ((b.whiteMove && wtime > 0) || (!b.whiteMove && btime > 0)) {
                int remaining = b.whiteMove ? wtime : btime;
                int inc = b.whiteMove ? winc : binc;
                int movesLeft = (movestogo > 0) ? movestogo : 30;
                int allocation = (remaining / movesLeft) + (inc / 2);
                if (allocation < 5) allocation = 5;
                bestMove = searchBestMoveTimed(b, std::chrono::milliseconds(allocation));
            } else {
                bestMove = searchBestMove(b, 6);
            }
            std::cout << "bestmove " << moveToUCI(bestMove) << std::endl;
        } else if (command == "quit") {
            break;
        }
    }
}

import chess
import chess.engine
import concurrent.futures
import os
import sys

ALPHA_PATH = "./engine"
STOCKFISH_PATH = "stockfish"

def play_single_game(game_id, sf_elo, alpha_is_white):
    """Worker function to play a single game in an isolated thread."""
    try:
        # Each game gets its own instances of the engines
        alpha = chess.engine.SimpleEngine.popen_uci(ALPHA_PATH)
        sf = chess.engine.SimpleEngine.popen_uci(STOCKFISH_PATH)
    except Exception as e:
        return f"Error: {e}"

    sf.configure({"UCI_LimitStrength": True, "UCI_Elo": sf_elo})
    board = chess.Board()

    white_engine = alpha if alpha_is_white else sf
    black_engine = sf if alpha_is_white else alpha

    while not board.is_game_over():
        if board.turn == chess.WHITE:
            result = white_engine.play(board, chess.engine.Limit(time=0.1))
        else:
            result = black_engine.play(board, chess.engine.Limit(time=0.1))
        board.push(result.move)

    outcome = board.outcome()
    
    # Gracefully close the subprocesses to prevent memory leaks
    alpha.quit()
    sf.quit()

    # Determine winner
    if outcome.winner is None:
        return "Draw"
    elif outcome.winner == chess.WHITE:
        return "Alpha" if alpha_is_white else "Stockfish"
    else:
        return "Alpha" if not alpha_is_white else "Stockfish"


def run_parallel_tournament(num_games=100, sf_elo=1320):
    print(f"Starting Parallel Benchmark: Alpha vs Stockfish (Elo {sf_elo})")
    
    # Dynamically scale to your hardware, leaving 1 core for the OS
    max_threads = max(1, os.cpu_count() - 1)
    print(f"Spinning up {max_threads} parallel arenas...\n")

    results = {"Alpha": 0, "Stockfish": 0, "Draw": 0}

    # ThreadPoolExecutor is perfect here because the heavy lifting is done by the C++ engine processes
    with concurrent.futures.ThreadPoolExecutor(max_workers=max_threads) as executor:
        futures = []
        for i in range(num_games):
            alpha_is_white = (i % 2 == 0) # Alternate colors
            futures.append(executor.submit(play_single_game, i, sf_elo, alpha_is_white))

        # as_completed yields the results the exact millisecond a game finishes
        for future in concurrent.futures.as_completed(futures):
            winner = future.result()
            if winner.startswith("Error"):
                print(winner)
                continue
                
            results[winner] += 1
            completed = sum(results.values())
            print(f"[{completed}/{num_games}] Match concluded. Result: {winner}")

    print("\n" + "="*30)
    print("      FINAL BENCHMARK      ")
    print("="*30)
    print(f"Alpha Wins:     {results['Alpha']}")
    print(f"Stockfish Wins: {results['Stockfish']}")
    print(f"Draws:          {results['Draw']}")
    
    score = results['Alpha'] + (results['Draw'] * 0.5)
    win_rate = (score / num_games) * 100
    print(f"Alpha Score:    {win_rate:.1f}%")

if __name__ == "__main__":
    # Now you can easily run 100+ games in a fraction of the time
    run_parallel_tournament(num_games=200, sf_elo=1320)
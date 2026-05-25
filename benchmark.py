import chess
import chess.engine
import concurrent.futures
import os
import time

ALPHA_PATH = "./engine"
STOCKFISH_PATH = "stockfish"

def play_single_game(game_id, sf_elo, alpha_is_white):
    alpha = None
    sf = None
    start_time = time.time()
    
    try:
        alpha = chess.engine.SimpleEngine.popen_uci(ALPHA_PATH)
        sf = chess.engine.SimpleEngine.popen_uci(STOCKFISH_PATH)
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
        game_duration = time.time() - start_time
        
        winner = "Draw"
        if outcome.winner == chess.WHITE: 
            winner = "Alpha" if alpha_is_white else "Stockfish"
        elif outcome.winner == chess.BLACK: 
            winner = "Alpha" if not alpha_is_white else "Stockfish"
            
        return {
            "status": "ok", 
            "winner": winner, 
            "duration": game_duration
        }

    except Exception as e:
        return {"status": "error", "message": f"Game {game_id} Error: {str(e)}"}
    finally:
        # CRITICAL FIX: Guarantees processes die even if python-chess crashes
        if alpha: 
            alpha.quit()
        if sf: 
            sf.quit()

def run_parallel_tournament(num_games=100, sf_elo=1320):
    print(f"Starting Parallel Benchmark: Alpha vs Stockfish (Elo {sf_elo})")
    
    # Divide by 2 because each arena needs 1 Alpha + 1 Stockfish thread
    # This prevents the CPU oversubscription that causes TimeoutErrors
    max_threads = max(1, os.cpu_count() // 2)
    print(f"Spinning up {max_threads} safe parallel arenas...\n")

    results = {"Alpha": 0, "Stockfish": 0, "Draw": 0}
    total_time = 0.0

    with concurrent.futures.ThreadPoolExecutor(max_workers=max_threads) as executor:
        # Pass the game index to alternate colors evenly
        futures = [executor.submit(play_single_game, i, sf_elo, (i % 2 == 0)) for i in range(num_games)]

        for future in concurrent.futures.as_completed(futures):
            res = future.result()
            if res["status"] == "error":
                print(res["message"])
                continue
                
            winner = res["winner"]
            results[winner] += 1
            total_time += res["duration"]
            
            completed = sum(results.values())
            print(f"[{completed}/{num_games}] Result: {winner:<9} | Time: {res['duration']:5.1f}s")

    print("\n" + "="*30)
    print("      FINAL BENCHMARK      ")
    print("="*30)
    print(f"Alpha Wins:     {results['Alpha']}")
    print(f"Stockfish Wins: {results['Stockfish']}")
    print(f"Draws:          {results['Draw']}")
    
    win_rate = ((results['Alpha'] + (results['Draw'] * 0.5)) / num_games) * 100
    
    # Prevent division by zero if all games crashed
    valid_games = sum(results.values())
    avg_time = (total_time / valid_games) if valid_games > 0 else 0
    
    print(f"\nAlpha Score:    {win_rate:.1f}%")
    print(f"Avg Time/Game:  {avg_time:.1f}s")

if __name__ == "__main__":
    run_parallel_tournament(num_games=20, sf_elo=1320)
# Chess Engine Alpha

A C++20 bitboard chess engine with UCI support, plus a Python benchmarking harness to compare strength against Stockfish at target Elo levels.

## Prerequisites

- Linux/macOS shell environment
- `g++` with C++20 support
- `make`
- Python 3.10+
- A Stockfish binary available on your `PATH` as `stockfish` (or adjust `STOCKFISH_PATH` in `benchmark.py`)

## 1) Build the Engine

From repository root:

```bash
make
```

This produces an executable named `engine` in the repo root.

Clean build artifacts:

```bash
make clean
```

## 2) Run the Engine (UCI mode)

Launch:

```bash
./engine
```

You can manually verify UCI handshake:

```text
uci
isready
quit
```

Expected responses include `uciok` and `readyok`.

## 3) Python Environment for Benchmarking

Create and activate a venv, then install benchmark dependencies:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

## 4) Benchmark vs Stockfish

Default benchmark script (`benchmark.py`) runs many games in parallel and alternates colors.

Run:

```bash
python benchmark.py
```

Current defaults in the script:

- `num_games=200`
- `sf_elo=1320`
- `0.1s` per move for each side
- Maximum thread workers = `CPU cores - 1`

### Adjust Benchmark Settings

Edit the bottom call in `benchmark.py`:

```python
run_parallel_tournament(num_games=200, sf_elo=1320)
```

And/or adjust per-move time control in `play_single_game`:

```python
chess.engine.Limit(time=0.1)
```

## 5) Typical Workflow

1. `make`
2. `source .venv/bin/activate`
3. `python benchmark.py`
4. Compare win rate / score after each engine improvement

## Troubleshooting

- **`stockfish` not found**: install Stockfish or set `STOCKFISH_PATH` to an absolute path in `benchmark.py`.
- **`engine` not found by benchmark**: ensure `make` succeeded and that `ALPHA_PATH = "./engine"` remains correct.
- **Python import errors**: re-run `pip install -r requirements.txt` inside the active venv.

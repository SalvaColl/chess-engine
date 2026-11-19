# ♟️ CPEngine

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=for-the-badge&logo=c%2B%2B)
![Python](https://img.shields.io/badge/Python-3.x-blue.svg?style=for-the-badge&logo=python)
![License](https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Active-success.svg?style=for-the-badge)

**CPEngine** is a high-performance, UCI-compliant chess engine written in C++, paired with a custom graphical user interface built in Python (Pygame).

Developed with a focus on advanced search heuristics, memory-efficient pruning, and strategic bitboard evaluation, CPEngine is capable of defeating advanced human players and has achieved strong results in fast time-control testing against established engines (est. 2100+ Elo).

---

## ✨ Features


### The Engine (C++)

* **Protocol:** Supports core UCI commands for integration with chess GUIs.
* **Board Representation:** Bitboard-based chess representation and legal move generation using Disservin's chess-library.
* **Search Architecture:**
  * Negamax with Alpha-Beta Pruning.
  * Principal Variation Search (PVS).
  * Iterative Deepening with Aspiration Windows.
  * Time-limited search with periodic deadline checks.
  * Bounded Check Extensions and Quiescence Search with Check Evasions.
* **Transposition Table:**
  * Zobrist Hashing with exact, upper-bound and lower-bound entries.
  * Cached best moves and depth-aware replacement.
  * Mate-score normalization and Mate Distance Pruning.
* **Pruning & Reductions:**
  * Null Move Pruning (NMP).
  * Reverse Futility Pruning (RFP).
  * Late Move Reductions (LMR).
  * Late Move Pruning (LMP).
  * Fixed-margin Quiescence Pruning.
* **Move Ordering:**
  * Transposition Table Move Prioritization.
  * MVV-LVA Capture Ordering.
  * Two Killer Moves per Ply.
  * History Heuristic with depth-weighted updates.
  * Lazy Selection Sorting using fixed-size arrays at recursive nodes.
* **Evaluation:**
  * Material and Piece-Square Tables (PSTs).
  * Tapered Middlegame/Endgame Evaluation.
  * Passed, Isolated and Doubled Pawn Heuristics.
  * Open and Semi-Open File Bonuses for Rooks.
  * Bishop Pair Bonus.

### The GUI (Python)
* **Interactive Play:** Fully playable 2D graphical board using `pygame` and `python-chess`.
* **Multithreaded Engine Handling:** Asynchronous, non-blocking background threads ensure a smooth 60 FPS UI while the engine calculates.
* **Quality of Life Menus:** Interactive side-selection on startup, graphical Game Over / Restart overlays, and a visual Pawn Underpromotion menu.
* **Audio-Visuals:** Integrated sound effects, piece assets, and real-time engine metric displays (Eval, Depth, NPS, Best Move).
* **Process Safety:** Graceful subprocess cleanup prevents hidden zombie processes on exit.

---

## 🚀 Getting Started

### Prerequisites
* **C++ Compiler:** `g++` (MinGW-w64 on Windows) supporting C++17.
* **Python:** Python 3.8+

### 1. Clone the Repository
```bash
git clone https://github.com/SalvaColl/chess-engine
cd chess-engine
```

### 2. Build the Engine
Run the included batch script to compile the engine with `-O3` optimizations and static linking:

```cmd
.\build.bat
```

### 3. Run the GUI
Install the Python dependencies:

```bash
pip install -r requirements.txt
```

Launch the game:

```bash
python gui.py
```

---

## 🔧 Using with Other Chess GUIs

Because CPEngine implements standard UCI, you can load `engine.exe` directly into third-party GUIs:
* **Cutechess**
* **Arena Chess GUI**
* **LucasChess**

Point the GUI's engine configuration to `engine.exe`.

---

## 📊 Performance & Benchmarks

In internal testing via Cutechess (1 second/move time control), CPEngine achieved a performance rating of 2203 ELO against Stockfish.

---

## 📜 License
Distributed under the MIT License. See `LICENSE` for more information.
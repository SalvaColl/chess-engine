# ♟️ CPEngine

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=for-the-badge&logo=c%2B%2B)
![Python](https://img.shields.io/badge/Python-3.x-blue.svg?style=for-the-badge&logo=python)
![License](https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Active-success.svg?style=for-the-badge)

**CPEngine** is a high-performance, UCI-compliant chess engine written in C++, paired with a custom graphical user interface built in Python (Pygame).

Developed with a focus on advanced search heuristics and efficient pruning, CPEngine is capable of defeating advanced human players and has achieved strong results in fast time-control testing against established engines (est. 2100+ Elo).

---

## ✨ Features

### The Engine (C++)
* **Protocol:** Fully compatible with the Universal Chess Interface (UCI).
* **Board Representation:** Optimized move generation and bitboard-style logic.
* **Search Algorithm:** Alpha-Beta Negamax with Iterative Deepening.
* **Transposition Table:** Implemented via Zobrist Hashing to recall previously evaluated positions and detect repetitions.
* **Advanced Pruning & Reductions:**
  * Null Move Pruning (NMP)
  * Reverse Futility Pruning (RFP)
  * Late Move Reductions (LMR)
* **Move Ordering Heuristics:**
  * Hash / TT Move priority
  * MVV-LVA (Most Valuable Victim - Least Valuable Attacker)
  * Killer Move Heuristic (with ply-based resetting)
* **Tactical Deepening:** Quiescence Search with Selective Depth (`seldepth`) tracking and Check Extensions to avoid the horizon effect.

### The GUI (Python)
* **Interactive Play:** Fully playable 2D graphical board using `pygame` and `chess`.
* **Audio-Visuals:** Integrated sound effects and piece assets.
* **Engine Integration:** Uses `subprocess` with `CREATE_NO_WINDOW` on Windows to run the C++ engine invisibly in the background.

---

## 🚀 Getting Started

### Prerequisites
* **C++ Compiler:** `g++` (MinGW-w64 on Windows) supporting C++17.
* **Python:** Python 3.8+

### 1. Clone the Repository
```bash
git clone https://github.com/SalvaColl/chess-engine.git
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

In internal testing via Cutechess (1 second/move time control), CPEngine achieved a performance rating of 2138 ELO against Stockfish.

---

## 📁 Repository Structure

```text
├── engine.cpp       # Core UCI search and evaluation engine
├── chess.hpp        # Move generation and board state header
├── gui.py           # Pygame board interface
├── build.bat        # Windows GCC compilation script
├── requirements.txt # Python dependencies
├── Capture.ogg      # Capture sound
├── Move.ogg         # Move sound
└── make_sound.py    # Script used to generate the capture and move sounds
```

## 📜 License
Distributed under the MIT License. See `LICENSE` for more information.

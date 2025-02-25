# BBD Chess Engine 🎮 ♟️

<img src="https://img.shields.io/badge/version-1.0.1-blue" alt="Version 1.0.1">
<img src="https://img.shields.io/badge/C%2B%2B-20-brightgreen" alt="C++ 20">


BBD is a high-performance chess engine written in C++20. Don't ask what the name means. 😉

## ✨ Features

- 💡 **Efficient Bitboard Representation** - Representing the board as 64-bit integers for optimal performance
- 🧠 **Advanced Search Algorithms**:
  - Negamax with Alpha-Beta pruning
  - Principal Variation Search (PVS)
  - Quiescence Search
  - Null Move Pruning
  - Reverse Futility Pruning
  - Aspiration Windows
- 🔄 **Move Ordering Heuristics**:
  - Transposition Table move ordering
  - Killer Moves
  - History Heuristic
- 🔍 **Memory Optimizations**:
  - Transposition Table
  - Incremental Zobrist hash updating
- 🧪 **Position Evaluation**:
  - Neural Network (NNUE) evaluation
  - Efficiently Updatable Neural Network
- 🔁 **Drawing Detection**:
  - Three-fold repetition checking
- 🌐 **UCI Protocol Support** - Compatible with UCI chess GUIs

## 🚀 Getting Started

### Prerequisites

- CMake (version 3.15 or higher)
- Clang++ compiler (for optimal performance)

### Installation

```bash
# Clone the repository
git clone https://github.com/your-username/bbd.git
cd bbd

# Build the project
cmake -DCMAKE_CXX_COMPILER=clang++ -S . -B build/
cmake --build build/

# Run the engine
./build/bbd
```

## 📋 Usage

### UCI Mode

BBD supports the Universal Chess Interface (UCI) protocol, allowing it to be used with most chess GUIs.

```bash
./build/bbd
```

Then enter UCI commands:

```
uci
position startpos moves e2e4 c7c5
go depth 10
```

### Tournament Mode

BBD can also be used in tournament mode:

```bash
./build/bbd input_file current_position output_file
```

### Benchmarking

You can run a standard benchmark suite:

```bash
./build/bbd bench
```

## 📊 Development Progress

Our team has been continuously improving BBD with new features:

### Core Engine
- ✅ Efficient bitboard representation
- ✅ Legal move generation
- ✅ Special move handling (castling, en passant, promotions)
- ✅ Zobrist hashing with incremental updates
- ✅ Three-fold repetition detection
- ✅ UCI compatibility for chess GUIs

### Search Improvements
- ✅ Alpha-Beta pruning
- ✅ Quiescence search
- ✅ Principal Variation Search
- ✅ Transposition Tables
- ✅ Reverse Futility Pruning
- ✅ Null Move Pruning
- ✅ Aspiration Windows
- ✅ Killer Moves heuristic
- ✅ History Heuristic for move ordering
- ✅ TT move ordering
- 🔄 Late Move Reductions (in progress)

### Evaluation
- ✅ NNUE (Neural Network) evaluation
- ✅ Incrementally updated accumulators
- ✅ Pesto Piece Square Tables

### Code Quality
- ✅ Comprehensive test suite
- ✅ Automatic formatting with clang-format
- ✅ CI/CD integration

## 📜 Timeline

Development has been progressing steadily since December 2024:

- **Week 1-2**: Core board representation and basic operations
- **Week 3-4**: Move generation and UCI compatibility
- **Week 5**: Basic search algorithms and alpha-beta pruning
- **Week 6-7**: Advanced search techniques (PVS, TT, pruning methods)
- **December 2024 - January 2025**: Refinement of search algorithms and evaluation

## 👨‍💻 Contributors

- 🧑‍💻 **Luca-Mihnea Metehau**: Move generation, search algorithms, UCI compatibility, evaluation improvements
- 🧑‍💻 **Emeric Payer**: Square implementation, bitboard implementation, argument parser, PVS, transposition tables
- 🧑‍💻 **Georgy Salakhutdinov**: CMake setup, Zobrist hashing, castling rights management, board state tracking, 3-fold repetition detection
- 🧑‍💻 **Georgii Kuznetsov**: Board class basics, Formatting, null move pruning, time search, testing infrastructure (google tests), Neural Network evaluation

## 🧪 Testing

BBD includes a comprehensive test suite to ensure correctness:

```bash
# Build and run the tests
cmake --build build/ --target run-tests
```

## 🔧 Development Tools

- **Formatting**: 
  ```bash
  cmake --build build/ --target format-apply
  ```

- **Format Checking**:
  ```bash
  cmake --build build/ --target format-check
  ```

## 🙏 Acknowledgements

Special thanks to the chess programming community for their invaluable resources and algorithms that have inspired this engine.

---

*"Chess is the gymnasium of the mind." - Blaise Pascal*

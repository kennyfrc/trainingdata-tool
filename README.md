# trainingdata-tool
Tool to generate [lc0](https://github.com/LeelaChessZero/lc0) training data. Useful for [Supervised Learning](https://github.com/dkappe/leela-chess-weights/wiki/Supervised-Learning) from PGN games.

---

## Kenn's notes

Whether you plan to compile this for windows or mac, the guidance that I can give to you right now to make this work on your machine is to ensure that 1) you get the exact commits from the respective lc0, polyglot, and zlib folders by doing `git clone <folder>` and then `git checkout <commit hash>` and 2) you need to update the header files in the `src` folder as it draws from the polyglot folder. There are also some proto issues that can be resolved by ensuring that you have `lc0/subprojects/protobuf*` in your local machine. 

In order to compile this, you need:
1. Cmake
2. Make
3. Exact commits in zlib, polyglot, lc0 in Daniel's original file (or just use mine)

Example:
```
cmake CMakeLists.txt
make
// as you go through make, debug and fix dependencies as you go
```

You should expect a `trainingdata-tool` binary at the end of it.

---

## Usage
Pass the PGN input file and it will output training data in the same way lc0 selfplay does. Example:
```
trainingdata-tool Kasparov.pgn
```

There are 4 options suported so far:
 - `-v`: Verbose mode
 - `-lichess-mode`: Lichess mode. Will extract SF evaluation score from Lichess commented games. Non-commented games will be filtered out.
 - `-files-per-dir <integer number>`: Max games to store in a single directory, when that number is reached a new directory is created to store the new games to avoid stressing the file system too much.
 - `-max-files-to-convert <integer number>`: Stop after this many files have been written.
 - `-chunks-per-file`: How many training data chunks to write in each file.

 Example:
 ```
 trainingdata-tool -max-games-to-convert 1000 -files-per-dir 500 -v -lichess-mode
Max games to convert set to: 1000
Max files per directory set to: 500
Verbose mode ON
Lichess mode ON
 ```

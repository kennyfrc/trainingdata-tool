# trainingdata-tool
Tool to generate [lc0](https://github.com/LeelaChessZero/lc0) training data. Useful for [Supervised Learning](https://github.com/dkappe/leela-chess-weights/wiki/Supervised-Learning) from PGN games.

In order to maximize this tool, it's best to add Q and Policy data from Stockfish or Leela using [Lichess Artist](https://github.com/kennyfrc/lichess-artist) or my [UCI analyzer](https://github.com/kennyfrc/uci-analyzer-experimental). Both allow you to generate lichess-style annotations which `trainingdata-tool` can accept as inputs. UCI Analyzer writes pgns faster.

---

## Compilation Guide (Linux)

In order to compile this, you need:
1. Cmake (`sudo apt-get install cmake`)
2. Boost for Ubuntu (`sudo apt-get install libboost-all-dev`)
3. Clone the repo commits (see the hashes) in zlib, polyglot, lc0 in Daniel's original file (or just use mine)
4. Make

In the original source file, there were a lot of dependency issues in `src` that this repo has been resolved.

Example:
```
sudo apt-get install cmake
sudo apt-get install libboost-all-dev
git clone https://github.com/DanielUranga/lc0.git
cd lc0
git checkout 015583a28b
cd ..
git clone https://github.com/DanielUranga/polyglot.git
cd polyglot
git checkout 830fa94674
cd ..
git clone https://github.com/madler/zlib.git
cd zlib
git checkout cacf7f1d4e
cd ..
cmake CMakeLists.txt
make
```

## Compilation Guide (Mac)

For mac, the process is similar with the exception of boost.

## Q Ratio Flag

Also note that there is an undocumented flag called `-dedup-q-ratio` which allows you to set the q-ratio.

Example:
```
trainingdata-tool Kasparov.pgn -max-games-to-convert 1000 -files-per-dir 500 -v -lichess-mode -dedup-q-ratio 0.2
```

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

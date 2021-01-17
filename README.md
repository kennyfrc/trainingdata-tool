# trainingdata-tool
Tool to generate [lc0](https://github.com/LeelaChessZero/lc0) training data. Useful for [Supervised Learning](https://github.com/dkappe/leela-chess-weights/wiki/Supervised-Learning) from PGN games.

## Compilation Instructions

### For Linux

```
./build-linux.sh
```

### For Mac

```
./build-mac.sh
```

### For Windows

You may use this [appveyor artifact](https://ci.appveyor.com/project/kennyfrc/trainingdata-tool/build/artifacts) instead: 

## Changes compared to the Original

* Generate policy data using MultiPV and softmax
* Set D values using [this data](https://en.chessbase.com/post/has-the-number-of-draws-in-chess-increased)
* Updated the centipawn to Q formula. See [here](https://github.com/LeelaChessZero/lc0/pull/841) for reference.


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
 - `-chunks-per-file`: How many training data chunks (positions) to write in each file (game). There are, on average, 135 positions per game so I suggest setting 135.

 Example:
 ```
 trainingdata-tool -max-games-to-convert 1000000 -files-per-dir 100000 -chunks-per-file 135 -v -lichess-mode
Max games to convert set to: 1000000
Max files per directory set to: 100000
Verbose mode ON
Lichess mode ON
 ```

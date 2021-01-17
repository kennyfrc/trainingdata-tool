# trainingdata-tool

Tool to generate [lc0](https://github.com/LeelaChessZero/lc0) training data. Useful for [Supervised Learning](https://github.com/dkappe/leela-chess-weights/wiki/Supervised-Learning) from PGN games.

## Compilation Instructions

### For Linux

Run `./build-linux.sh`

### For Mac

Run `./build-mac.sh`

## Releases

For Windows, you may use this [appveyor artifact](https://ci.appveyor.com/project/kennyfrc/trainingdata-tool/build/artifacts).

For Ubuntu (20.04), sign up to CircleCI. Once logged in, you will find an artifact in this [project folder](https://app.circleci.com/pipelines/github/kennyfrc/trainingdata-tool).

## Changes compared to the Original

* Generate policy data from move variations and softmax
* Set D values using [this data](https://en.chessbase.com/post/has-the-number-of-draws-in-chess-increased)
* Updated the centipawn to Q formula. See [here](https://github.com/LeelaChessZero/lc0/pull/841) for reference.

## How do I annotate a pgn with Q and policy data?

To annotate a pgn with Q and policy, you can add evaluations from any chess engine using [pgn-extract](https://www.cs.kent.ac.uk/people/staff/djb/pgn-extract) and [uci-analyser](https://github.com/kennyfrc/uci-analyzer-experimental). 

With uci-analyser, you can generate evals for both the mainline and variations. `trainingdata-tool` then takes the mainline evaluation and converts it into Q. Then it transforms evals from the mainline and variations into a policy distribution using softmax.

However, uci-analyser is only compatible with stockfish. So if you want to distill other lc0 weights and other AB engines, you will likely have to modify `analyse.cpp` in uci-analyser to make it compatible. An lc0 compatible pgn writer is [lichess-artist](https://github.com/kennyfrc/lichess-artist) but it's an order of magnitude slower than that of uci-analyser.


## Usage - Z data only

Pass the PGN input file and it will output training data in the same way lc0 selfplay does. Example:
```
trainingdata-tool Kasparov.pgn -max-games-to-convert 1000000 -files-per-dir 100000 -chunks-per-file 135
```

## Usage - Z, Q, and Policy data

To train with your generated Q and/or policy data, add the `-lichess-mode` flag. You'll need to use a pgn writer that uses the lichess format.

There are 4 options that allow this:
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

## Usage - Other options

There are 4 options:
 - `-v`: Verbose mode
 - `-lichess-mode`: Lichess mode. Will extract SF evaluation score from Lichess commented games. Non-commented games will be filtered out.
 - `-files-per-dir <integer number>`: Max games to store in a single directory, when that number is reached a new directory is created to store the new games to avoid stressing the file system too much.
 - `-max-files-to-convert <integer number>`: Stop after this many files have been written.
 - `-chunks-per-file`: How many training data chunks (positions) to write in each file (game). There are, on average, 135 positions per game so I suggest setting 135.


## Possible Future Updates

* Update from v4 training data to v5 (MLH head).
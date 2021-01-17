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

Below is an example of a pgn (clipped after the 5th move) that uses the lichess format.

```
[Event "?"]
[Site "?"]
[Date "2019.09.24"]
[Round "1"]
[White "sf10_1"]
[Black "sf10_2"]
[Result "1-0"]
[ECO "A05"]
[Opening "Reti Opening"]
[TimeControl "inf"]
[PlyCount "129"]
[GameDuration "00:00:03"]
[GameEndTime "2019-09-24T13:09:53.478 CDT"]
[GameStartTime "2019-09-24T13:09:49.780 CDT"]

1. Nf3 { [%eval 0.34] } (1. e4 { [%eval 0.34] }) (1. Nc3 { [%eval 0.17] }) (1. d4 { [%eval 0.17] }) (1. e3 { [%eval 0.09] }) (1. a3 { [%eval -0.18] }) 1... Nf6 { [%eval -0.54] } (1... e5 { [%eval -0.54] }) (1... d5 { [%eval -0.67] }) (1... Nc6 { [%eval -0.67] }) (1... e6 { [%eval -0.90] }) (1... a6 { [%eval -1.00] }) 2. c4 { [%eval 0.78] } (2. e4 { [%eval 0.78] }) (2. d4 { [%eval 0.32] }) (2. Nc3 { [%eval 0.12] }) (2. g4 { [%eval 0.02] }) (2. a4 { [%eval -0.55] }) 2... g6 { [%eval 0.37] } (2... e5 { [%eval 0.37] }) (2... Nc6 { [%eval -0.01] }) (2... d5 { [%eval -0.15] }) (2... e6 { [%eval -0.18] }) (2... d6 { [%eval -0.41] }) (2... h6 { [%eval -0.48] }) 3. Nc3 { [%eval 0.70] } (3. e4 { [%eval 0.70] }) (3. d4 { [%eval 0.32] }) (3. a3 { [%eval 0.09] }) (3. d3 { [%eval 0.05] }) (3. g3 { [%eval -0.01] }) 3... d5 { [%eval -0.63] } (3... Bg7 { [%eval -0.63] }) (3... d6 { [%eval -0.63] }) (3... e5 { [%eval -0.69] }) (3... c5 { [%eval -1.01] }) (3... c6 { [%eval -1.30] }) 4. Qa4+ { [%eval 0.88] } (4. cxd5 { [%eval 0.88] }) (4. d4 { [%eval 0.59] }) (4. e3 { [%eval 0.29] }) (4. Nxd5 { [%eval -0.03] }) (4. d3 { [%eval -0.27] }) (4. c5 { [%eval -0.43] }) 4... c6 { [%eval 0.35] } (4... Bd7 { [%eval 0.35] }) (4... Qd7 { [%eval -0.48] }) (4... Nc6 { [%eval -0.60] }) (4... b5 { [%eval -0.82] }) (4... Nbd7 { [%eval -1.25] }) 5. cxd5 { [%eval 0.48] } (5. Nxd5 { [%eval -0.09] }) (5. d4 { [%eval -0.12] }) (5. d3 { [%eval -0.29] }) (5. c5 { [%eval -0.63] }) (5. e3 { [%eval -0.67] }) 5... b5 { [%eval -0.32] } (5... Bg7 { [%eval -0.60] }) (5... Bd7 { [%eval -0.74] }) (5... a6 { [%eval -0.88] }) (5... Nxd5 { [%eval -1.01] }) (5... Qb6 { [%eval -1.03] })
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
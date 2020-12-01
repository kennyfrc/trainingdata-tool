#if !defined(PGN_GAME_H_INCLUDED)
#define PGN_GAME_H_INCLUDED

#include "../lc0/src/neural/encoder.h"
#include "../lc0/src/neural/network.h"
#include "../lc0/src/neural/writer.h"
#include "../polyglot/src/pgn.h"
#include "polyglot_lib.h"
#include "PGNMoveInfo.h"

class PGNMoveInfo;

struct Options {
  bool verbose = false;
  bool lichess_mode = false;
};

struct PGNGame {
  char result[PGN_STRING_SIZE];
  char fen[PGN_STRING_SIZE];
  std::vector<PGNMoveInfo> moves;
  std::vector<PGNMoveInfo> variations;

  explicit PGNGame(pgn_t* pgn);
  std::vector<lczero::V4TrainingData> getChunks(Options options) const;
};

#endif

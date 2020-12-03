#if !defined(PGN_MOVE_INFO_H_INCLUDED)
#define PGN_MOVE_INFO_H_INCLUDED

#include "../polyglot/src/pgn.h"
// #include "../lc0/src/neural/encoder.h"
// #include "../lc0/src/neural/network.h"
// #include "../lc0/src/neural/writer.h"

struct PGNMoveInfo {
  char move[PGN_STRING_SIZE];
  char comment[PGN_STRING_SIZE];
  char nag[PGN_STRING_SIZE];
  int move_no;
  float q;

  explicit PGNMoveInfo(char* move, char* comment, char* nag, int move_no = 0, float q = 0.0f);
};

#endif

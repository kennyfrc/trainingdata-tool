#include "PGNMoveInfo.h"

#include <cstring>

PGNMoveInfo::PGNMoveInfo(char* move, char* comment, char* nag, int move_no, float q) {
  strncpy(this->move, move, PGN_STRING_SIZE);
  strncpy(this->comment, comment, PGN_STRING_SIZE);
  strncpy(this->nag, nag, PGN_STRING_SIZE);
  this->move_no = move_no;
  this->q = q;
  // this->lc0_move = lc0_move;
}

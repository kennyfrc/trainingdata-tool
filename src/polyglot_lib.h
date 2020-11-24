// From https://github.com/rozim/ChessData

#if !defined(POLYGLOT_LIB_H_INCLUDED)
#define POLYGLOT_LIB_H_INCLUDED

#include "../polyglot/src/attack.h"
#include "../polyglot/src/board.h"
#include "../polyglot/src/colour.h"
// #include "../polyglot/src/epd.h"
#include "../polyglot/src/fen.h"
#include "../polyglot/src/game.h"
#include "../polyglot/src/move.h"
#include "../polyglot/src/move_do.h"
#include "../polyglot/src/move_gen.h"
#include "../polyglot/src/move_legal.h"
#include "../polyglot/src/pgn.h"
#include "../polyglot/src/piece.h"
#include "../polyglot/src/san.h"
#include "../polyglot/src/search.h_"
#include "../polyglot/src/square.h"

// dss: init everything
extern void polyglot_init();
extern void polyglot_quit();

#endif
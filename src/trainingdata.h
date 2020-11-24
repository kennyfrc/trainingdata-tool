#if !defined(TRAININGDATA_H_INCLUDED)
#define TRAININGDATA_H_INCLUDED

#include "../lc0/src/neural/encoder.h"
#include "../lc0/src/neural/network.h"
#include "../lc0/src/neural/writer.h"

lczero::V4TrainingData get_v4_training_data(
        lczero::GameResult game_result, const lczero::PositionHistory& history,
        lczero::Move played_move, lczero::MoveList legal_moves, float Q);

#endif

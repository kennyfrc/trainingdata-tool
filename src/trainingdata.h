#if !defined(TRAININGDATA_H_INCLUDED)
#define TRAININGDATA_H_INCLUDED

#include "../lc0/src/neural/encoder.h"
#include "../lc0/src/neural/network.h"
#include "../lc0/src/neural/writer.h"
#include "../polyglot/src/board.h"
#include "../polyglot/src/move.h"
#include "PGNMoveInfo.h"

lczero::V4TrainingData get_v4_training_data(
        lczero::GameResult game_result, const lczero::PositionHistory& history,
        lczero::Move played_move, lczero::MoveList legal_moves, float Q, std::vector<PGNMoveInfo> variations, int move_no, board_t *board_ptr);

struct MovePolicy {
    lczero::Move played;
    float q_value;
    float d_value;
    float policy_weight;

    // explicit MovePolicy(lczero::Move lc0_move, float q_value = 0.0f, float policy_weight = 0.0f);
};

#endif

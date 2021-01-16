#include "PGNGame.h"
#include "trainingdata.h"
#include "dbg.h"

#include <cstring>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <cmath>

uint64_t resever_bits_in_bytes(uint64_t v) {
  v = ((v >> 1) & 0x5555555555555555ull) | ((v & 0x5555555555555555ull) << 1);
  v = ((v >> 2) & 0x3333333333333333ull) | ((v & 0x3333333333333333ull) << 2);
  v = ((v >> 4) & 0x0F0F0F0F0F0F0F0Full) | ((v & 0x0F0F0F0F0F0F0F0Full) << 4);
  return v;
}


// softmax allows us to transform the data from [0,1]
// Q values are adjusted based on side to move
std::vector<MovePolicy> transform_with_softmax(std::vector<MovePolicy> move_policies, lczero::Position position) {
  float k_sum = 0;
  std::vector<MovePolicy> final_move_policies;
  for(auto move : move_policies) {
    if(position.IsBlackToMove()) {
      k_sum += exp(-1*move.q_value);
    } else {
      k_sum += exp(move.q_value);
    }
  }

  // d score was also snuck in here
  // this is more of a heuristic than a "scientific" decision
  // black has higher draw probability than white

  for(auto move : move_policies) {
    MovePolicy updated_move;
    if(position.IsBlackToMove()) {
      updated_move.played = move.played;
      updated_move.q_value = -1*move.q_value;
      updated_move.d_value = (-1*move.q_value) > 0.0f ? (1.0f-(-1*move.q_value))*0.50f : (1.0f-(move.q_value))*0.50f;
      updated_move.policy_weight = exp(-1*move.q_value) / k_sum;
      final_move_policies.emplace_back(updated_move);
      assert(updated_move.d_value >= 0 && updated_move.d_value <= 1);
      assert(updated_move.q_value >= -1 && updated_move.q_value <= 1);
    } else {
      updated_move.played = move.played;
      updated_move.q_value = move.q_value;
      updated_move.d_value = move.q_value > 0.0f ? (1.0f-(move.q_value))*0.25f : (1.0f-(-1*move.q_value))*0.25f;
      updated_move.policy_weight = exp(move.q_value) / k_sum;
      final_move_policies.emplace_back(updated_move);
      assert(updated_move.d_value >= 0 && updated_move.d_value <= 1);
      assert(updated_move.q_value >= -1 && updated_move.q_value <= 1);
    }
  }

  float policy_sum;
  for (auto move : final_move_policies) {
    policy_sum += move.policy_weight;
  }

  // check if transformed successfully
  // assert_eq(policy_sum, 1.0f);
  std::cout << "policy_sum: " << policy_sum << std::endl;
  // exit(0);

  return final_move_policies;
}



lczero::V4TrainingData get_v4_training_data(
        lczero::GameResult game_result, const lczero::PositionHistory& history,
        lczero::Move played_move, lczero::MoveList legal_moves, float Q, std::vector<PGNMoveInfo> variations, int move_no, board_t *board_ptr) {
  lczero::V4TrainingData result;

  // Set version.
  result.version = 4;

  // Illegal moves will have "-1" probability
  for (auto& probability : result.probabilities) {
    probability = -1;
  }

  // Populate legal moves with probability "0"
  for (lczero::Move move : legal_moves) {
    result.probabilities[move.as_nn_index()] = 0;
  }

  // Set policy vector
  std::vector<MovePolicy> move_policies;

  // Set main move's move, Q, and D
  MovePolicy main_move;
  main_move.played = played_move;
  main_move.q_value = Q;
  main_move.policy_weight = 0.0f;

  if(history.Last().IsBlackToMove()) {
    main_move.d_value = (-1*Q) > 0.0f ? (1.0f-(-1*Q))*0.50f : (1.0f-(Q))*0.50f;
  } else {
    main_move.d_value = Q > 0.0f ? (1.0f-(Q))*0.25f : (1.0f-(-1*Q))*0.25f;
  }

  assert(main_move.d_value >= 0 && main_move.d_value <= 1);
  assert(main_move.q_value >= -1 && main_move.q_value <= 1);

  move_policies.emplace_back(main_move);

  // Set variation's move, Q, and D
  for (auto variation : variations) {
    if(move_no == variation.move_no) {
      float engine_score;
      float engine_probability;
      MovePolicy mp_variation;

      extract_lichess_comment_score(variation.comment, engine_score);

      engine_probability = convert_sf_score_to_win_probability(engine_score);

      int variation_id = move_from_san(variation.move, board_ptr);
      lczero::Move lc0_variation = poly_move_to_lc0_move(variation_id, board_ptr);

      bool found = false;
      auto legal_moves = history.Last().GetBoard().GenerateLegalMoves();
      for (auto legal : legal_moves) {
        if (legal == lc0_variation && legal.castling() == lc0_variation.castling()) {
          found = true;
          break;
        }
      }
      if (!found) {
        std::cout << "Variation not found: " << variation.move << " "
                  << square_file(move_to(variation_id)) << std::endl;
      }

      mp_variation.played = lc0_variation;
      mp_variation.q_value = engine_probability;

      // add variation's lc0 move and q into policy vector
      move_policies.emplace_back(mp_variation);
    }
  }

  // Have a place where all policy weights are assigned per move
  std::vector<MovePolicy> updated_move_policies = transform_with_softmax(move_policies, history.Last());

  // Assign probabilities for the main move and variations
  for (auto move : updated_move_policies) {
    result.probabilities[move.played.as_nn_index()] = move.policy_weight;
  }

  // Populate planes.
  lczero::InputPlanes planes =
          EncodePositionForNN(history, 8, lczero::FillEmptyHistory::FEN_ONLY);
  int plane_idx = 0;
  for (auto& plane : result.planes) {
    plane = resever_bits_in_bytes(planes[plane_idx++].mask);
  }

  const auto& position = history.Last();
  // Populate castlings.
  result.castling_us_ooo =
          position.CanCastle(lczero::Position::WE_CAN_OOO) ? 1 : 0;
  result.castling_us_oo =
          position.CanCastle(lczero::Position::WE_CAN_OO) ? 1 : 0;
  result.castling_them_ooo =
          position.CanCastle(lczero::Position::THEY_CAN_OOO) ? 1 : 0;
  result.castling_them_oo =
          position.CanCastle(lczero::Position::THEY_CAN_OO) ? 1 : 0;

  // Other params.
  result.side_to_move = position.IsBlackToMove() ? 1 : 0;
  result.move_count = 0;
  result.rule50_count = position.GetNoCaptureNoPawnPly();

  // Game result.
  if (game_result == lczero::GameResult::WHITE_WON) {
    result.result = position.IsBlackToMove() ? -1 : 1;
  } else if (game_result == lczero::GameResult::BLACK_WON) {
    result.result = position.IsBlackToMove() ? 1 : -1;
  } else {
    result.result = 0;
  }

  std::vector<float> q_values;
  std::vector<float> d_values;

  // Q for Q+Z training
  // D values for WDL head
  for (auto move : updated_move_policies) {
    q_values.emplace_back(move.q_value);
    d_values.emplace_back(move.d_value);   
  }

  if(position.IsBlackToMove()) {
    float q_min, d_max;
    q_min = *min_element(q_values.begin(), q_values.end());
    d_max = *max_element(d_values.begin(), d_values.end());
    result.best_q = q_min != 0.0f ? q_min : 0.0f;
    result.root_q = Q != 0.0f ? (-1*Q) : 0.0f;
    result.best_d = d_max;
    result.root_d = main_move.d_value;
    // for debugging
    // std::cout << " |-- Best q value: " << result.best_q << std::endl;
    // std::cout << " |-- Root q value: " << result.root_q << std::endl;
    // std::cout << " |-- Best d value: " << result.best_d << std::endl;
    // std::cout << " |-- Root d value: " << result.root_d << std::endl;
  } else {
    float q_max, d_max;
    q_max = *max_element(q_values.begin(), q_values.end());
    d_max = *max_element(d_values.begin(), d_values.end());
    result.best_q = q_max != 0.0f ? q_max : 0.0f;
    result.root_q = Q != 0.0f ? Q : 0.0f;
    result.best_d = d_max;
    result.root_d = main_move.d_value;
    // for debugging
    // std::cout << " |-- Best q value: " << result.best_q << std::endl;
    // std::cout << " |-- Root q value: " << result.root_q << std::endl;
    // std::cout << " |-- Best d value: " << result.best_d << std::endl;
    // std::cout << " |-- Root d value: " << result.root_d << std::endl;
  }    

  return result;
}

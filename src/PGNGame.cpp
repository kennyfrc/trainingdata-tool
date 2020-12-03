#include "PGNGame.h"
#include "trainingdata.h"

#include <cmath>
#include <cstring>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

float convert_sf_score_to_win_probability(float score) {
  return 2 / (1 + exp(-0.4 * score)) - 1;
}

bool extract_lichess_comment_score(const char* comment, float& Q) {
  std::string s(comment);
  static std::regex rgx("\\[%eval (-?\\d+(\\.\\d+)?)\\]");
  static std::regex rgx2("\\[%eval #(-?\\d+)\\]");
  std::smatch matches;
  if (std::regex_search(s, matches, rgx)) {
    Q = std::stof(matches[1].str());
    return true;
  } else if (std::regex_search(s, matches, rgx2)) {
    Q = matches[1].str().at(0) == '-' ? -128.0f : 128.0f;
    return true;
  }
  return false;
}

lczero::Move poly_move_to_lc0_move(move_t move, board_t* board) {
  lczero::BoardSquare from(square_rank(move_from(move)),
                           square_file(move_from(move)));
  lczero::BoardSquare to(square_rank(move_to(move)),
                         square_file(move_to(move)));
  lczero::Move m(from, to);

  if (move_is_promote(move)) {
    lczero::Move::Promotion lookup[5] = {
        lczero::Move::Promotion::None,   lczero::Move::Promotion::Knight,
        lczero::Move::Promotion::Bishop, lczero::Move::Promotion::Rook,
        lczero::Move::Promotion::Queen,
    };
    auto prom = lookup[move >> 12];
    m.SetPromotion(prom);
  } else if (move_is_castle(move, board)) {
    bool is_short_castle =
        square_file(move_from(move)) < square_file(move_to(move));
    int file_to = is_short_castle ? 6 : 2;
    m.SetTo(lczero::BoardSquare(square_rank(move_to(move)), file_to));
    m.SetCastling();
  }

  if (colour_is_black(board->turn)) {
    m.Mirror();
  }

  return m;
}

PGNGame::PGNGame(pgn_t* pgn) {
  strncpy(this->result, pgn->result, PGN_STRING_SIZE);
  strncpy(this->fen, pgn->fen, PGN_STRING_SIZE);

  char str[256];
  int move_no = 0;
  while (pgn_next_move(pgn, str, 256)) {
    if(pgn->variation == false) {
      move_no += 1;
      this->moves.emplace_back(str, pgn->last_read_comment, pgn->last_read_nag, move_no);
      // std::cout << "Move number: " << move_no << std::endl;
      // std::cout << "Move: " << str << std::endl;
    } else {
      this->variations.emplace_back(str, pgn->last_read_comment, pgn->last_read_nag, move_no);
      // std::cout << "Within Move number: " << move_no << std::endl;
      // std::cout << "Variation: " << str << std::endl;
    }
  }
}

std::vector<lczero::V4TrainingData> PGNGame::getChunks(Options options) const {
  std::vector<lczero::V4TrainingData> chunks;
  lczero::ChessBoard starting_board;
  std::string starting_fen =
      std::strlen(this->fen) > 0 ? this->fen : lczero::ChessBoard::kStartposFen;

  {
    std::istringstream fen_str(starting_fen);
    std::string board;
    std::string who_to_move;
    std::string castlings;
    std::string en_passant;
    fen_str >> board >> who_to_move >> castlings >> en_passant;
    if (fen_str.eof()) {
      starting_fen.append(" 0 0");
    }
  }

  if (options.verbose) {
    std::cout << "Started new game, starting FEN: \'" << starting_fen << "\'"
              << std::endl;
  }

  starting_board.SetFromFen(starting_fen, nullptr, nullptr);

  lczero::PositionHistory position_history;
  position_history.Reset(starting_board, 0, 0);

  board_t board[1];

  board_from_fen(board, starting_fen.c_str());

  lczero::GameResult game_result;
  if (options.verbose) {
    std::cout << "Game result: " << this->result << std::endl;
  }
  if (my_string_equal(this->result, "1-0")) {
    game_result = lczero::GameResult::WHITE_WON;
  } else if (my_string_equal(this->result, "0-1")) {
    game_result = lczero::GameResult::BLACK_WON;
  } else {
    game_result = lczero::GameResult::DRAW;
  }

  char str[256];
  std::vector<PGNMoveInfo> variations;
  for (auto pgn_move : this->moves) {
    // Extract move from pgn
    int move = move_from_san(pgn_move.move, board);
    if (move == MoveNone || !move_is_legal(move, board)) {
      std::cout << "illegal move: " << pgn_move.move << std::endl;
      break;
    }

    // needs refactoring - ugly regex
    if (options.verbose) {
      move_to_san(move, board, str, 256);
      std::cout << "\nRead move: " << str << std::endl;
      if (pgn_move.comment[0]) {
        // regex
        const std::string comment = pgn_move.comment;
        const std::regex no_brackets("[}]");
        std::stringstream result;
        std::regex_replace(std::ostream_iterator<char>(result), comment.begin(), comment.end(), no_brackets, "");

        // comment
        std::cout << str << " main line comment: " << result.str() << std::endl;

        for (auto variation : this->variations) {
          if(pgn_move.move_no == variation.move_no) {
            // regex
            const std::string comment = variation.comment;
            const std::regex no_brackets("[}]");
            std::stringstream result;
            std::regex_replace(std::ostream_iterator<char>(result), comment.begin(), comment.end(), no_brackets, "");

            // comment
            std::cout << " |-- Read variation: " << variation.move << std::endl;
            std::cout << "    |-- " <<variation.move << " variation comment: " << result.str() << std::endl;
          }
        }
      }
    }

    bool bad_move = false;
    if (pgn_move.nag[0]) {
      // If the move is bad or dubious, skip it.
      // See https://en.wikipedia.org/wiki/Numeric_Annotation_Glyphs for PGN
      // NAGs
      if (pgn_move.nag[0] == '2' || pgn_move.nag[0] == '4' ||
          pgn_move.nag[0] == '5' || pgn_move.nag[0] == '6') {
        bad_move = true;
      }
    }

    // Convert move to lc0 format
    lczero::Move lc0_move = poly_move_to_lc0_move(move, board);

    // std::cout << "This is how the board looks like [mid]: " << board << std::endl;

    bool found = false;
    auto legal_moves = position_history.Last().GetBoard().GenerateLegalMoves();
    for (auto legal : legal_moves) {
      if (legal == lc0_move && legal.castling() == lc0_move.castling()) {
        found = true;
        break;
      }
    }
    if (!found) {
      std::cout << "Move not found: " << pgn_move.move << " "
                << square_file(move_to(move)) << std::endl;
    }

    // Extract engine scores and convert to win probability
    // Add variation scores too
    if (options.lichess_mode) {
      // Extract mainline score
      if (pgn_move.comment[0]) {
        float lichess_score;
        bool success =
            extract_lichess_comment_score(pgn_move.comment, lichess_score);
        if (!success) {
          break;  // Comment contained no "%eval"
        }
        pgn_move.q = convert_sf_score_to_win_probability(lichess_score);
      } else {
        // This game has no comments, skip it.
        break;
      }

      // Extract variation score

      for (auto variation : this->variations) {
        if (variation.move_no == pgn_move.move_no && variation.comment[0]) {
          float variation_score;
          bool success =
              extract_lichess_comment_score(variation.comment, variation_score);
          if (!success) {
            break; // Variation contained no eval
          }
          variation.q = convert_sf_score_to_win_probability(variation_score);
          variations.emplace_back(variation.move, variation.comment, variation.nag, variation.move_no, variation.q);
          // std::cout << "Main Q: " << std::setprecision(5) << variation.q << std::endl;
        }
      }
    }

    if (!(bad_move && options.lichess_mode)) {
      // Generate training data
      lczero::V4TrainingData chunk = get_v4_training_data(
          game_result, position_history, lc0_move, legal_moves, pgn_move.q, variations, pgn_move.move_no, board);
      chunks.push_back(chunk);
      if (options.verbose) {
        std::string result;
        switch (game_result) {
          case lczero::GameResult::WHITE_WON:
            result = "1-0";
            break;
          case lczero::GameResult::BLACK_WON:
            result = "0-1";
            break;
          case lczero::GameResult::DRAW:
            result = "1/2-1/2";
            break;
          default:
            result = "???";
            break;
        }
        // std::cout << "legal moves\n";
        // std::cout << legal_moves[0].as_string() << "\n";
        std::cout << "Write chunk: [" << lc0_move.as_string() << ", " << result
                  << ", " << pgn_move.q <<"]\n";
      }
    }

    // Execute move
    position_history.Append(lc0_move);
    move_do(board, move);
  }

  if (options.verbose) {
    std::cout << "Game end." << std::endl;
  }

  return chunks;
}

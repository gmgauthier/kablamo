/* SPDX-License-Identifier: Unlicense */

#pragma once

namespace kablamo {

inline constexpr int kW = 9;
inline constexpr int kH = 9;
inline constexpr int kMines = 10;

enum class Phase { ready, playing, won, lost };

struct Cell {
  bool mine = false;
  bool flagged = false;
  bool revealed = false;
  int adj = 0;
};

class Board {
 public:
  Board();

  void reset();
  bool reveal(int r, int c);
  void toggle_flag(int r, int c);
  int remaining() const;
  Phase phase() const
  {
    return phase_;
  }
  const Cell& at(int r, int c) const;
  int boom_r() const
  {
    return boom_r_;
  }
  int boom_c() const
  {
    return boom_c_;
  }

 private:
  void ensure_mines(int safe_r, int safe_c);
  void flood(int r, int c);
  void count_adj();
  bool in_bounds(int r, int c) const;
  void check_win();
  void reveal_mines();

  Cell cells_[kH][kW]{};
  Phase phase_ = Phase::ready;
  bool mines_placed_ = false;
  int flags_ = 0;
  int boom_r_ = -1;
  int boom_c_ = -1;
};

}  // namespace kablamo

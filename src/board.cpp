/* SPDX-License-Identifier: Unlicense */

#include "board.hpp"

#include <algorithm>
#include <random>
#include <vector>

namespace kablamo {

Board::Board()
{
  reset();
}

void Board::reset()
{
  for (int r = 0; r < kH; ++r)
    for (int c = 0; c < kW; ++c)
      cells_[r][c] = Cell{};
  phase_ = Phase::ready;
  mines_placed_ = false;
  flags_ = 0;
  boom_r_ = boom_c_ = -1;
}

bool Board::in_bounds(int r, int c) const
{
  return r >= 0 && r < kH && c >= 0 && c < kW;
}

const Cell& Board::at(int r, int c) const
{
  return cells_[r][c];
}

int Board::remaining() const
{
  return kMines - flags_;
}

void Board::count_adj()
{
  for (int r = 0; r < kH; ++r) {
    for (int c = 0; c < kW; ++c) {
      int n = 0;
      for (int dr = -1; dr <= 1; ++dr)
        for (int dc = -1; dc <= 1; ++dc) {
          if (dr == 0 && dc == 0)
            continue;
          const int rr = r + dr;
          const int cc = c + dc;
          if (in_bounds(rr, cc) && cells_[rr][cc].mine)
            ++n;
        }
      cells_[r][c].adj = n;
    }
  }
}

void Board::ensure_mines(int safe_r, int safe_c)
{
  if (mines_placed_)
    return;
  std::vector<int> slots;
  slots.reserve(kW * kH - 1);
  for (int i = 0; i < kW * kH; ++i) {
    if (i == safe_r * kW + safe_c)
      continue;
    slots.push_back(i);
  }
  std::mt19937 rng{std::random_device{}()};
  std::shuffle(slots.begin(), slots.end(), rng);
  for (int i = 0; i < kMines && i < static_cast<int>(slots.size()); ++i) {
    const int r = slots[static_cast<size_t>(i)] / kW;
    const int c = slots[static_cast<size_t>(i)] % kW;
    cells_[r][c].mine = true;
  }
  count_adj();
  mines_placed_ = true;
  phase_ = Phase::playing;
}

void Board::flood(int r, int c)
{
  if (!in_bounds(r, c))
    return;
  Cell& cell = cells_[r][c];
  if (cell.revealed || cell.flagged || cell.mine)
    return;
  cell.revealed = true;
  if (cell.adj > 0)
    return;
  for (int dr = -1; dr <= 1; ++dr)
    for (int dc = -1; dc <= 1; ++dc) {
      if (dr == 0 && dc == 0)
        continue;
      flood(r + dr, c + dc);
    }
}

void Board::check_win()
{
  if (phase_ != Phase::playing)
    return;
  for (int r = 0; r < kH; ++r)
    for (int c = 0; c < kW; ++c)
      if (!cells_[r][c].mine && !cells_[r][c].revealed)
        return;
  phase_ = Phase::won;
  for (int r = 0; r < kH; ++r)
    for (int c = 0; c < kW; ++c)
      if (cells_[r][c].mine && !cells_[r][c].flagged) {
        cells_[r][c].flagged = true;
        ++flags_;
      }
}

void Board::reveal_mines()
{
  for (int r = 0; r < kH; ++r)
    for (int c = 0; c < kW; ++c)
      if (cells_[r][c].mine && !cells_[r][c].flagged)
        cells_[r][c].revealed = true;
}

bool Board::reveal(int r, int c)
{
  if (phase_ == Phase::won || phase_ == Phase::lost)
    return true;
  if (!in_bounds(r, c))
    return true;
  Cell& cell = cells_[r][c];
  if (cell.flagged || cell.revealed)
    return true;
  ensure_mines(r, c);
  if (cell.mine) {
    cell.revealed = true;
    boom_r_ = r;
    boom_c_ = c;
    phase_ = Phase::lost;
    reveal_mines();
    return false;
  }
  flood(r, c);
  check_win();
  return true;
}

void Board::toggle_flag(int r, int c)
{
  if (phase_ == Phase::won || phase_ == Phase::lost)
    return;
  if (!in_bounds(r, c))
    return;
  Cell& cell = cells_[r][c];
  if (cell.revealed)
    return;
  cell.flagged = !cell.flagged;
  flags_ += cell.flagged ? 1 : -1;
}

}  // namespace kablamo

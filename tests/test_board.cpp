/* SPDX-License-Identifier: Unlicense */

#include "board.hpp"
#include "check.hpp"

#include <cstdlib>

namespace {

int count_mines(const kablamo::Board& board)
{
  int n = 0;
  for (int r = 0; r < kablamo::kH; ++r)
    for (int c = 0; c < kablamo::kW; ++c)
      if (board.at(r, c).mine)
        ++n;
  return n;
}

bool adj_matches(const kablamo::Board& board)
{
  for (int r = 0; r < kablamo::kH; ++r) {
    for (int c = 0; c < kablamo::kW; ++c) {
      int n = 0;
      for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
          if (dr == 0 && dc == 0)
            continue;
          const int rr = r + dr;
          const int cc = c + dc;
          if (rr < 0 || cc < 0 || rr >= kablamo::kH || cc >= kablamo::kW)
            continue;
          if (board.at(rr, cc).mine)
            ++n;
        }
      }
      if (board.at(r, c).adj != n)
        return false;
    }
  }
  return true;
}

void reveal_all_safe(kablamo::Board& board)
{
  bool progressed = true;
  while (progressed && board.phase() == kablamo::Phase::playing) {
    progressed = false;
    for (int r = 0; r < kablamo::kH; ++r) {
      for (int c = 0; c < kablamo::kW; ++c) {
        const kablamo::Cell& cell = board.at(r, c);
        if (cell.mine || cell.revealed || cell.flagged)
          continue;
        board.reveal(r, c);
        progressed = true;
      }
    }
  }
}

}  // namespace

int main()
{
  {
    kablamo::Board board;
    CHECK(board.phase() == kablamo::Phase::ready);
    CHECK(board.remaining() == kablamo::kMines);
    CHECK(board.reveal(-1, 0));
    CHECK(board.reveal(0, kablamo::kW));
    CHECK(board.phase() == kablamo::Phase::ready);
    board.toggle_flag(-1, 0);
    CHECK(board.remaining() == kablamo::kMines);
  }

  for (int trial = 0; trial < 30; ++trial) {
    kablamo::Board board;
    const int sr = trial % kablamo::kH;
    const int sc = (trial / kablamo::kH) % kablamo::kW;
    CHECK(board.reveal(sr, sc));
    CHECK(board.phase() == kablamo::Phase::playing);
    CHECK(!board.at(sr, sc).mine);
    CHECK(board.at(sr, sc).revealed);
    CHECK(count_mines(board) == kablamo::kMines);
    CHECK(adj_matches(board));

    for (int r = 0; r < kablamo::kH; ++r) {
      for (int c = 0; c < kablamo::kW; ++c) {
        const kablamo::Cell& cell = board.at(r, c);
        if (!cell.revealed || cell.adj != 0)
          continue;
        for (int dr = -1; dr <= 1; ++dr) {
          for (int dc = -1; dc <= 1; ++dc) {
            const int rr = r + dr;
            const int cc = c + dc;
            if (rr < 0 || cc < 0 || rr >= kablamo::kH || cc >= kablamo::kW)
              continue;
            CHECK(!board.at(rr, cc).mine);
            CHECK(board.at(rr, cc).revealed);
          }
        }
      }
    }

    reveal_all_safe(board);
    CHECK(board.phase() == kablamo::Phase::won);
  }

  {
    kablamo::Board board;
    CHECK(board.reveal(4, 4));
    bool found = false;
    for (int r = 0; r < kablamo::kH && !found; ++r) {
      for (int c = 0; c < kablamo::kW && !found; ++c) {
        if (!board.at(r, c).mine || board.at(r, c).revealed)
          continue;
        kablamo::Board lost = board;
        CHECK(!lost.reveal(r, c));
        CHECK(lost.phase() == kablamo::Phase::lost);
        CHECK(lost.boom_r() == r);
        CHECK(lost.boom_c() == c);
        CHECK(lost.at(r, c).revealed);
        found = true;
      }
    }
    CHECK(found);
  }

  {
    kablamo::Board board;
    board.toggle_flag(1, 1);
    CHECK(board.remaining() == kablamo::kMines - 1);
    CHECK(board.reveal(1, 1));
    CHECK(board.phase() == kablamo::Phase::ready);
    CHECK(!board.at(1, 1).revealed);
    board.toggle_flag(1, 1);
    CHECK(board.remaining() == kablamo::kMines);
    CHECK(board.reveal(1, 1));
    CHECK(board.phase() == kablamo::Phase::playing);
    board.toggle_flag(1, 1);
    CHECK(!board.at(1, 1).flagged);
    board.reset();
    CHECK(board.phase() == kablamo::Phase::ready);
    CHECK(!board.at(1, 1).revealed);
    CHECK(count_mines(board) == 0);
  }

  return suite_test::done("board");
}

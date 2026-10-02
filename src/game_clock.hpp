/* SPDX-License-Identifier: Unlicense */

#pragma once

namespace kablamo {

/* The HUD timer: whole seconds of play, shown on a three-digit LCD. */
class GameClock {
 public:
  static constexpr int kMax = 999;

  /* First reveal of a game. Nothing has elapsed yet, so the LCD reads 000;
   * the first tick, one second later, makes it 001. */
  void start()
  {
    running_ = true;
    seconds_ = 0;
  }
  /* One second of play has elapsed. */
  void tick()
  {
    if (running_ && seconds_ < kMax)
      ++seconds_;
  }
  void stop()
  {
    running_ = false;
  }
  void reset()
  {
    running_ = false;
    seconds_ = 0;
  }
  int seconds() const
  {
    return seconds_;
  }
  bool running() const
  {
    return running_;
  }

 private:
  int seconds_ = 0;
  bool running_ = false;
};

}  // namespace kablamo

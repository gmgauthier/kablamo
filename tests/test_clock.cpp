/* SPDX-License-Identifier: Unlicense */

#include "game_clock.hpp"
#include "check.hpp"

int main()
{
  kablamo::GameClock clock;
  CHECK(clock.seconds() == 0);

  /* The first reveal starts the clock at 0, not 1. */
  clock.start();
  CHECK(clock.running());
  CHECK(clock.seconds() == 0);

  /* One tick per elapsed second. */
  clock.tick();
  CHECK(clock.seconds() == 1);

  /* 999 is reached after 999 seconds, not 998, and the LCD stops there. */
  for (int i = 1; i < 998; ++i)
    clock.tick();
  CHECK(clock.seconds() == 998);
  clock.tick();
  CHECK(clock.seconds() == 999);
  clock.tick();
  CHECK(clock.seconds() == 999);

  /* Stop keeps the shown time; reset clears it. */
  clock.stop();
  CHECK(!clock.running());
  CHECK(clock.seconds() == 999);
  clock.reset();
  CHECK(clock.seconds() == 0);
  CHECK(!clock.running());

  return suite_test::done("clock");
}

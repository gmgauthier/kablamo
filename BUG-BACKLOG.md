# Bug backlog

Reviewed 2026-10-01 against the 0.1.1 sources.

`meson test` runs `tests/test_board.cpp` (`board`) and `tests/test_clock.cpp` (`clock`). `board` checks the shipped board rules: a safe first reveal, exactly ten mines, flood of zeros, win, loss, and flag-before-start. `clock` checks the HUD timer model: 0 at the first reveal, one per second, 999 after 999 seconds. The assertions below are not locked in as correct behavior.

The 9×9 board, ten mines, first-click safety, flood, flagging, and the 999 cap match the shipped rules. `Board::at()` is only called in range. A negative flag counter is drawn by the LCD and is not treated as a defect.

## Open

None.

## Closed

None.

## Closed

### Timer shows 1 before a second has elapsed

- Severity: incorrect
- Confidence: high
- Where: `src/main_window.cpp:120`
- Trigger: The first reveal that leaves the board in `Phase::playing`. `sync_hud` sets `seconds_ = 1` and paints it, then starts a 1000 ms timeout. `on_tick` (`src/main_window.cpp:100`) increments while `seconds_ < 999`.
- Outcome: The LCD is one second ahead of wall time for the rest of the game, and it reaches 999 after 998 seconds of play. An opening click that already won never enters this branch, so that finished game stays at 000 while a normal first click is already 001.
- Fixed in v0.1.3: The first reveal starts the clock at 000. Each elapsed second adds one, so the LCD matches wall time and reaches 999 after 999 seconds. An opening click that wins and a normal first click both read 000.

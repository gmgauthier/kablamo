# Bug backlog

Reviewed 2026-10-01 against the 0.1.1 sources.

`meson test` runs `tests/test_board.cpp` (`board`). It checks the shipped board rules: a safe first reveal, exactly ten mines, flood of zeros, win, loss, and flag-before-start. It does not drive the HUD clock. The assertions below are not locked in as correct behavior.

The 9×9 board, ten mines, first-click safety, flood, flagging, and the 999 cap match the shipped rules. `Board::at()` is only called in range. A negative flag counter is drawn by the LCD and is not treated as a defect.

## Open

### Timer shows 1 before a second has elapsed

- Severity: incorrect
- Confidence: high
- Where: `src/main_window.cpp:120`
- Trigger: The first reveal that leaves the board in `Phase::playing`. `sync_hud` sets `seconds_ = 1` and paints it, then starts a 1000 ms timeout. `on_tick` (`src/main_window.cpp:100`) increments while `seconds_ < 999`.
- Outcome: The LCD is one second ahead of wall time for the rest of the game, and it reaches 999 after 998 seconds of play. An opening click that already won never enters this branch, so that finished game stays at 000 while a normal first click is already 001.

## Closed

None.

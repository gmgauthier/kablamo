# Kablamo! backlog

Current release: **v0.1.0**. Last updated: 2026-09-15.

Win 3.1 / 95 Minesweeper. Binary `kablamo`. Suite catalog: `lcos-projects/PRODUCT-BACKLOG.md`. Plan: [DEVELOPMENT.md](DEVELOPMENT.md). How to land work: [DEVELOPMENT.md](DEVELOPMENT.md#process) — `feature/` / `fix/` branches, PRs to `master`, lint gate, semver on shipped PRs.

## High Priority

Nothing queued. v1 (M0–M3) is tagged: fixed 9×9, 10 mines. Parked work lives under Low Priority.

## Low Priority

- Intermediate / Expert / custom board
- Chord-click
- Question-mark marks
- Best Times
- Sound

## Out of Scope

- 81 GtkButtons (the field is one Cairo `DrawingArea`)
- Mines placed before the first click
- A save file
- Network
- Custom title bar; do not override `GTK_THEME`
- Bryan’s seal
- Replacing AisleRiot / Micropolis / Maelstrom / Zork (those already ship on the ISO)

## Shipped

**v0.1.0 (M0–M3)** — Fixed 9×9, 10 mines; first click safe; left-click reveal; right-click flag; flood zeros; win/lose; timer cap 999; Win95 LCD counters; face (smile / dead / shades / `O_O`); New / F2; detonated mine and wrong-flag X; `.deb` / tarball / AppImage. No save file.

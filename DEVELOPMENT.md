# Kablamo! development plan

A gtkmm-3 **Minesweeper** for LCOS. The *window* is Windows 3.1 / 95 Minesweeper.

Display name: **Kablamo!**  
Binary / repo / package: `kablamo`  
License: The Unlicense (`UNLICENSE`)  
Repos: https://gitea.scriptorium/gmgauthier/kablamo (origin), https://github.com/gmgauthier/kablamo

Reference window: `brand/ui-reference.svg`

LCOS already ships AisleRiot, Micropolis, Maelstrom, Zork. It does **not** ship Minesweeper.

## Status (2026-09-11)

**M3 in tree.** Packaging: `debian/`, `scripts/release.sh`, `INSTALL.md`. Tag `v0.1.0`.

## 1. Locked decisions

| Decision | Choice |
|---|---|
| Product | Original app. Chrome is Win 95 Minesweeper |
| Name | Kablamo! Binary `kablamo`. APP_ID `org.gmgauthier.Kablamo` |
| Toolkit | C++17, gtkmm-3.0, GTK3 CSS, Meson |
| Board | **Fixed 9×9, 10 mines** (Beginner). No Intermediate / Expert / custom in v1 |
| Look | One decorated window. Raised cells, sunken revealed field, face button, mine counter, timer |
| Input | Left-click reveal, right-click flag. First click is never a mine |
| Never as v1 | Chord-click, question-mark marks, high scores, sound, custom size, difficulty menu |
| Network | None |
| Files | None. No save format |
| Init | No systemd |
| Brand | Borrow LCOS beige / navy. Do **not** use Bryan’s seal. Mark is a small mine / boom burst |
| License | The Unlicense |
| Versioning | `meson.build` is the source of truth |

## 2. Window

```
Game              Help
 New        F2    About Kablamo!
 ────────
 Exit
```

Status strip: remaining mines, face, timer. Face: smile / dead / shades / `O_O` while a cell is pressed. Click face = New.

## 3. Architecture

Cairo `Gtk::DrawingArea` for the field (not 81 GtkButtons). gtkmm-3.0 only.

`Board` (M1): 9×9, 10 mines, place after first click excluding that cell, flood zeros, win/lose.

`MineField`: 24px cells, raised/sunken 3D, hit-test, press/release.

Timer: 1 s ticks while playing, cap 999.

## 4. Milestones

### M0 — Window

Scaffold. Empty 9×9 raised grid, face, counters at 10 / 000. New and F2 reset the drawing. Clicks depress a cell; no mines yet.

### M1 — Playable

Reveal / flood / flag / first-click safe / win / lose. Timer. Face states.

### M2 — Chrome polish

Win95 number colors, detonated mine, wrong-flag X, LCD polish. README: **Vended by Grok Build**.

### M3 — Package (this slice)

`debian/`, `scripts/release.sh` (`.deb` + tarball + AppImage). Tag `v0.1.0`. Attach all three artifacts.

## 5. Parked

Intermediate / Expert / custom, chord-click, question marks, Best Times, sound.

## 6. Traps

- 81 GtkButtons
- Mines before first click
- Custom title bar
- Overriding `GTK_THEME`; `prefer_light_theme` only
- Bryan’s seal

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
| Versioning | Semantic (`MAJOR.MINOR.PATCH`). `meson.build` is the source of truth. Debian changelog and git tag `vX.Y.Z` match it. See **Process**. |

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

## Process

Do not commit to `master`. Every change lands through a pull request.

### Branches

- `feature/<short-name>` — new user-visible work
- `fix/<short-name>` — bugs, packaging nits, regressions

Open a pull request into `master`. Merge only after review.

### Gates

A pull request must pass **lint** before merge. CI runs `./scripts/lint.sh` (no `--fix`). Locally:

- `./scripts/lint.sh --fix` — clang-format rewrites `src/`
- `./scripts/lint.sh` — SPDX headers, no tabs, clang-format `--dry-run --Werror`, cppcheck (`warning`) on `src/`
- `meson compile` with this tree’s `warning_level=2` is clean (no new warnings)

Do not pass `--fix` in CI. Do not merge a red PR.

**Tests** are required when they exist (`meson test -C build`). Until a test suite lands, the gate is lint plus a clean compile plus a manual pass of the change.

### Semantic versioning

Every **shipped** pull request — merged to `master` and tagged as a release — bumps the version. `meson.build` is the source of truth. Keep these in lockstep in the same PR:

- `meson.build` `version:`
- `debian/changelog` (new stanza)
- git tag `vMAJOR.MINOR.PATCH` after merge

Then `./scripts/release.sh` produces `.deb`, tarball, and AppImage.

| Bump | When |
|---|---|
| **PATCH** (`x.y.Z`) | Bug fix or packaging. No new user-facing feature. |
| **MINOR** (`x.Y.0`) | New backward-compatible feature. |
| **MAJOR** (`X.0.0`) | Breaking change: native file format, dropped config keys, removed UI users rely on. |

While the version is `0.y.z`, still bump MINOR and PATCH this way. Do not treat 0.x as a free-for-all. The Debian revision (`-1`, `-2`) is only for rebuilding the same upstream version with no source change.

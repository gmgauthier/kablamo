# Kablamo!

**Vended by Grok Build**

A **Minesweeper** for The Lunduke Computer Operating System (LCOS). The window is Windows 3.1 / 95 Minesweeper.

Binary: `kablamo`. Unlicense.

LCOS itself: [https://github.com/BryanLunduke/LCOS](https://github.com/BryanLunduke/LCOS)

## Status

**M0 in tree.** Window, 9×9 raised grid, face, counters. Playable mines in M1.

| Doc | What |
|---|---|
| [DEVELOPMENT.md](DEVELOPMENT.md) | Locked decisions, architecture, milestones M0–M3 |

## Build

```
sudo apt install build-essential meson ninja-build pkg-config g++ libgtkmm-3.0-dev
meson setup build
meson compile -C build
./build/kablamo
```

## License

[The Unlicense](https://unlicense.org). See [UNLICENSE](UNLICENSE).

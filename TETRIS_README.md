# Tetris

Tetris is available from the main menu as option 7. It uses a 10-column, 20-row
visible playfield with two hidden rows for new pieces. The seven tetrominoes are
dealt from a shuffled 7-bag, and a fixed seed always produces the same sequence.

## Controls

- Left/Right arrows or `a`/`d`: move the active piece.
- Down arrow or `s`: soft drop one row for 1 point.
- Up arrow or `x`: rotate clockwise; `z`: rotate counter-clockwise.
- Space: hard drop and lock immediately, scoring 2 points per row fallen.
- `c`: hold or swap the active piece once before the next piece.
- `p`: pause or resume. The game timers stop while paused.
- `q`, Escape, or Ctrl-C: leave Tetris and return to the menu.
- After game over, press `r` to restart or `q`/Escape to return to the menu.

Arrow-key input is read without pressing Enter. The Down key is handled as a
single soft-drop step per press because terminal input does not report key-up
events.

## Scoring and levels

Line-clear scores are multiplied by the current level:

| Lines cleared at once | Base score |
| --- | ---: |
| 1 | 100 |
| 2 | 300 |
| 3 | 500 |
| 4 | 800 |

Back-to-back Tetrises score 1.5 times the normal Tetris points. Every 10 cleared
lines advances one level. Gravity starts at one row per 45 ticks (about
three-quarters of a second) and speeds up to three ticks per row at level 15.
Soft and hard drops award their per-row points in addition to line-clear points.

## Design

`TetrisGame` in `src/tetris.h` and `src/tetris.cpp` owns the board, 7-bag,
piece queue, hold slot, scoring, gravity, lock delay, line clears, and SRS
rotation kicks. It has no terminal input or output, so its rules can be tested
independently. `playTetris()` is the POSIX terminal layer: it configures raw
input with `termios`, polls input with `select`, draws the board and side
panels, and restores the terminal when the game exits.

The game shows a landing ghost, held piece, next three pieces, current score,
level, cleared lines, and the session high score. Pieces lock after 30 ticks on
the ground; successful movement or rotation can reset that delay up to 15 times.

Build and test the project from the repository root:

```sh
cmake --build build
ctest --test-dir build --output-on-failure
./build/games
```

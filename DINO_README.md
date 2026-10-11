# Dinosaur Run

A terminal version of Chrome's offline T-Rex runner, added to Terminal Games as
menu option 4.

## Playing

Select **4. Dinosaur Run** from the main menu. You need an interactive terminal
of at least 60 columns by 23 rows (100 columns wide is best) that supports
256 colours, which is nearly every modern terminal.

| Key                | Action                                   |
| ------------------ | ---------------------------------------- |
| Space, Up, `w`, `j` | Start, jump, and restart after a crash   |
| Down, `s`          | Duck. In the air it makes you fall fast  |
| `q`, Esc, Ctrl-C   | Back to the menu                         |

No Enter needed. Terminals do not report key releases, so a duck lasts about half
a second after the last Down press; hold the key to keep ducking.

## What is in it

- Pixel-art dinosaur with a running animation, a duck pose and a dead pose
- Small and large cacti, in groups of up to three
- Pterodactyls that show up once you are fast enough, at three heights: jump the
  low ones, duck the middle ones and run under the high ones
- The game speeds up the longer you survive
- Score, a high score that lasts until you leave the program, and a flashing
  score every 100 points
- Night mode with a moon and stars every 700 points
- Scrolling ground and clouds, and a game over screen with a restart icon

## Implementation

- [`src/dino.h`](src/dino.h) defines `DinoGame`, the game logic. It does not touch
  the terminal. One `step()` call is one 1/60 s tick, and the same seed always
  plays out the same way, which is what makes it testable.
- [`src/dino.cpp`](src/dino.cpp) holds the sprites, the logic, and the terminal code.
  The scene is drawn into a pixel buffer, then every two pixels are folded into one
  terminal cell with half-block characters (`▀`), so pixels come out square.
  Colours use the 256-colour palette so it also works in macOS Terminal.
  The terminal is switched to raw input and the alternate screen, and put back
  when the game exits.
- [`tests/tests.cpp`](tests/tests.cpp) tests jump height and timing, fast fall,
  crashing, each bird height with and without ducking, and has an autopilot play
  tens of thousands of ticks at full speed to prove that the obstacle gaps can always
  be cleared.

Rebuild after changing code, then run the tests:

```sh
cmake --build build
ctest --test-dir build --output-on-failure
```

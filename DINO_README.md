# Dinosaur Run

This document covers only the Dinosaur Run game added to Terminal Games.

## How it started

Dinosaur Run began as a turn-based terminal game: the player typed a command
and pressed Enter for each step. It was then changed into a real-time runner so
the dinosaur and cactus move continuously while the player can provide input.
The artwork and obstacle behavior were refined to resemble the supplied
reference: a small multi-line dinosaur and a short cactus that can be cleared
with a jump.

## Playing

Select **4. Dinosaur Run** from the main menu.

- Press **Space**, **W**, or **J** to jump. Do not press Enter.
- Press **Q** to return to the main menu.
- Clear each cactus to increase the score. Hitting one ends the run.

The game needs an interactive terminal for real-time keyboard input.
Choose option `4` in the menu. Run the build command again after changing code.
To run the project tests:

```sh
ctest --test-dir build-xcode --output-on-failure
```

## Implementation

- [`src/dino.cpp`](src/dino.cpp) contains the real-time game loop, terminal
  keyboard handling, rendering, movement, and collision behavior.
- [`src/dino.h`](src/dino.h) defines the testable game state.
- [`src/main.cpp`](src/main.cpp) adds Dinosaur Run as menu option 4.
- [`CMakeLists.txt`](CMakeLists.txt) includes the game in the build.
- [`tests/tests.cpp`](tests/tests.cpp) checks jumping, obstacle clearance, and
  collision behavior.

The game temporarily switches the terminal to non-canonical input so keypresses
are read without Enter, and restores the terminal settings when it exits.

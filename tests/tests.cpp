#include <algorithm>
#include <iostream>
#include <string>

#include "dino.h"
#include "game2048.h"
#include "hangman.h"
#include "tictactoe.h"

static int failures = 0;

#define CHECK(expr)                                                              \
    do {                                                                         \
        if (!(expr)) {                                                           \
            std::cout << "FAIL " << __FILE__ << ":" << __LINE__ << "  " #expr "\n"; \
            failures++;                                                          \
        }                                                                        \
    } while (0)

static Board board(const std::string& s) {
    Board b;
    for (int i = 0; i < 9; i++) b[i] = s[i];
    return b;
}

// Plays every possible sequence of X (player) moves, O (computer) replies with
// computerMove each time. Returns false if X ever wins or O makes an illegal move.
static int gamesPlayed = 0;

static bool computerNeverLoses(const Board& b) {
    for (int i = 0; i < 9; i++) {
        if (b[i] != ' ') continue;
        Board next = b;
        next[i] = 'X';
        if (winner(next) == 'X') return false;
        if (isFull(next)) { gamesPlayed++; continue; }

        int m = computerMove(next, 'O', 'X');
        if (m < 0 || m > 8 || next[m] != ' ') return false;
        next[m] = 'O';
        if (winner(next) == 'O' || isFull(next)) { gamesPlayed++; continue; }

        if (!computerNeverLoses(next)) return false;
    }
    return true;
}

static void testTicTacToe() {
    resetMemo();  
    CHECK(winner(board("XXX OO   ")) == 'X');
    CHECK(winner(board("O  O  O  ")) == 'O');
    CHECK(winner(board("X   X   X")) == 'X');
    CHECK(winner(board("  X X X  ")) == 'X');
    CHECK(winner(board("XOXOXOOXO")) == ' ');
    CHECK(isFull(board("XOXOXOOXO")));
    CHECK(computerMove(board("OO XX    "), 'O', 'X') == 2);  // takes the win
    CHECK(computerMove(board("XX  O    "), 'O', 'X') == 2);  // blocks (only non-losing move)
    CHECK(computerMove(board("         "), 'O', 'X') == 4);  // centre first
    resetMemo();
    CHECK(computerMove(board("XOX O X  "), 'O', 'X') == 7);
    resetMemo();  // boards above aren't all legal positions; start the memo clean
    gamesPlayed = 0;
    CHECK(computerNeverLoses(board("         ")));  // X moves first, every line of play
    CHECK(gamesPlayed > 0);                          // check that atleast 1 game is played and test done
}

static void test2048() {
    int score = 0;
    CHECK((Game2048::slideRow({2, 2, 0, 0}, score) == Row{4, 0, 0, 0}));
    CHECK(score == 4);
    CHECK((Game2048::slideRow({0, 0, 0, 2}, score) == Row{2, 0, 0, 0}));
    CHECK((Game2048::slideRow({2, 4, 8, 16}, score) == Row{2, 4, 8, 16}));

    int score_quad = 0;
    CHECK((Game2048::slideRow({2, 2, 2, 2}, score_quad) == Row{4, 4, 0, 0}));
    CHECK(score_quad == 8);

    int score_cascade = 0;
    CHECK((Game2048::slideRow({4, 2, 2, 0}, score_cascade) == Row{4, 4, 0, 0}));
    CHECK(score_cascade == 4);

    int score_pairs = 0;
    CHECK((Game2048::slideRow({2, 2, 4, 4}, score_pairs) == Row{4, 8, 0, 0}));
    CHECK(score_pairs == 12);

    Game2048 game(42);
    game.setGrid({Row{2, 2, 0, 0}, Row{0, 0, 0, 0}, Row{0, 0, 0, 0}, Row{0, 0, 0, 0}});
    CHECK(game.move(Direction::Right));
    CHECK(game.grid()[0][3] == 4);
    CHECK(!game.hasWon());

    Game2048 noOpGame(42);
    noOpGame.setGrid({Row{2, 0, 0, 0}, Row{4, 0, 0, 0}, Row{0, 0, 0, 0}, Row{0, 0, 0, 0}});
    auto beforeNoOp = noOpGame.grid();
    CHECK(!noOpGame.move(Direction::Left));
    CHECK(noOpGame.grid() == beforeNoOp);
    CHECK(noOpGame.score() == 0);

    Game2048 undoGame(42);
    std::array<Row, 4> beforeUndo{Row{2, 2, 0, 0}, Row{0, 0, 0, 0}, Row{0, 0, 0, 0}, Row{0, 0, 0, 0}};
    undoGame.setGrid(beforeUndo);
    CHECK(undoGame.move(Direction::Right));
    auto afterMove = undoGame.grid();
    CHECK(undoGame.score() == 4);
    CHECK(undoGame.undo());
    CHECK(undoGame.grid() == beforeUndo);
    CHECK(undoGame.score() == 0);
    CHECK(!undoGame.undo());  // only one undo is available
    CHECK(undoGame.move(Direction::Right));
    CHECK(undoGame.grid() == afterMove);  // RNG state was restored too

    Game2048 preservedUndoGame(42);
    std::array<Row, 4> beforePreservedUndo{
        Row{2, 2, 8, 16}, Row{32, 64, 128, 256},
        Row{64, 128, 256, 512}, Row{128, 256, 512, 1024}};
    preservedUndoGame.setGrid(beforePreservedUndo);
    CHECK(preservedUndoGame.move(Direction::Left));
    CHECK(!preservedUndoGame.move(Direction::Left));
    CHECK(preservedUndoGame.undo());
    CHECK(preservedUndoGame.grid() == beforePreservedUndo);
    CHECK(preservedUndoGame.score() == 0);

    Game2048 noUndoGame(42);
    noUndoGame.setGrid({Row{2, 0, 0, 0}, Row{0, 0, 0, 0}, Row{0, 0, 0, 0}, Row{0, 0, 0, 0}});
    CHECK(!noUndoGame.move(Direction::Left));
    CHECK(!noUndoGame.undo());

    game.setGrid({Row{2, 4, 2, 4}, Row{4, 2, 4, 2}, Row{2, 4, 2, 4}, Row{4, 2, 4, 2}});
    CHECK(!game.canMove());
}

static void testHangman() {
    Hangman game("github");
    CHECK(game.guess('g'));
    CHECK(!game.guess('z'));
    CHECK(game.wrongGuesses() == 1);
    CHECK(game.masked() == "g _ _ _ _ _");
    for (char c : std::string("ithub")) game.guess(c);
    CHECK(game.won());
}

// A simple autopilot: jumps cacti and low birds, ducks mid-height birds.
static void autopilot(const DinoGame& game, bool& jump, bool& duck) {
    jump = false;
    duck = false;
    const double front = dino::kDinoX + dino::kDinoW;
    for (const auto& o : game.obstacles()) {
        if (o.x + o.width < dino::kDinoX + 4) continue;  // already behind us
        const double closing = game.speed() + o.speedOffset;
        const double d = o.x - front;
        const int altitude = dino::kGroundY - o.y - o.height;
        if (o.kind == DinoObstacle::Kind::Bird) {
            if (altitude >= dino::kBirdHigh) continue;
            if (altitude >= dino::kBirdMid) {
                duck = d <= closing * 14;
                return;
            }
        }
        const double launch = closing * DinoGame::kJumpTicks / 2.0 - (o.width + 10) / 2.0;
        jump = !game.airborne() && d <= launch;
        return;
    }
}

static void testDino() {
    CHECK(dinoArtIsValid());

    // Waits for the first jump.
    DinoGame ready(7);
    CHECK(ready.ready());
    for (int i = 0; i < 100; ++i) ready.step(false, false);
    CHECK(ready.ready());
    CHECK(ready.obstacles().empty());
    ready.step(true, false);
    CHECK(ready.running());
    CHECK(ready.airborne());

    // A jump peaks at the documented height and lands again after about a second.
    DinoGame jumper(7);
    jumper.step(true, false);
    int peak = jumper.heightAboveGround();
    int airTicks = 1;
    while (jumper.airborne() && airTicks < 200) {
        jumper.step(false, false);
        peak = std::max(peak, jumper.heightAboveGround());
        ++airTicks;
    }
    CHECK(peak >= 23 && peak <= 25);
    CHECK(airTicks >= 36 && airTicks <= 40);
    CHECK(!jumper.airborne());

    // Holding down in the air brings you back to the ground sooner.
    DinoGame diver(7);
    diver.step(true, false);
    for (int i = 0; i < 10; ++i) diver.step(false, false);
    int diveTicks = 0;
    while (diver.airborne() && diveTicks < 200) {
        diver.step(false, true);
        ++diveTicks;
    }
    CHECK(diveTicks < 10);

    // Standing still means crashing into the first obstacle, which is not at once.
    DinoGame idle(3);
    idle.step(true, false);
    long idleTicks = 0;
    while (!idle.crashed() && idleTicks < 2000) {
        idle.step(false, false);
        ++idleTicks;
    }
    CHECK(idle.crashed());
    CHECK(idleTicks > 100);
    CHECK(idle.highScore() == idle.score());
    const int frozenScore = idle.score();
    for (int i = 0; i < 50; ++i) idle.step(true, false);
    CHECK(idle.score() == frozenScore);  // a crashed game stays crashed

    // The same seed plays out the same way.
    DinoGame a(42), b(42);
    a.step(true, false);
    b.step(true, false);
    bool same = true;
    for (int i = 0; i < 1500; ++i) {
        a.step(false, false);
        b.step(false, false);
        if (a.crashed() != b.crashed() || a.score() != b.score() || a.obstacles().size() != b.obstacles().size())
            same = false;
    }
    CHECK(same);

    // Ducking helps against a mid-height bird, and only against that one.
    auto birdTest = [](int altitude, bool duck, bool jump) {
        DinoGame g(1, 100);
        g.start();
        g.spawnObstacle(DinoObstacle::Kind::Bird, 60, 1, altitude);
        for (int i = 0; i < 120 && !g.crashed(); ++i) {
            bool autoJump, autoDuck;
            autopilot(g, autoJump, autoDuck);
            g.step(jump && autoJump, duck);
        }
        return !g.crashed();
    };
    CHECK(!birdTest(dino::kBirdMid, false, false));  // standing: hit
    CHECK(birdTest(dino::kBirdMid, true, false));    // ducking: safe
    CHECK(!birdTest(dino::kBirdLow, true, false));   // ducking a low bird: hit
    CHECK(birdTest(dino::kBirdLow, false, true));    // jumping a low bird: safe
    CHECK(birdTest(dino::kBirdHigh, false, false));  // high bird: walk under
    CHECK(!birdTest(dino::kBirdLow, false, false));

    // A well-timed jump clears every kind of cactus group, at the start speed.
    auto cactusTest = [](DinoObstacle::Kind kind, int count) {
        DinoGame g(1, 100);
        g.start();
        g.spawnObstacle(kind, 100, count);
        for (int i = 0; i < 160 && !g.crashed(); ++i) {
            bool jump, duck;
            autopilot(g, jump, duck);
            g.step(jump, duck);
        }
        return !g.crashed();
    };
    CHECK(cactusTest(DinoObstacle::Kind::SmallCactus, 1));
    CHECK(cactusTest(DinoObstacle::Kind::SmallCactus, 2));
    CHECK(cactusTest(DinoObstacle::Kind::LargeCactus, 1));

    // The autopilot can play for a long time on several seeds. That shows the
    // generated obstacle gaps always leave enough room, even at full speed,
    // and that night, birds and score flashing all happen along the way.
    for (unsigned seed = 1; seed <= 6; ++seed) {
        DinoGame game(seed, 100);
        game.step(true, false);
        bool sawBird = false, sawNight = false, sawBigGroup = false;
        for (long i = 0; i < 40000 && !game.crashed(); ++i) {
            bool jump, duck;
            autopilot(game, jump, duck);
            game.step(jump, duck);
            for (const auto& o : game.obstacles()) {
                sawBird = sawBird || o.kind == DinoObstacle::Kind::Bird;
                sawBigGroup = sawBigGroup || o.count > 1;
            }
            sawNight = sawNight || game.night();
        }
        CHECK(!game.crashed());
        CHECK(game.score() > 2000);
        CHECK(game.speed() == DinoGame::kMaxSpeed);
        CHECK(sawBird && sawNight && sawBigGroup);
    }

    // The high score carries over to the next game.
    DinoGame next(5, 100, 1234);
    CHECK(next.highScore() == 1234);
}

int main() {
    testTicTacToe();
    test2048();
    testHangman();
    testDino();
    if (failures == 0) std::cout << "All tests passed.\n";
    return failures == 0 ? 0 : 1;
}

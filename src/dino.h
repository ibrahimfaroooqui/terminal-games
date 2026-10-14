#pragma once

#include <cstdint>
#include <random>
#include <vector>

// Dinosaur Run: a terminal take on Chrome's offline T-Rex game.
//
// The game works in "pixels". The renderer draws two pixels per terminal row
// (using half-block characters), so one pixel is roughly square on screen.
// The logic below is independent of the terminal so it can be unit tested:
// one call to step() is one 1/60 s tick.

namespace dino {

constexpr int kFieldHeight = 46;  // pixels, i.e. 23 terminal rows
constexpr int kGroundY = 43;      // y of the ground line (y grows downwards)
constexpr int kDinoX = 6;         // left edge of the dinosaur

constexpr int kDinoW = 18, kDinoH = 18;
constexpr int kDuckW = 24, kDuckH = 10;
constexpr int kSmallCactusW = 7, kSmallCactusH = 11;
constexpr int kLargeCactusW = 10, kLargeCactusH = 15;
constexpr int kBirdW = 15, kBirdH = 10;
constexpr int kCloudW = 18, kCloudH = 4;

constexpr int kTicksPerSecond = 60;

// Bird heights, measured from the ground to the bottom of the bird.
constexpr int kBirdLow = 2;    // jump over it
constexpr int kBirdMid = 8;    // duck under it (or jump)
constexpr int kBirdHigh = 16;  // just run under it

struct Box {
    int x, y, w, h;
};

}  // namespace dino

struct DinoObstacle {
    enum class Kind { SmallCactus, LargeCactus, Bird };

    Kind kind = Kind::SmallCactus;
    double x = 0;  // left edge
    int y = 0;     // top edge
    int count = 1; // cacti in the group
    int width = 0;
    int height = 0;
    double speedOffset = 0;  // birds fly a little faster or slower than the ground
    double gap = 0;          // free space to leave before the next obstacle
    int frame = 0;           // bird wing frame
};

struct DinoCloud {
    double x;
    int y;
};

struct DinoStar {
    double x;
    int y;
};

class DinoGame {
public:
    enum class State { Ready, Running, Crashed };

    explicit DinoGame(unsigned seed = 1, int width = 100, int highScore = 0);

    // Starts the run without jumping (used for "restart").
    void start();
    // Advances one tick. `jump` is a fresh jump press, `duck` is "down is held".
    // In the Ready state a jump starts the run.
    void step(bool jump, bool duck);

    // Adds an obstacle for tests; `altitude` only matters for birds.
    void spawnObstacle(DinoObstacle::Kind kind, double x, int count = 1, int altitude = dino::kBirdLow);

    State state() const { return state_; }
    bool ready() const { return state_ == State::Ready; }
    bool running() const { return state_ == State::Running; }
    bool crashed() const { return state_ == State::Crashed; }
    int score() const { return score_; }
    int highScore() const { return highScore_; }
    double speed() const { return speed_; }
    int width() const { return width_; }
    long ticks() const { return ticks_; }

    int heightAboveGround() const { return static_cast<int>(height_ + 0.5); }
    bool airborne() const { return height_ > 0.0; }
    bool ducking() const { return ducking_; }
    bool dropping() const { return dropping_; }
    int runFrame() const { return (runTicks_ / 5) % 2; }

    bool night() const { return nightTicks_ > 0; }
    bool scoreVisible() const { return flashTicks_ <= 0 || (flashTicks_ / 15) % 2 == 1; }
    double groundOffset() const { return groundOffset_; }
    double moonX() const { return moonX_; }
    const std::vector<std::uint8_t>& groundStrip() const { return strip_; }
    const std::vector<DinoObstacle>& obstacles() const { return obstacles_; }
    const std::vector<DinoCloud>& clouds() const { return clouds_; }
    const std::vector<DinoStar>& stars() const { return stars_; }

    // Collision boxes of the dinosaur in field coordinates.
    std::vector<dino::Box> dinoBoxes() const;

    static constexpr int kJumpTicks = 38;
    static constexpr double kJumpApex = 24.0;
    static constexpr double kStartSpeed = 1.2;
    static constexpr double kMaxSpeed = 2.4;

private:
    void spawnNext();
    void updateScenery();
    bool collides() const;
    int randomInt(int low, int high);

    std::mt19937 rng_;
    int width_;
    State state_ = State::Ready;
    long ticks_ = 0;
    long runTicks_ = 0;

    double speed_ = kStartSpeed;
    double distance_ = 0;
    int score_ = 0;
    int highScore_;
    int flashTicks_ = 0;

    double height_ = 0;
    double velocity_ = 0;
    bool ducking_ = false;
    bool dropping_ = false;

    int nightTicks_ = 0;
    int nextNightScore_ = 700;

    double groundOffset_ = 0;
    double moonX_;
    int cloudGap_ = 60;
    std::vector<std::uint8_t> strip_;
    std::vector<DinoObstacle> obstacles_;
    std::vector<DinoCloud> clouds_;
    std::vector<DinoStar> stars_;
    std::vector<DinoObstacle::Kind> history_;
};

// True if every sprite has the size the constants above promise.
bool dinoArtIsValid();

void playDino();

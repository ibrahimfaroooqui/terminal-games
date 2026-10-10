#include "dino.h"

#include <array>
#include <chrono>
#include <iostream>
#include <limits>
#include <string>
#include <sys/select.h>
#include <termios.h>
#include <thread>
#include <unistd.h>

namespace {

class TerminalMode {
public:
    TerminalMode() = default;

    bool enable() {
        if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO)) {
            std::cerr << "Dinosaur Run needs an interactive terminal.\n";
            return false;
        }
        if (tcgetattr(STDIN_FILENO, &original_) != 0) {
            std::cerr << "Could not read terminal settings.\n";
            return false;
        }

        termios raw = original_;
        raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) != 0) {
            std::cerr << "Could not enable real-time keyboard input.\n";
            return false;
        }

        active_ = true;
        std::cout << "\x1b[?1049h\x1b[?25l";
        std::cout.flush();
        return true;
    }

    ~TerminalMode() {
        if (active_) {
            tcsetattr(STDIN_FILENO, TCSANOW, &original_);
            std::cout << "\x1b[?25h\x1b[?1049l";
            std::cout.flush();
        }
    }

    TerminalMode(const TerminalMode&) = delete;
    TerminalMode& operator=(const TerminalMode&) = delete;

private:
    termios original_{};
    bool active_ = false;
};

bool readKey(char& key) {
    fd_set input;
    FD_ZERO(&input);
    FD_SET(STDIN_FILENO, &input);
    timeval timeout{};
    timeout.tv_usec = 100000;

    const int ready = select(STDIN_FILENO + 1, &input, nullptr, nullptr, &timeout);
    if (ready < 0) {
        std::cerr << "Could not read keyboard input.\n";
        return false;
    }
    if (ready == 0) return true;

    const ssize_t bytesRead = read(STDIN_FILENO, &key, 1);
    if (bytesRead < 0) {
        std::cerr << "Could not read keyboard input.\n";
        return false;
    }
    return bytesRead > 0;
}

void draw(const DinoGame& game) {
    constexpr int trackWidth = 60;
    constexpr int dinoPosition = 4;
    constexpr int groundRow = 7;
    const std::array<std::string, 4> runningDino = game.alternateFrame()
        ? std::array<std::string, 4>{"  __  ", " /o \\ ", "/|___>", " _/ \\ "}
        : std::array<std::string, 4>{"  __  ", " /o \\ ", "/|___>", " / \\  "};
    const std::array<std::string, 3> cactus{"  |  ", " ||| ", " | | "};

    std::array<std::string, groundRow + 1> scene;
    for (auto& row : scene) row.assign(trackWidth, ' ');

    const int dinoTop = groundRow - static_cast<int>(runningDino.size())
                        - (game.jumping() ? 3 : 0);
    for (size_t row = 0; row < runningDino.size(); ++row) {
        for (size_t column = 0; column < runningDino[row].size(); ++column) {
            const int x = dinoPosition + static_cast<int>(column);
            const int y = dinoTop + static_cast<int>(row);
            if (x >= 0 && x < trackWidth && y >= 0 && y < groundRow
                && runningDino[row][column] != ' ') {
                scene[y][x] = runningDino[row][column];
            }
        }
    }

    const int cactusPosition = dinoPosition + static_cast<int>(runningDino[0].size())
                               + game.distanceToObstacle();
    for (size_t row = 0; row < cactus.size(); ++row) {
        for (size_t column = 0; column < cactus[row].size(); ++column) {
            const int x = cactusPosition + static_cast<int>(column);
            const int y = groundRow - static_cast<int>(cactus.size())
                          + static_cast<int>(row);
            if (x >= 0 && x < trackWidth && cactus[row][column] != ' ') {
                scene[y][x] = cactus[row][column];
            }
        }
    }

    scene[groundRow].assign(trackWidth, '_');
    std::cout << "\x1b[H"
              << "Dinosaur Run   Score: " << game.score() << "\n"
              << "Press SPACE, W, or J to jump. Press Q to quit.\n\n";
    for (const auto& row : scene) std::cout << row << '\n';
    std::cout.flush();
}

}  // namespace

bool DinoGame::step(bool jump) {
    if (!alive_) return false;

    if (jump && jumpTicks_ == 0) jumpTicks_ = jumpDuration_;
    --distance_;
    if (distance_ <= 0 && jumpTicks_ == 0) {
        alive_ = false;
        return false;
    }
    if (distance_ <= -cactusWidth_) {
        ++score_;
        distance_ = 30 + score_ % 7;
    }

    if (jumpTicks_ > 0) --jumpTicks_;
    alternateFrame_ = !alternateFrame_;
    return alive_;
}

void playDino() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    TerminalMode terminal;
    if (!terminal.enable()) return;

    DinoGame game;
    while (game.alive()) {
        draw(game);
        char key = '\0';
        if (!readKey(key)) return;
        if (key == 'q' || key == 'Q') return;
        game.step(key == ' ' || key == 'w' || key == 'W' || key == 'j' || key == 'J');
    }

    draw(game);
    std::cout << "\nCrash! Final score: " << game.score() << "\n";
    std::cout.flush();
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

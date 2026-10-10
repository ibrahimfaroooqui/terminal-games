#pragma once

class DinoGame {
public:
    bool step(bool jump);
    bool alive() const { return alive_; }
    int distanceToObstacle() const { return distance_; }
    int score() const { return score_; }
    bool jumping() const { return jumpTicks_ > 0; }
    bool alternateFrame() const { return alternateFrame_; }

private:
    static constexpr int cactusWidth_ = 5;
    static constexpr int jumpDuration_ = 18;

    int distance_ = 32;
    int score_ = 0;
    int jumpTicks_ = 0;
    bool alive_ = true;
    bool alternateFrame_ = false;
};

void playDino();

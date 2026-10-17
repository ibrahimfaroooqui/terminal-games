#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <utility>
#include <vector>

enum class Suit { Spades, Hearts, Diamonds, Clubs };
enum class Rank : int { Two = 2, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace };

class Card {
public:
    Card(Suit suit, Rank rank) : suit_(suit), rank_(rank) {}

    Suit suit() const { return suit_; }
    Rank rank() const { return rank_; }
    int value() const;
    std::string rankText() const;
    std::string suitText() const;
    std::array<std::string, 7> render(bool color = true) const;
    static std::array<std::string, 7> renderBack(bool color = true);

private:
    Suit suit_;
    Rank rank_;
};

class Deck {
public:
    explicit Deck(unsigned seed = std::random_device{}(), std::size_t decks = 6);

    Card draw();
    void prepareForRound();
    std::size_t remaining() const { return cards_.size() - next_; }
    std::size_t size() const { return cards_.size(); }
    bool needsShuffle() const;

private:
    void resetAndShuffle();

    std::mt19937 rng_;
    std::size_t deckCount_;
    std::vector<Card> cards_;
    std::size_t next_ = 0;
};

class Hand {
public:
    void add(Card card) { cards_.push_back(std::move(card)); }
    void clear() { cards_.clear(); }
    const std::vector<Card>& cards() const { return cards_; }
    int total() const;
    bool isSoft() const;
    bool isBust() const { return total() > 21; }
    bool isBlackjack() const;

private:
    std::vector<Card> cards_;
};

enum class BlackjackResult {
    NotStarted,
    InProgress,
    Blackjack,
    DealerBlackjack,
    PlayerWin,
    DealerWin,
    Push,
    PlayerBust
};

class BlackjackGame {
public:
    static constexpr int kStartingBankrollCents = 100000;
    static constexpr int kMinimumBetDollars = 10;

    explicit BlackjackGame(unsigned seed = std::random_device{}(), std::size_t decks = 6);

    bool startRound(int betDollars);
    bool hit();
    bool stand();
    bool canDoubleDown() const;
    bool doubleDown();
    bool dealerShouldHit() const;
    bool dealerStep();

    int bankrollCents() const { return bankrollCents_; }
    int wagerCents() const { return wagerCents_; }
    bool playerTurn() const { return playerTurn_; }
    bool dealerTurn() const { return dealerTurn_; }
    bool holeCardRevealed() const { return holeCardRevealed_; }
    bool roundComplete() const { return result_ != BlackjackResult::InProgress; }
    BlackjackResult result() const { return result_; }
    const Hand& playerHand() const { return player_; }
    const Hand& dealerHand() const { return dealer_; }

    static int returnFor(BlackjackResult result, int wagerCents);

private:
    void beginDealerTurn();
    void settle(BlackjackResult result);
    void settleDealerHand();

    Deck deck_;
    Hand player_;
    Hand dealer_;
    int bankrollCents_ = kStartingBankrollCents;
    int wagerCents_ = 0;
    BlackjackResult result_ = BlackjackResult::NotStarted;
    bool playerTurn_ = false;
    bool dealerTurn_ = false;
    bool holeCardRevealed_ = false;
};

void playBlackjack();

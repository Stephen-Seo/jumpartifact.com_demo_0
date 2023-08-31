#ifndef JUMPARTIFACT_DOT_COM_DEMO_GAME_H_
#define JUMPARTIFACT_DOT_COM_DEMO_GAME_H_

// standard library includes
#include <chrono>

// local includes
#include "screen.h"

class Game {
 public:
  Game();

  // No copy.
  Game(const Game&) = delete;
  Game& operator=(const Game&) = delete;

  // Allow move.
  Game(Game&&) = default;
  Game& operator=(Game&&) = default;

  void update();
  void draw();

  void clear_screens();

 private:
  ScreenStack::Ptr screen_stack;
  std::chrono::steady_clock::time_point prev_time;
};

#endif

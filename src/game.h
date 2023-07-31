#ifndef JUMPARTIFACT_DOT_COM_DEMO_GAME_H_
#define JUMPARTIFACT_DOT_COM_DEMO_GAME_H_

// standard library includes
#include <chrono>

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

 private:
  void update_impl(float dt);

  std::chrono::steady_clock::time_point prev_time;
  float TEMP_cached_dt;
};

#endif

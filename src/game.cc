#include "game.h"

// local includes
#include "screen_test.h"

Game::Game()
    : screen_stack(ScreenStack::new_instance()),
      prev_time(std::chrono::steady_clock::now()) {}

void Game::update() {
  auto next_time = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      next_time - prev_time);
  prev_time = next_time;
  screen_stack->update(((float)duration.count()) / 1000000);
}

void Game::draw() { screen_stack->draw(); }

#include "game.h"

// local includes
#include "screen_trunner.h"

// third party includes
#include <raymath.h>

Game::Game()
    : screen_stack(ScreenStack::new_instance()),
      prev_time(std::chrono::steady_clock::now()) {
  screen_stack->push_constructing_screen<TRunnerScreen>();
}

void Game::update() {
  auto next_time = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      next_time - prev_time);
  prev_time = next_time;
  screen_stack->update(Clamp(((float)duration.count()) / 1000000, 0.0F, 1.0F));
}

void Game::draw() { screen_stack->draw(); }

void Game::clear_screens() { screen_stack->clear_screens(); }

void Game::clear_and_push_trunner() {
  screen_stack->clear_screens();
  screen_stack->push_constructing_screen<TRunnerScreen>();
}

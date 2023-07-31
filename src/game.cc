#include "game.h"

// standard library includes
#include <string>

// third party includes
#include <raylib.h>

Game::Game()
    : prev_time(std::chrono::steady_clock::now()), TEMP_cached_dt(0.0F) {}

void Game::update() {
  auto next_time = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      next_time - prev_time);
  prev_time = next_time;
  update_impl(((float)duration.count()) / 1000000);
}

void Game::draw() {
  std::string dt_string =
      std::string("Delta-time: ") + std::to_string(TEMP_cached_dt);

  BeginDrawing();
  ClearBackground(BLACK);
  DrawText("Testing...", 100, 100, 30, RAYWHITE);
  DrawText(dt_string.c_str(), 100, 140, 30, RAYWHITE);
  EndDrawing();
}

void Game::update_impl(float dt) { TEMP_cached_dt = dt; }

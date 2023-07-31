#include "game.h"

// third party includes
#include <raylib.h>

Game::Game() {}

void Game::update() {}

void Game::draw() {
  BeginDrawing();
  ClearBackground(BLACK);
  DrawText("Testing...", 100, 100, 30, RAYWHITE);
  EndDrawing();
}

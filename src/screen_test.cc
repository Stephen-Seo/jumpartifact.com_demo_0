#include "screen_test.h"

// standard library includes
#include <string>

// third party includes
#include <raylib.h>

TestScreen::TestScreen(ScreenStack::Weak weak_ptr)
    : Screen(weak_ptr), TEMP_cached_dt(0.0F) {}
TestScreen::~TestScreen() {}

bool TestScreen::update(float dt, bool) {
  TEMP_cached_dt = dt;
  return false;
}

bool TestScreen::draw(RenderTexture *render_texture) {
  std::string dt_string =
      std::string("Delta-time: ") + std::to_string(TEMP_cached_dt);

  BeginTextureMode(*render_texture);
  ClearBackground(BLACK);
  DrawText("Testing...", 100, 100, 30, RAYWHITE);
  DrawText(dt_string.c_str(), 100, 140, 30, RAYWHITE);
  EndTextureMode();
  return false;
}

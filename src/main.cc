// emscripten includes
#ifdef __EMSCRIPTEN__
#include "ems.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#else
#include <random>
#endif

// third party includes
#include <raylib.h>

// local includes

#ifdef __EMSCRIPTEN__
extern "C" {

EM_BOOL resize_event_callback(int event_type, const EmscriptenUiEvent *event,
                              void *ud) {
  if (event_type == EMSCRIPTEN_EVENT_RESIZE) {
    SetWindowSize(call_js_get_canvas_width(), call_js_get_canvas_height());
    //((GameRenderer *)ud)->screen_size_changed();
  }
  return false;
} // resize_event_callback(...)
} // extern "C"
#endif

// Main loop frame
void jumpartifact_demo_update(void *ud) {
  BeginDrawing();
  ClearBackground(BLACK);
  DrawText("Testing...", 100, 100, 30, RAYWHITE);
  EndDrawing();
}

int main() {
  InitWindow(800, 800, "Demo");
#ifdef __EMSCRIPTEN__
  SetWindowSize(call_js_get_canvas_width(), call_js_get_canvas_height());

  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, false,
                                 resize_event_callback);

  emscripten_set_main_loop_arg(jumpartifact_demo_update, 0, 0, 1);
#else
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    jumpartifact_demo_update(0);
  }

  CloseAudioDevice();
  CloseWindow();
#endif

  return 0;
}

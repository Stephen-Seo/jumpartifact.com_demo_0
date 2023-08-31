// emscripten includes
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>

#include "ems.h"
#else
#include <random>
#endif

// third party includes
#include <raylib.h>

// local includes
#include "game.h"

#ifdef __EMSCRIPTEN__

extern Game *global_game_ptr;

extern "C" {

EM_BOOL resize_event_callback(int event_type, const EmscriptenUiEvent *event,
                              void *ud) {
  if (event_type == EMSCRIPTEN_EVENT_RESIZE) {
    SetWindowSize(call_js_get_canvas_width(), call_js_get_canvas_height());
    //((GameRenderer *)ud)->screen_size_changed();
  }
  return false;
}  // resize_event_callback(...)

int EMSCRIPTEN_KEEPALIVE clear_all_screens() {
  if (global_game_ptr) {
    global_game_ptr->clear_screens();
    return 0;
  }
  return 1;
}

int EMSCRIPTEN_KEEPALIVE clear_and_push_trunner() {
  if (global_game_ptr) {
    global_game_ptr->clear_and_push_trunner();
    return 0;
  }
  return 1;
}

}  // extern "C"
#endif

// Main loop frame
void jumpartifact_demo_update(void *ud) {
  Game *game = (Game *)ud;

  game->update();
  game->draw();
}

int main() {
  InitWindow(800, 800, "Demo");

#ifdef NDEBUG
  SetTraceLogLevel(LOG_WARNING);
#endif

#ifdef __EMSCRIPTEN__
  Game game{};

  global_game_ptr = &game;

  SetWindowSize(call_js_get_canvas_width(), call_js_get_canvas_height());

  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, &game, false,
                                 resize_event_callback);

  emscripten_set_main_loop_arg(jumpartifact_demo_update, &game, 0, 1);
#else
  {
    Game game{};
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    while (!WindowShouldClose()) {
      jumpartifact_demo_update(&game);
    }
  }

  CloseAudioDevice();
  CloseWindow();
#endif

  return 0;
}

#include "ems.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <emscripten/html5.h>

EM_JS(int, canvas_get_width, (),
      { return document.getElementById("canvas").clientWidth; });

EM_JS(int, canvas_get_height, (),
      { return document.getElementById("canvas").clientHeight; });

int call_js_get_canvas_width() { return canvas_get_width(); }

int call_js_get_canvas_height() { return canvas_get_height(); }

#else
int call_js_get_canvas_width() { return 800; }

int call_js_get_canvas_height() { return 800; }
#endif

#include "common_constants.h"

// third-party includes.
#include <raylib.h>

// local includes.
#include "ems.h"

NeonColor get_random_neon_color() {
  return (NeonColor)(call_js_get_random() * (float)NeonColor::NEON_COLOR_SIZE);
}

void set_color_from_neon_color(NeonColor nc, Color *c_out) {
  if (!c_out) {
    return;
  }

  switch (nc) {
    case NeonColor::NEON_COLOR_RED:
      *c_out = Color{255, 0, 0, 255};
      break;
    case NeonColor::NEON_COLOR_GREEN:
      *c_out = Color{0, 255, 0, 255};
      break;
    case NeonColor::NEON_COLOR_BLUE:
      *c_out = Color{0, 0, 255, 255};
      break;
    case NeonColor::NEON_COLOR_CYAN:
      *c_out = Color{0, 255, 255, 255};
      break;
    case NeonColor::NEON_COLOR_YELLOW:
      *c_out = Color{255, 255, 0, 255};
      break;
    case NeonColor::NEON_COLOR_MAGENTA:
      *c_out = Color{255, 0, 255, 255};
      break;
    default:
      *c_out = Color{255, 255, 255, 255};
      break;
  }
}

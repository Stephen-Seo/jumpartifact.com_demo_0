#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_COMMON_CONSTANTS_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_COMMON_CONSTANTS_H_

// Forward declaration.
struct Color;

constexpr unsigned int SURFACE_UNIT_WIDTH = 51;
constexpr unsigned int SURFACE_UNIT_HEIGHT = 51;

constexpr float SURFACE_X_OFFSET = (float)SURFACE_UNIT_WIDTH / 2.0F - 0.5F;
constexpr float SURFACE_Y_OFFSET = (float)SURFACE_UNIT_HEIGHT / 2.0F - 0.5F;

enum class NeonColor {
  NEON_COLOR_RED = 0,
  NEON_COLOR_GREEN,
  NEON_COLOR_BLUE,
  NEON_COLOR_CYAN,
  NEON_COLOR_YELLOW,
  NEON_COLOR_MAGENTA,
  NEON_COLOR_SIZE
};

extern NeonColor get_random_neon_color();
extern void set_color_from_neon_color(NeonColor nc, Color *c_out);
extern void set_color_from_neon_random(Color *c_out);

#endif

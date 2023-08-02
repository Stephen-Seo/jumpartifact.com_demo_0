#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_TANK_RUNNER_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_TANK_RUNNER_H_

#include "screen.h"

// standard library includes
#include <array>
#include <bitset>

// third party includes
#include <raylib.h>

constexpr float GRID_SPACING_RATE = 1.0F;
constexpr float GRID_SPACING_OFFSET = 1.5F;

class TRunnerScreen : public Screen {
 public:
  TRunnerScreen(std::weak_ptr<ScreenStack> stack);
  ~TRunnerScreen() override;

  bool update(float dt) override;
  bool draw() override;

 private:
  enum Pixel : unsigned char {
    PIXEL_BLANK,
    PIXEL_BLACK,
    PIXEL_RED,
    PIXEL_GREEN,
    PIXEL_BLUE,
    PIXEL_YELLOW,
    PIXEL_CYAN,
    PIXEL_MAGENTA,
    PIXEL_ORANGE,
    PIXEL_BROWN,
    PIXEL_TURQUOISE,
    PIXEL_SKY,
    PIXEL_WHITE
  };

  static Color PixelToColor(Pixel p);

  const Camera3D camera;
  std::bitset<64> flags;
  Mesh TEMP_cube;
  Material TEMP_default_material;
  Matrix TEMP_matrix;
  Matrix TEMP_offset_matrix;
  float grid_spacing;
  float grid_spacing_modifier;
};

#endif

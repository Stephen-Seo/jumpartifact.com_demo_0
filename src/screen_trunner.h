#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_TANK_RUNNER_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_TANK_RUNNER_H_

#include "screen.h"

// standard library includes
#include <array>
#include <bitset>

// third party includes
#include <raylib.h>

constexpr float POS_VALUE_INC_RATE = 0.5F;
constexpr float CAMERA_UPDATE_RATE = 1.0F;

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

  Camera3D camera;
  std::bitset<64> flags;
  Material default_material;
  Model TEMP_cube_model;
  Mesh plane_mesh;
  Texture2D TEMP_cube_texture;
  Matrix TEMP_matrix;
  Vector3 camera_pos;
  Vector3 camera_target;
  float pos_value;

  void camera_to_targets(float dt);
};

#endif

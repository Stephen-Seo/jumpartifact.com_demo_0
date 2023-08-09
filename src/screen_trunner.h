#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_TANK_RUNNER_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_TANK_RUNNER_H_

#include "screen.h"

// standard library includes
#include <array>
#include <bitset>
#include <optional>

// third party includes
#include <raylib.h>

constexpr float POS_VALUE_INC_RATE = 0.2F;
constexpr float CAMERA_UPDATE_RATE = 1.0F;

constexpr unsigned int SURFACE_UNIT_WIDTH = 51;
constexpr unsigned int SURFACE_UNIT_HEIGHT = 51;

constexpr float SURFACE_X_OFFSET = (float)SURFACE_UNIT_WIDTH / 2.0F - 0.5F;
constexpr float SURFACE_Y_OFFSET = (float)SURFACE_UNIT_HEIGHT / 2.0F - 0.5F;

constexpr float SURFACE_HEIGHT_INTERVAL = 0.7F;

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

  struct SurfaceUnit {
    float nw, ne, sw, se;
  };

  static Color PixelToColor(Pixel p);

  std::array<std::optional<SurfaceUnit>,
             SURFACE_UNIT_WIDTH * SURFACE_UNIT_HEIGHT>
      surface;
  std::array<BoundingBox, SURFACE_UNIT_WIDTH * SURFACE_UNIT_HEIGHT> surface_bbs;
  Camera3D camera;
  std::bitset<64> flags;
  Model TEMP_cube_model;
  Texture2D TEMP_cube_texture;
  Matrix TEMP_matrix;
  Vector3 camera_pos;
  Vector3 camera_target;
  Vector3 mouse_hit;
  unsigned int idx_hit;

  void camera_to_targets(float dt);
};

#endif

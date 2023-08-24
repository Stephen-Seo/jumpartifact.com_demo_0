#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_TANK_RUNNER_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_TANK_RUNNER_H_

#include "screen.h"

// standard library includes
#include <array>
#include <bitset>
#include <memory>
#include <optional>

// third party includes
#include <raylib.h>

// local includes
#include "common_constants.h"
#include "electricity_effect.h"
#include "surface_triangle.h"
#include "walker.h"

constexpr float POS_VALUE_INC_RATE = 0.2F;
constexpr float CAMERA_UPDATE_RATE = 1.0F;

constexpr float SURFACE_HEIGHT_INTERVAL = 0.7F;

constexpr int BUTTON_FONT_SIZE = 30;

constexpr float SURFACE_RESET_TIME = 4.0F;
constexpr float SURFACE_RESET_TIME_TRI_DRAW = 3.0F;
constexpr float SURFACE_RESET_Y_OFFSET = 40.0F;

class TRunnerScreen : public Screen {
 public:
  struct SurfaceUnit {
    float nw, ne, sw, se;
  };

  TRunnerScreen(std::weak_ptr<ScreenStack> stack);
  ~TRunnerScreen() override;

  bool update(float dt, bool is_resized) override;
  bool draw(RenderTexture *render_texture) override;

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

  std::array<std::optional<SurfaceUnit>,
             SURFACE_UNIT_WIDTH * SURFACE_UNIT_HEIGHT>
      surface;
  std::array<BoundingBox, SURFACE_UNIT_WIDTH * SURFACE_UNIT_HEIGHT> surface_bbs;
  std::array<Walker, 4> walkers;

  Camera3D camera;
  /*
   * 0 - resetting surface
   * 1 - walker hack attempted
   */
  std::bitset<64> flags;
  Model TEMP_cube_model;
  Texture2D TEMP_cube_texture;
  Matrix TEMP_matrix;
  RenderTexture2D bgRenderTexture;
  RenderTexture2D fgRenderTexture;
  Vector3 camera_pos;
  Vector3 camera_target;
  Vector3 mouse_hit;
  std::unique_ptr<std::array<SurfaceTriangle,
                             SURFACE_UNIT_WIDTH * SURFACE_UNIT_HEIGHT * 2> >
      surface_triangles;
  std::vector<ElectricityEffect> electricityEffects;
  unsigned int idx_hit;
  std::optional<unsigned int> controlled_walker_idx;
  const int left_text_width;
  const int right_text_width;
  const int forward_text_width;
  const int reset_surface_text_width;
  float surface_reset_anim_timer;
  bool walker_hack_success;

  void camera_to_targets(float dt);
  void generate_surface();
  void generate_surface_with_triangles();
};

#endif

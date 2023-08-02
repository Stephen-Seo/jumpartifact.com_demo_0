#include "screen_trunner.h"

// standard library includes
#include <raylib.h>

#include <cassert>
#include <cmath>

// local includes
#include "3d_helpers.h"

TRunnerScreen::TRunnerScreen(std::weak_ptr<ScreenStack> stack)
    : Screen(stack),
      camera{Vector3{0.0F, 0.0F, 0.0F}, Vector3{0.0F, 0.0F, -1.0F},
             Vector3{0.0F, 1.0F, 0.0F}, 80.0F, CAMERA_PERSPECTIVE},
      flags(),
      TEMP_cube(GenMeshCube(2.0F, 2.0F, 2.0F)),
      TEMP_default_material(LoadMaterialDefault()),
      TEMP_matrix(get_identity_matrix()),
      TEMP_offset_matrix(translate_matrix_z(-4.0F)),
      grid_spacing(1.0F),
      grid_spacing_modifier(0.0F) {}

TRunnerScreen::~TRunnerScreen() {
  UnloadMesh(TEMP_cube);
  UnloadMaterial(TEMP_default_material);
}

bool TRunnerScreen::update(float dt) {
  //   grid_spacing_modifier += dt * GRID_SPACING_RATE;
  //   if (grid_spacing_modifier > 2.0F * PI) {
  //     grid_spacing_modifier -= 2.0F * PI;
  //   }
  //   grid_spacing = std::sin(grid_spacing_modifier) + GRID_SPACING_OFFSET;
  TEMP_matrix = get_rotation_matrix_about_z(dt) *
                get_rotation_matrix_about_y(dt * 1.2F) *
                get_rotation_matrix_about_x(dt * 1.4F) * TEMP_matrix;
  return false;
}

bool TRunnerScreen::draw() {
  BeginDrawing();
  ClearBackground(PixelToColor(Pixel::PIXEL_SKY));
  BeginMode3D(camera);

  DrawMesh(TEMP_cube, TEMP_default_material, TEMP_matrix * TEMP_offset_matrix);

  //  for (unsigned int i = 11; i-- > 1;) {
  //    // upper
  //    DrawRay(Ray{Vector3{0.0F, 2.0F, -(0.5F + grid_spacing * (float)i)},
  //                Vector3{1.0F, 0.0F, 0.0F}},
  //            BLACK);
  //    DrawRay(Ray{Vector3{0.0F, 2.0F, -(0.5F + grid_spacing * (float)i)},
  //                Vector3{-1.0F, 0.0F, 0.0F}},
  //            BLACK);
  //
  //    // lower
  //    DrawRay(Ray{Vector3{0.0F, -2.0F, -(0.5F + grid_spacing * (float)i)},
  //                Vector3{1.0F, 0.0F, 0.0F}},
  //            BLACK);
  //    DrawRay(Ray{Vector3{0.0F, -2.0F, -(0.5F + grid_spacing * (float)i)},
  //                Vector3{-1.0F, 0.0F, 0.0F}},
  //            BLACK);
  //
  //    // left
  //    DrawRay(Ray{Vector3{-2.0F, 0.0F, -(0.5F + grid_spacing * (float)i)},
  //                Vector3{0.0F, 1.0F, 0.0F}},
  //            BLACK);
  //    DrawRay(Ray{Vector3{-2.0F, 0.0F, -(0.5F + grid_spacing * (float)i)},
  //                Vector3{0.0F, -1.0F, 0.0F}},
  //            BLACK);
  //
  //    // right
  //    DrawRay(Ray{Vector3{2.0F, 0.0F, -(0.5F + grid_spacing * (float)i)},
  //                Vector3{0.0F, 1.0F, 0.0F}},
  //            BLACK);
  //    DrawRay(Ray{Vector3{2.0F, 0.0F, -(0.5F + grid_spacing * (float)i)},
  //                Vector3{0.0F, -1.0F, 0.0F}},
  //            BLACK);
  //  }
  EndMode3D();
  EndDrawing();

  return false;
}

Color TRunnerScreen::PixelToColor(Pixel p) {
  switch (p) {
    case PIXEL_BLANK:
      return Color{0, 0, 0, 0};
    case PIXEL_BLACK:
      return Color{0, 0, 0, 255};
    case PIXEL_RED:
      return Color{255, 50, 50, 255};
    case PIXEL_GREEN:
      return Color{50, 255, 50, 255};
    case PIXEL_BLUE:
      return Color{50, 50, 255, 255};
    case PIXEL_YELLOW:
      return Color{255, 255, 0, 255};
    case PIXEL_CYAN:
      return Color{0, 255, 255, 255};
    case PIXEL_MAGENTA:
      return Color{255, 0, 255, 255};
    case PIXEL_ORANGE:
      return Color{255, 200, 0, 255};
    case PIXEL_BROWN:
      return Color{180, 130, 0, 255};
    case PIXEL_TURQUOISE:
      return Color{0, 255, 200, 255};
    case PIXEL_SKY:
      return Color{168, 178, 255, 255};
    case PIXEL_WHITE:
      return Color{255, 255, 255, 255};
    default:
      assert(!"unreachable");
      return Color{0, 0, 0, 255};
  }
}

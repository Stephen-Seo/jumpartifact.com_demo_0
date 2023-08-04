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
      TEMP_cube_model(LoadModel("res/test_cube.obj")),
      TEMP_cube_texture(LoadTexture("res/test_cube_texture.png")),
      TEMP_matrix(get_identity_matrix()),
      TEMP_permanent_matrix(get_identity_matrix()),
      TEMP_value(0.0F),
      TEMP_current_state(0) {
  TEMP_cube_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture =
      TEMP_cube_texture;
}

TRunnerScreen::~TRunnerScreen() {
  UnloadTexture(TEMP_cube_texture);
  UnloadModel(TEMP_cube_model);
}

bool TRunnerScreen::update(float dt) {
  TEMP_value += TEMP_VALUE_INC_RATE * dt;
  if (TEMP_value < PI / 2.0F) {
    TEMP_matrix = get_rotation_matrix_about_z(TEMP_value);
  } else if (TEMP_value < PI) {
    if (TEMP_current_state == 0) {
      TEMP_current_state = 1;
      TEMP_permanent_matrix =
          TEMP_permanent_matrix * get_rotation_matrix_about_z(PI / 2.0F);
    } else if (TEMP_current_state != 1) {
      assert(!"unreachable");
    }
    TEMP_matrix = get_rotation_matrix_about_y(TEMP_value - PI / 2.0F);
  } else if (TEMP_value < PI * 3.0F / 2.0F) {
    if (TEMP_current_state == 1) {
      TEMP_current_state = 2;
      TEMP_permanent_matrix =
          TEMP_permanent_matrix * get_rotation_matrix_about_y(PI / 2.0F);
    } else if (TEMP_current_state != 2) {
      assert(!"unreachable");
    }
    TEMP_matrix = get_rotation_matrix_about_x(TEMP_value - PI);
  } else {
    if (TEMP_current_state == 2) {
      TEMP_current_state = 0;
      TEMP_permanent_matrix =
          TEMP_permanent_matrix * get_rotation_matrix_about_x(PI / 2.0F);
    } else {
      assert(!"unreachable");
    }
    TEMP_value -= PI * 3.0F / 2.0F;
    TEMP_matrix = get_rotation_matrix_about_z(TEMP_value);
  }
  return false;
}

bool TRunnerScreen::draw() {
  BeginDrawing();
  ClearBackground(PixelToColor(Pixel::PIXEL_SKY));
  BeginMode3D(camera);

  // DrawMesh(TEMP_cube, TEMP_default_material, TEMP_matrix *
  // TEMP_offset_matrix);
  DrawModel(Model{.transform = TEMP_cube_model.transform *
                               TEMP_permanent_matrix * TEMP_matrix,
                  .meshCount = TEMP_cube_model.meshCount,
                  .materialCount = TEMP_cube_model.materialCount,
                  .meshes = TEMP_cube_model.meshes,
                  .materials = TEMP_cube_model.materials,
                  .meshMaterial = TEMP_cube_model.meshMaterial,
                  .boneCount = TEMP_cube_model.boneCount,
                  .bones = TEMP_cube_model.bones,
                  .bindPose = TEMP_cube_model.bindPose},
            Vector3{0.0F, 0.0F, -4.0F}, 1.0F, WHITE);

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

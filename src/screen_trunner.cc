#include "screen_trunner.h"

// standard library includes
#include <cassert>
#include <cmath>

// local includes
#include "3d_helpers.h"
#include "ems.h"

TRunnerScreen::TRunnerScreen(std::weak_ptr<ScreenStack> stack)
    : Screen(stack),
      camera{Vector3{0.0F, 1.0F, 0.5F}, Vector3{0.0F, 0.0F, 0.0F},
             Vector3{0.0F, 1.0F, 0.0F}, 80.0F, CAMERA_PERSPECTIVE},
      flags(),
      default_material(LoadMaterialDefault()),
      TEMP_cube_model(LoadModel("res/test_cube.obj")),
      plane_mesh(GenMeshPlane(1.0F, 1.0F, 2, 2)),
      TEMP_cube_texture(LoadTexture("res/test_cube_texture.png")),
      TEMP_matrix(get_identity_matrix()),
      camera_pos{0.0F, 4.0F, 4.0F},
      camera_target{0.0F, 0.0F, 0.0F},
      pos_value(0.0F) {
  TEMP_cube_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture =
      TEMP_cube_texture;
  TEMP_cube_model.transform = TEMP_cube_model.transform *
                              scale_matrix_xyz(0.5F, 0.5F, 0.5F) *
                              translate_matrix_y(0.5F);
}

TRunnerScreen::~TRunnerScreen() {
  UnloadTexture(TEMP_cube_texture);
  UnloadMesh(plane_mesh);
  UnloadModel(TEMP_cube_model);
  UnloadMaterial(default_material);
}

bool TRunnerScreen::update(float dt) {
  pos_value += dt * POS_VALUE_INC_RATE;
  if (pos_value > PI * 2.0F) {
    pos_value -= PI * 2.0F;
  }
  camera_pos.x = std::sin(pos_value) * 4.0F;
  camera_pos.z = std::cos(pos_value) * 4.0F;
  camera_to_targets(dt);
  return false;
}

bool TRunnerScreen::draw() {
  BeginDrawing();
  ClearBackground(PixelToColor(Pixel::PIXEL_SKY));
  BeginMode3D(camera);

  for (int z = -25; z <= 25; ++z) {
    for (int x = -25; x <= 25; ++x) {
      if (x == 0 && z == 0) {
        default_material.maps[MATERIAL_MAP_DIFFUSE].color = RAYWHITE;
      } else {
        default_material.maps[MATERIAL_MAP_DIFFUSE].color =
            Color{(unsigned char)(200 + x * 2), (unsigned char)(150 + z * 2),
                  20, 255};
      }
      DrawMesh(plane_mesh, default_material, translate_matrix_xyz(x, 0.0F, z));
    }
  }

  DrawModel(Model{.transform = TEMP_cube_model.transform * TEMP_matrix,
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

void TRunnerScreen::camera_to_targets(float dt) {
  camera.position.x +=
      (camera_pos.x - camera.position.x) * CAMERA_UPDATE_RATE * dt;
  camera.position.y +=
      (camera_pos.y - camera.position.y) * CAMERA_UPDATE_RATE * dt;
  camera.position.z +=
      (camera_pos.z - camera.position.z) * CAMERA_UPDATE_RATE * dt;

  camera.target.x +=
      (camera_target.x - camera.target.x) * CAMERA_UPDATE_RATE * dt;
  camera.target.y +=
      (camera_target.y - camera.target.y) * CAMERA_UPDATE_RATE * dt;
  camera.target.z +=
      (camera_target.z - camera.target.z) * CAMERA_UPDATE_RATE * dt;
}

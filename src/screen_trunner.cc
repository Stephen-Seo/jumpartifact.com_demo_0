#include "screen_trunner.h"

// standard library includes
#include <cassert>
#include <cmath>
#include <queue>

#ifndef NDEBUG
#include <iostream>
#endif

// local includes
#include "3d_helpers.h"
#include "ems.h"

TRunnerScreen::TRunnerScreen(std::weak_ptr<ScreenStack> stack)
    : Screen(stack),
      surface(),
      camera{Vector3{0.0F, 1.0F, 0.5F}, Vector3{0.0F, 0.0F, 0.0F},
             Vector3{0.0F, 1.0F, 0.0F}, 80.0F, CAMERA_PERSPECTIVE},
      flags(),
      TEMP_cube_model(LoadModel("res/test_cube.obj")),
      TEMP_cube_texture(LoadTexture("res/test_cube_texture.png")),
      TEMP_matrix(get_identity_matrix()),
      camera_pos{0.0F, 4.0F, 4.0F},
      camera_target{0.0F, 0.0F, 0.0F},
      pos_value(0.0F),
      mouse_px(0),
      mouse_pz(0) {
  TEMP_cube_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture =
      TEMP_cube_texture;
  TEMP_cube_model.transform = TEMP_cube_model.transform *
                              scale_matrix_xyz(0.5F, 0.5F, 0.5F) *
                              translate_matrix_y(0.5F);

  // Initialize surface.
#ifndef NDEBUG
  std::cout << "Initializing surface...\n";
#endif
  std::queue<unsigned int> to_update;
  to_update.push(SURFACE_UNIT_WIDTH / 2 +
                 (SURFACE_UNIT_HEIGHT / 2) * SURFACE_UNIT_WIDTH);
  while (!to_update.empty()) {
    unsigned int idx = to_update.front();
    to_update.pop();
    if (surface.at(idx).has_value()) {
      continue;
    }
    SurfaceUnit current{0.0F, 0.0F, 0.0F, 0.0F};
    // 0000 0001 - nw set
    // 0000 0010 - ne set
    // 0000 0100 - sw set
    // 0000 1000 - se set
    unsigned char flags = 0;

    // Check adjacent.
    if (idx % SURFACE_UNIT_WIDTH > 0) {
      if (surface.at(idx - 1).has_value()) {
        if ((flags & 1) == 0) {
          current.nw = surface.at(idx - 1).value().ne;
          flags |= 1;
        }
        if ((flags & 4) == 0) {
          current.sw = surface.at(idx - 1).value().se;
          flags |= 4;
        }
      } else {
        to_update.push(idx - 1);
      }
    }
    if (idx % SURFACE_UNIT_WIDTH < SURFACE_UNIT_WIDTH - 1) {
      if (surface.at(idx + 1).has_value()) {
        if ((flags & 2) == 0) {
          current.ne = surface.at(idx + 1).value().nw;
          flags |= 2;
        }
        if ((flags & 8) == 0) {
          current.se = surface.at(idx + 1).value().sw;
          flags |= 8;
        }
      } else {
        to_update.push(idx + 1);
      }
    }
    if (idx / SURFACE_UNIT_WIDTH > 0) {
      if (surface.at(idx - SURFACE_UNIT_WIDTH).has_value()) {
        if ((flags & 1) == 0) {
          current.nw = surface.at(idx - SURFACE_UNIT_WIDTH).value().sw;
          flags |= 1;
        }
        if ((flags & 2) == 0) {
          current.ne = surface.at(idx - SURFACE_UNIT_WIDTH).value().se;
          flags |= 2;
        }
      } else {
        to_update.push(idx - SURFACE_UNIT_WIDTH);
      }
    }
    if (idx / SURFACE_UNIT_WIDTH < SURFACE_UNIT_HEIGHT - 1) {
      if (surface.at(idx + SURFACE_UNIT_WIDTH).has_value()) {
        if ((flags & 4) == 0) {
          current.sw = surface.at(idx + SURFACE_UNIT_WIDTH).value().nw;
          flags |= 4;
        }
        if ((flags & 8) == 0) {
          current.se = surface.at(idx + SURFACE_UNIT_WIDTH).value().ne;
          flags |= 8;
        }
      } else {
        to_update.push(idx + SURFACE_UNIT_WIDTH);
      }
    }

    // Calculate remaining values.
    float avg = 0.0F;
    unsigned int count = 0;
    if ((flags & 1) != 0) {
      avg += current.nw;
      ++count;
    }
    if ((flags & 2) != 0) {
      avg += current.ne;
      ++count;
    }
    if ((flags & 4) != 0) {
      avg += current.sw;
      ++count;
    }
    if ((flags & 8) != 0) {
      avg += current.se;
      ++count;
    }
    if (count != 0) {
      avg = avg / (float)count;
    }

    if ((flags & 1) == 0) {
      current.nw = avg + call_js_get_random() * SURFACE_HEIGHT_INTERVAL -
                   SURFACE_HEIGHT_INTERVAL / 2.0F;
    }
    if ((flags & 2) == 0) {
      current.ne = avg + call_js_get_random() * SURFACE_HEIGHT_INTERVAL -
                   SURFACE_HEIGHT_INTERVAL / 2.0F;
    }
    if ((flags & 4) == 0) {
      current.sw = avg + call_js_get_random() * SURFACE_HEIGHT_INTERVAL -
                   SURFACE_HEIGHT_INTERVAL / 2.0F;
    }
    if ((flags & 8) == 0) {
      current.se = avg + call_js_get_random() * SURFACE_HEIGHT_INTERVAL -
                   SURFACE_HEIGHT_INTERVAL / 2.0F;
    }

    surface.at(idx) = current;
  }

#ifndef NDEBUG
  std::cout << "Screen finished init.\n";
#endif
}

TRunnerScreen::~TRunnerScreen() {
  UnloadTexture(TEMP_cube_texture);
  UnloadModel(TEMP_cube_model);
}

bool TRunnerScreen::update(float dt) {
  pos_value += dt * POS_VALUE_INC_RATE;
  if (pos_value > PI * 2.0F) {
    pos_value -= PI * 2.0F;
  }
  camera_pos.x = std::sin(pos_value) * 4.0F;
  camera_pos.z = std::cos(pos_value) * 4.0F;
  camera_to_targets(dt);

  if (IsMouseButtonPressed(0)) {
    float press_x = GetTouchX();
    float press_y = GetTouchY();
    Ray ray = GetMouseRay(Vector2{press_x, press_y}, camera);
#ifndef NDEBUG
    std::cout << "X: " << press_x << ", Y: " << press_y << std::endl;
    std::cout << "Ray pos: " << ray.position.x << ", " << ray.position.y << ", "
              << ray.position.z << " Ray dir: " << ray.direction.x << ", "
              << ray.direction.y << ", " << ray.direction.z << std::endl;
#endif
    if (!ray_to_xz_plane(ray, mouse_px, mouse_pz)) {
      mouse_px = 0.0F;
      mouse_pz = 0.0F;
#ifndef NDEBUG
      std::cerr << "Ray intersected xz plane out of bounds!\n";
#endif
    }
  }

  return false;
}

bool TRunnerScreen::draw() {
  BeginDrawing();
  ClearBackground(PixelToColor(Pixel::PIXEL_SKY));
  BeginMode3D(camera);

  for (unsigned int idx = 0; idx < SURFACE_UNIT_WIDTH * SURFACE_UNIT_HEIGHT;
       ++idx) {
    int x = idx % SURFACE_UNIT_WIDTH;
    int y = idx / SURFACE_UNIT_WIDTH;
    int ox = x - SURFACE_UNIT_WIDTH / 2;
    int oy = y - SURFACE_UNIT_HEIGHT / 2;
    float xf = (float)(x)-SURFACE_X_OFFSET;
    float zf = (float)(y)-SURFACE_Y_OFFSET;
    Color color{(unsigned char)(200 + ox * 2), (unsigned char)(150 + oy * 2),
                20, 255};
    auto &current = surface[idx];
    DrawTriangle3D(Vector3{xf - 0.5F, current->nw, zf - 0.5F},
                   Vector3{xf - 0.5F, current->sw, zf + 0.5F},
                   Vector3{xf + 0.5F, current->ne, zf - 0.5F}, color);
    DrawTriangle3D(Vector3{xf + 0.5F, current->se, zf + 0.5F},
                   Vector3{xf + 0.5F, current->ne, zf - 0.5F},
                   Vector3{xf - 0.5F, current->sw, zf + 0.5F}, color);
  }

  // DrawModel(Model{.transform = TEMP_cube_model.transform * TEMP_matrix,
  //                 .meshCount = TEMP_cube_model.meshCount,
  //                 .materialCount = TEMP_cube_model.materialCount,
  //                 .meshes = TEMP_cube_model.meshes,
  //                 .materials = TEMP_cube_model.materials,
  //                 .meshMaterial = TEMP_cube_model.meshMaterial,
  //                 .boneCount = TEMP_cube_model.boneCount,
  //                 .bones = TEMP_cube_model.bones,
  //                 .bindPose = TEMP_cube_model.bindPose},
  //           Vector3{0.0F, 0.0F, -4.0F}, 1.0F, WHITE);

  // TODO DEBUG
  DrawLine3D(Vector3{0.0F, 3.0F, 0.0F}, Vector3{mouse_px, 0.0F, mouse_pz},
             BLACK);

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

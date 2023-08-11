#include "screen_trunner.h"

// standard library includes
#include <cassert>
#include <cmath>
#include <queue>

#ifndef NDEBUG
#include <iostream>
#endif

// third party includes
#include <raymath.h>

// local includes
#include "3d_helpers.h"
#include "ems.h"

TRunnerScreen::TRunnerScreen(std::weak_ptr<ScreenStack> stack)
    : Screen(stack),
      surface(),
      surface_bbs(),
      walkers{Walker{(float)(SURFACE_UNIT_WIDTH / 4) - SURFACE_X_OFFSET,
                     (float)(SURFACE_UNIT_HEIGHT / 4) - SURFACE_Y_OFFSET, true},

              Walker{(float)((SURFACE_UNIT_WIDTH / 4) * 3) - SURFACE_X_OFFSET,
                     (float)(SURFACE_UNIT_HEIGHT / 4) - SURFACE_Y_OFFSET, true},

              Walker{(float)(SURFACE_UNIT_WIDTH / 4) - SURFACE_X_OFFSET,
                     (float)((SURFACE_UNIT_HEIGHT / 4) * 3) - SURFACE_Y_OFFSET,
                     true},

              Walker{(float)((SURFACE_UNIT_WIDTH / 4) * 3) - SURFACE_X_OFFSET,
                     (float)((SURFACE_UNIT_HEIGHT / 4) * 3) - SURFACE_Y_OFFSET,
                     true}

      },
      camera{Vector3{0.0F, 1.0F, 0.5F}, Vector3{0.0F, 0.0F, 0.0F},
             Vector3{0.0F, 1.0F, 0.0F}, 80.0F, CAMERA_PERSPECTIVE},
      flags(),
      TEMP_cube_model(LoadModel("res/test_cube.obj")),
      TEMP_cube_texture(LoadTexture("res/test_cube_texture.png")),
      TEMP_matrix(get_identity_matrix()),
      camera_pos{0.0F, 4.0F, 4.0F},
      camera_target{0.0F, 0.0F, 0.0F},
      mouse_hit{0.0F, 0.0F, 0.0F},
      idx_hit(SURFACE_UNIT_WIDTH / 2 +
              (SURFACE_UNIT_HEIGHT / 2) * SURFACE_UNIT_WIDTH) {
#ifndef NDEBUG
  std::cout << "idx_hit initialized to " << idx_hit << std::endl;
#endif
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

    // Calculate bounding boxes.
    int x = idx % SURFACE_UNIT_WIDTH;
    int y = idx / SURFACE_UNIT_WIDTH;
    float xf = (float)(x)-SURFACE_X_OFFSET;
    float zf = (float)(y)-SURFACE_Y_OFFSET;
    surface_bbs.at(idx).min.x = xf - 0.5F;
    surface_bbs.at(idx).min.z = zf - 0.5F;
    surface_bbs.at(idx).max.x = xf + 0.5F;
    surface_bbs.at(idx).max.z = zf + 0.5F;

    surface_bbs.at(idx).min.y = current.nw;
    if (current.ne < surface_bbs.at(idx).min.y) {
      surface_bbs.at(idx).min.y = current.ne;
    }
    if (current.sw < surface_bbs.at(idx).min.y) {
      surface_bbs.at(idx).min.y = current.sw;
    }
    if (current.se < surface_bbs.at(idx).min.y) {
      surface_bbs.at(idx).min.y = current.se;
    }

    surface_bbs.at(idx).max.y = current.nw;
    if (current.ne > surface_bbs.at(idx).max.y) {
      surface_bbs.at(idx).max.y = current.ne;
    }
    if (current.sw > surface_bbs.at(idx).max.y) {
      surface_bbs.at(idx).max.y = current.sw;
    }
    if (current.se > surface_bbs.at(idx).max.y) {
      surface_bbs.at(idx).max.y = current.se;
    }
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

    for (unsigned int idx = 0; idx < SURFACE_UNIT_WIDTH * SURFACE_UNIT_HEIGHT;
         ++idx) {
      int x = idx % SURFACE_UNIT_WIDTH;
      int y = idx / SURFACE_UNIT_WIDTH;
      float xf = (float)(x)-SURFACE_X_OFFSET;
      float zf = (float)(y)-SURFACE_Y_OFFSET;

      const auto &current = surface[idx].value();
      Vector3 nw{xf - 0.5F, current.nw, zf - 0.5F};
      Vector3 ne{xf + 0.5F, current.ne, zf - 0.5F};
      Vector3 sw{xf - 0.5F, current.sw, zf + 0.5F};
      Vector3 se{xf + 0.5F, current.se, zf + 0.5F};

      const auto on_collide_fn = [this, idx, xf, zf,
                                  &current](const auto &collision) {
        this->idx_hit = idx;
#ifndef NDEBUG
        std::cout << "idx_hit set to " << idx_hit << std::endl;
#endif
        this->mouse_hit = collision;

        this->camera_target.x = xf;
        this->camera_target.y =
            (current.nw + current.ne + current.sw + current.se) / 4.0F;
        this->camera_target.z = zf;
        if (idx != SURFACE_UNIT_WIDTH / 2 +
                       (SURFACE_UNIT_HEIGHT / 2) * SURFACE_UNIT_WIDTH) {
          this->camera_pos = Vector3Add(
              Vector3Scale(Vector3Normalize(this->camera_target), 4.0F),
              this->camera_target);
          this->camera_pos.y += 4.0F;
        } else {
          this->camera_pos.x = 0.0F;
          this->camera_pos.y = this->camera_target.y + 4.0F;
          this->camera_pos.z = 0.0F;
        }
        this->camera_target.y += 1.0F;
      };

      if (auto bb_collision = GetRayCollisionBox(ray, surface_bbs[idx]);
          bb_collision.hit) {
        if (auto collision = GetRayCollisionTriangle(ray, nw, sw, ne);
            collision.hit) {
          on_collide_fn(collision.point);
          break;
        } else if (auto collision = GetRayCollisionTriangle(ray, ne, sw, se);
                   collision.hit) {
          on_collide_fn(collision.point);
          break;
        }
      }
    }
  }

  camera_to_targets(dt);

  for (auto &walker : walkers) {
    walker.update(dt, surface_bbs, SURFACE_UNIT_WIDTH, SURFACE_UNIT_HEIGHT);
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
    Color color = idx == idx_hit
                      ? RAYWHITE
                      : Color{(unsigned char)(200 + ox * 2),
                              (unsigned char)(150 + oy * 2), 20, 255};
    const auto &current = surface[idx].value();
    DrawTriangle3D(Vector3{xf - 0.5F, current.nw, zf - 0.5F},
                   Vector3{xf - 0.5F, current.sw, zf + 0.5F},
                   Vector3{xf + 0.5F, current.ne, zf - 0.5F}, color);
    DrawTriangle3D(Vector3{xf + 0.5F, current.se, zf + 0.5F},
                   Vector3{xf + 0.5F, current.ne, zf - 0.5F},
                   Vector3{xf - 0.5F, current.sw, zf + 0.5F}, color);
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

  for (auto &walker : walkers) {
    walker.draw(TEMP_cube_model);
  }

  // TODO DEBUG
  DrawLine3D(Vector3{0.0F, 3.0F, 0.0F}, mouse_hit, BLACK);

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

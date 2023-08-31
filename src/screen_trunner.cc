#include "screen_trunner.h"

// standard library includes
#include <cassert>
#include <cmath>
#include <queue>

#ifndef NDEBUG
#include <iostream>
#endif

// third party includes
#include <raylib.h>
#include <raymath.h>

// local includes
#include "3d_helpers.h"
#include "ems.h"
#include "screen_walker_hack.h"

TRunnerScreen::TRunnerScreen(std::weak_ptr<ScreenStack> stack)
    : Screen(stack),
      surface(),
      surface_bbs(),
      // NOLINTBEGIN(bugprone-integer-division)
      walkers{Walker{(float)(SURFACE_UNIT_WIDTH / 4) - SURFACE_X_OFFSET,
                     (float)(SURFACE_UNIT_HEIGHT / 4) - SURFACE_Y_OFFSET, true},

              Walker{(float)((SURFACE_UNIT_WIDTH / 4) * 3) - SURFACE_X_OFFSET,
                     (float)(SURFACE_UNIT_HEIGHT / 4) - SURFACE_Y_OFFSET, true},

              Walker{(float)(SURFACE_UNIT_WIDTH / 4) - SURFACE_X_OFFSET,
                     (float)((SURFACE_UNIT_HEIGHT / 4) * 3) - SURFACE_Y_OFFSET,
                     true},

              Walker{(float)((SURFACE_UNIT_WIDTH / 4) * 3) - SURFACE_X_OFFSET,
                     (float)((SURFACE_UNIT_HEIGHT / 4) * 3) - SURFACE_Y_OFFSET,
                     true}},
      // NOLINTEND(bugprone-integer-division)
      camera{Vector3{0.0F, 1.0F, 0.5F}, Vector3{0.0F, 0.0F, 0.0F},
             Vector3{0.0F, 1.0F, 0.0F}, 80.0F, CAMERA_PERSPECTIVE},
      flags(),
      TEMP_cube_model(LoadModel("res/test_cube.obj")),
      TEMP_cube_texture(LoadTexture("res/test_cube_texture.png")),
      TEMP_matrix(get_identity_matrix()),
      bgRenderTexture(),
      fgRenderTexture(),
      camera_pos{0.0F, 4.0F, 4.0F},
      camera_target{0.0F, 0.0F, 0.0F},
      mouse_hit{0.0F, 0.0F, 0.0F},
      surface_triangles(),
      electricityEffects(),
      sparkEffects(),
      idx_hit(SURFACE_UNIT_WIDTH / 2 +
              (SURFACE_UNIT_HEIGHT / 2) * SURFACE_UNIT_WIDTH),
      controlled_walker_idx(std::nullopt),
      left_text_width(MeasureText("Left", BUTTON_FONT_SIZE)),
      right_text_width(MeasureText("Right", BUTTON_FONT_SIZE)),
      forward_text_width(MeasureText("Forward", BUTTON_FONT_SIZE)),
      reset_surface_text_width(MeasureText("Reset Surface", BUTTON_FONT_SIZE)),
      surface_reset_anim_timer(0.0F),
      walker_hack_success(false) {
#ifndef NDEBUG
  std::cout << "idx_hit initialized to " << idx_hit << std::endl;
#endif
  TEMP_cube_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture =
      TEMP_cube_texture;
  TEMP_cube_model.transform = TEMP_cube_model.transform *
                              scale_matrix_xyz(0.5F, 0.5F, 0.5F) *
                              translate_matrix_y(0.5F);

  // Initialize surface.
  generate_surface();

  // Set up render textures.
  bgRenderTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
  fgRenderTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

  // Initialize ElectricityEffect shader.
  ElectricityEffect::update_shader_height();
  // Initialize SparkEffect shader.
  SparkEffect::update_shader_height();

#ifndef NDEBUG
  std::cout << "Screen finished init.\n";
#endif
}

TRunnerScreen::~TRunnerScreen() {
  SparkEffect::cleanup_shader();
  ElectricityEffect::cleanup_shader();

  UnloadRenderTexture(fgRenderTexture);
  UnloadRenderTexture(bgRenderTexture);

  UnloadTexture(TEMP_cube_texture);
  UnloadModel(TEMP_cube_model);
}

bool TRunnerScreen::update(float dt, bool is_resized) {
  if (is_resized) {
    UnloadRenderTexture(fgRenderTexture);
    UnloadRenderTexture(bgRenderTexture);

    bgRenderTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    fgRenderTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    ElectricityEffect::update_shader_height();
    SparkEffect::update_shader_height();
  }

  if (flags.test(1)) {
    if (walker_hack_success && controlled_walker_idx.has_value()) {
      walkers[controlled_walker_idx.value()].set_player_controlled(true);
      Color color;
      set_color_from_neon_random(&color);
      electricityEffects.push_back(ElectricityEffect(
          walkers[controlled_walker_idx.value()].get_body_pos(),
          ELECTRICITY_EFFECT_RADIUS, ELECTRICITY_EFFECT_LINE_COUNT,
          ELECTRICITY_EFFECT_LIFETIME, color));

      sparkEffects.push_back(
          SparkEffect(SPARK_EFFECT_SPARK_COUNT, SPARK_EFFECT_LIFETIME,
                      walkers[controlled_walker_idx.value()].get_body_pos(),
                      SPARK_EFFECT_XZ_VARIANCE, SPARK_EFFECT_RADIUS, color));
    } else {
      controlled_walker_idx.reset();
    }
    flags.reset(1);
  }

  if (controlled_walker_idx.has_value()) {
    auto walker_body_pos =
        walkers[controlled_walker_idx.value()].get_body_pos();

    camera_target = walker_body_pos + Vector3{0.0F, 1.0F, 0.0F};

    float rotation = walkers[controlled_walker_idx.value()].get_rotation();

    Vector3 offset = get_rotation_matrix_about_y(rotation + PI) *
                         Vector3{1.0F, 0.0F, 0.0F} * 4.0F +
                     Vector3{0.0F, 4.0F, 0.0F};

    camera_pos = walkers[controlled_walker_idx.value()].get_body_pos() + offset;
  }

  if (!flags.test(0)) {
    if (controlled_walker_idx.has_value() && IsMouseButtonDown(0)) {
      // Check if clicked on button.
      if (!walkers[controlled_walker_idx.value()].player_is_turning_left() &&
          GetTouchX() >= 0 && GetTouchX() <= left_text_width &&
          GetTouchY() >= GetScreenHeight() - BUTTON_FONT_SIZE &&
          GetTouchY() <= GetScreenHeight()) {
        walkers[controlled_walker_idx.value()].player_turn_left();
        goto post_check_click;
      } else if (!walkers[controlled_walker_idx.value()]
                      .player_is_turning_right() &&
                 GetTouchX() >= left_text_width &&
                 GetTouchX() <= left_text_width + right_text_width &&
                 GetTouchY() >= GetScreenHeight() - BUTTON_FONT_SIZE &&
                 GetTouchY() <= GetScreenHeight()) {
        walkers[controlled_walker_idx.value()].player_turn_right();
        goto post_check_click;
      } else if (!walkers[controlled_walker_idx.value()]
                      .player_is_going_forward()) {
        if (int width_mid = (left_text_width + right_text_width) / 2 -
                            forward_text_width / 2;
            GetTouchX() >= width_mid &&
            GetTouchX() <= width_mid + forward_text_width &&
            GetTouchY() >= GetScreenHeight() - BUTTON_FONT_SIZE * 2 &&
            GetTouchY() <= GetScreenHeight() - BUTTON_FONT_SIZE) {
          walkers[controlled_walker_idx.value()].player_go_forward();
          goto post_check_click;
        }
      }
    } else if (IsMouseButtonReleased(0)) {
      if (controlled_walker_idx.has_value()) {
        walkers[controlled_walker_idx.value()].player_idle();
        goto post_check_click;
      }
    }

    if (IsMouseButtonPressed(0)) {
      float press_x = GetTouchX();
      float press_y = GetTouchY();

      // Check if clicked on reset surface.
      if (!flags.test(0) &&
          press_x >= GetScreenWidth() - reset_surface_text_width &&
          press_y <= BUTTON_FONT_SIZE) {
        generate_surface_with_triangles();
        goto post_check_click;
      }

      Ray ray = GetMouseRay(Vector2{press_x, press_y}, camera);
#ifndef NDEBUG
      std::cout << "X: " << press_x << ", Y: " << press_y << std::endl;
      std::cout << "Ray pos: " << ray.position.x << ", " << ray.position.y
                << ", " << ray.position.z << " Ray dir: " << ray.direction.x
                << ", " << ray.direction.y << ", " << ray.direction.z
                << std::endl;
#endif

      // Check if clicked on a Walker.
      for (unsigned int idx = 0; idx < walkers.size(); ++idx) {
        if (GetRayCollisionBox(ray, walkers[idx].get_body_bb()).hit) {
          if (controlled_walker_idx.has_value()) {
            walkers[controlled_walker_idx.value()].set_player_controlled(false);
          }
          controlled_walker_idx = idx;
          auto s_stack = stack.lock();
          if (s_stack) {
            s_stack->push_constructing_screen_args<WalkerHackScreen>(
                &walker_hack_success);
            flags.set(1);
          }
          // walkers[controlled_walker_idx.value()].set_player_controlled(true);

          idx_hit = SURFACE_UNIT_WIDTH * SURFACE_UNIT_HEIGHT;

          goto post_check_click;
        }
      }

      // Check if clicked on ground.
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
            this->camera_pos = (Vector3Normalize(this->camera_target) * 4.0F) +
                               this->camera_target;
            this->camera_pos.y += 4.0F;
          } else {
            this->camera_pos.x = 0.0F;
            this->camera_pos.y = this->camera_target.y + 4.0F;
            this->camera_pos.z = 0.0F;
          }
          this->camera_target.y += 1.0F;
          if (this->controlled_walker_idx.has_value()) {
            this->walkers[this->controlled_walker_idx.value()]
                .set_player_controlled(false);
            this->controlled_walker_idx = std::nullopt;
          }
        };

        if (GetRayCollisionBox(ray, surface_bbs[idx]).hit) {
          if (auto collision = GetRayCollisionTriangle(ray, nw, sw, ne);
              collision.hit) {
            on_collide_fn(collision.point);
            goto post_check_click;
          } else if (auto collision = GetRayCollisionTriangle(ray, ne, sw, se);
                     collision.hit) {
            on_collide_fn(collision.point);
            goto post_check_click;
          }
        }
      }
    }
  }

post_check_click:

  if (flags.test(0)) {
    surface_reset_anim_timer += dt;
    if (surface_reset_anim_timer > SURFACE_RESET_TIME) {
      flags.reset(0);
      surface_triangles.reset();
    } else {
      for (auto &tri : *surface_triangles) {
        tri.update(dt);
      }
    }
  }

  camera_to_targets(dt);

  for (auto &walker : walkers) {
    walker.update(flags.test(0) ? 0.0F : dt, surface_bbs, SURFACE_UNIT_WIDTH,
                  SURFACE_UNIT_HEIGHT);
  }

  {
    std::vector<decltype(electricityEffects.size())> to_remove;
    for (decltype(electricityEffects.size()) idx = 0;
         idx < electricityEffects.size(); ++idx) {
      if (electricityEffects[idx].update(dt)) {
        to_remove.push_back(idx);
      }
    }

    for (auto iter = to_remove.rbegin(); iter != to_remove.rend(); ++iter) {
      if (*iter != electricityEffects.size() - 1) {
        electricityEffects[*iter] = *electricityEffects.rbegin();
      }
      electricityEffects.pop_back();
    }
  }

  {
    std::vector<decltype(sparkEffects.size())> to_remove;
    for (decltype(sparkEffects.size()) idx = 0; idx < sparkEffects.size();
         ++idx) {
      if (sparkEffects[idx].update(dt)) {
        to_remove.push_back(idx);
      }
    }

    for (auto iter = to_remove.rbegin(); iter != to_remove.rend(); ++iter) {
      if (*iter != sparkEffects.size() - 1) {
        sparkEffects[*iter] = *sparkEffects.rbegin();
      }
      sparkEffects.pop_back();
    }
  }

  return false;
}

bool TRunnerScreen::draw(RenderTexture *render_texture) {
  BeginTextureMode(bgRenderTexture);
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
    float reset_y_offset = 0.0F;
    if (flags.test(0)) {
      reset_y_offset = (1.0F - std::sin(surface_reset_anim_timer /
                                        SURFACE_RESET_TIME * PI / 2.0F)) *
                       -SURFACE_RESET_Y_OFFSET;
    }
    DrawTriangle3D(Vector3{xf - 0.5F, current.nw + reset_y_offset, zf - 0.5F},
                   Vector3{xf - 0.5F, current.sw + reset_y_offset, zf + 0.5F},
                   Vector3{xf + 0.5F, current.ne + reset_y_offset, zf - 0.5F},
                   color);
    DrawTriangle3D(Vector3{xf + 0.5F, current.se + reset_y_offset, zf + 0.5F},
                   Vector3{xf + 0.5F, current.ne + reset_y_offset, zf - 0.5F},
                   Vector3{xf - 0.5F, current.sw + reset_y_offset, zf + 0.5F},
                   color);
  }

  for (auto &walker : walkers) {
    walker.draw(TEMP_cube_model);
  }

  for (auto &ee : electricityEffects) {
    ee.draw(&camera);
  }

  for (auto &se : sparkEffects) {
    se.draw(&camera);
  }

  // TODO DEBUG
  if (!controlled_walker_idx.has_value() && !flags.test(0)) {
    DrawLine3D(Vector3{0.0F, 3.0F, 0.0F}, mouse_hit, BLACK);

    for (const auto &walker : walkers) {
      BoundingBox bb = walker.get_body_bb();
      bb.min = bb.min - Vector3{0.001F, 0.001F, 0.001F};
      bb.max = bb.max + Vector3{0.001F, 0.001F, 0.001F};
      DrawBoundingBox(bb, RED);
    }
  }

  EndMode3D();

  if (!flags.test(0)) {
    if (!flags.test(1) && controlled_walker_idx.has_value()) {
      int total_width = 0;
      DrawRectangle(0, GetScreenHeight() - BUTTON_FONT_SIZE, left_text_width,
                    BUTTON_FONT_SIZE, Color{255, 255, 255, 180});
      DrawRectangleLines(0, GetScreenHeight() - BUTTON_FONT_SIZE,
                         left_text_width, BUTTON_FONT_SIZE, GREEN);
      DrawText("Left", 0, GetScreenHeight() - BUTTON_FONT_SIZE,
               BUTTON_FONT_SIZE, BLACK);

      total_width += left_text_width;
      DrawRectangle(total_width, GetScreenHeight() - BUTTON_FONT_SIZE,
                    right_text_width, BUTTON_FONT_SIZE,
                    Color{255, 255, 255, 180});
      DrawRectangleLines(total_width, GetScreenHeight() - BUTTON_FONT_SIZE,
                         right_text_width, BUTTON_FONT_SIZE, GREEN);
      DrawText("Right", total_width, GetScreenHeight() - BUTTON_FONT_SIZE,
               BUTTON_FONT_SIZE, BLACK);

      total_width =
          (total_width + right_text_width) / 2 - forward_text_width / 2;
      DrawRectangle(total_width, GetScreenHeight() - BUTTON_FONT_SIZE * 2,
                    forward_text_width, BUTTON_FONT_SIZE,
                    Color{255, 255, 255, 180});
      DrawRectangleLines(total_width, GetScreenHeight() - BUTTON_FONT_SIZE * 2,
                         forward_text_width, BUTTON_FONT_SIZE, GREEN);
      DrawText("Forward", total_width, GetScreenHeight() - BUTTON_FONT_SIZE * 2,
               BUTTON_FONT_SIZE, BLACK);
    }

    DrawRectangle(GetScreenWidth() - reset_surface_text_width, 0,
                  reset_surface_text_width, BUTTON_FONT_SIZE,
                  Color{255, 255, 255, 180});
    DrawRectangleLines(GetScreenWidth() - reset_surface_text_width, 0,
                       reset_surface_text_width, BUTTON_FONT_SIZE, GREEN);
    DrawText("Reset Surface", GetScreenWidth() - reset_surface_text_width, 0,
             BUTTON_FONT_SIZE, BLACK);
  }

  EndTextureMode();

  if (flags.test(0)) {
    BeginTextureMode(fgRenderTexture);
    ClearBackground(Color{0, 0, 0, 0});
    BeginMode3D(camera);
    for (unsigned int idx = 0; idx < SURFACE_UNIT_WIDTH * SURFACE_UNIT_HEIGHT;
         ++idx) {
      int x = idx % SURFACE_UNIT_WIDTH;
      int y = idx / SURFACE_UNIT_WIDTH;
      int ox = x - SURFACE_UNIT_WIDTH / 2;
      int oy = y - SURFACE_UNIT_HEIGHT / 2;
      Color color = idx == idx_hit
                        ? RAYWHITE
                        : Color{(unsigned char)(200 + ox * 2),
                                (unsigned char)(150 + oy * 2), 20, 255};

      if (surface_reset_anim_timer < SURFACE_RESET_TIME_TRI_DRAW) {
        unsigned char alpha =
            ((1.0F - surface_reset_anim_timer / SURFACE_RESET_TIME_TRI_DRAW) *
             255.0F);
        surface_triangles->at(x * 2 + y * SURFACE_UNIT_WIDTH * 2)
            .draw(Color{color.r, color.g, color.b, alpha});
        surface_triangles->at(x * 2 + 1 + y * SURFACE_UNIT_WIDTH * 2)
            .draw(Color{color.r, color.g, color.b, alpha});
      }
    }
    EndMode3D();

    EndTextureMode();
  }

  BeginTextureMode(*render_texture);
  DrawTextureRec(
      bgRenderTexture.texture,
      Rectangle{0, 0, (float)GetScreenWidth(), (float)-GetScreenHeight()},
      {0, 0}, WHITE);
  if (flags.test(0)) {
    DrawTextureRec(
        fgRenderTexture.texture,
        Rectangle{0, 0, (float)GetScreenWidth(), (float)-GetScreenHeight()},
        {0, 0}, WHITE);
  }
  EndTextureMode();

  return true;
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

void TRunnerScreen::generate_surface() {
  for (auto &su : surface) {
    su = std::nullopt;
  }
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
}

void TRunnerScreen::generate_surface_with_triangles() {
  surface_triangles = surface_to_triangles(surface, SURFACE_UNIT_WIDTH);
  generate_surface();
  surface_reset_anim_timer = 0.0F;
  flags.set(0);
}

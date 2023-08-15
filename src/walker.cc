#include "walker.h"

// standard library includes
#include <cmath>

// third party includes
#include <raylib.h>
#include <raymath.h>

// local includes
#include "3d_helpers.h"
#include "ems.h"

Walker::Walker(float x, float z, bool auto_roaming, float body_height,
               float body_feet_radius, float feet_radius)
    : body_pos{x, body_height, z},
      target_body_pos{x, body_height, z},
      leg_nw(),
      leg_ne(),
      leg_sw(),
      leg_se(),
      target_leg_nw(),
      target_leg_ne(),
      target_leg_sw(),
      target_leg_se(),
      nw_flags(0x0),
      ne_flags(0x08),
      sw_flags(0x10),
      se_flags(0x18),
      flags(0),
      body_height(body_height),
      body_feet_radius(body_feet_radius),
      feet_radius(feet_radius),
      lift_start_y(0.0F),
      rotation(0.0F),
      target_rotation(0.0F),
      body_idle_move_timer(0.0F),
      roaming_time(5.0F),
      roaming_timer(0.0F) {
  flags |= auto_roaming ? 4 : 0;
  roaming_time =
      call_js_get_random() * ROAMING_WAIT_VARIANCE + ROAMING_WAIT_AMOUNT;

  const Vector3 nw = Vector3Normalize(Vector3{-1.0F, 0.0F, -1.0F});
  const Vector3 ne = Vector3Normalize(Vector3{1.0F, 0.0F, -1.0F});
  const Vector3 sw = Vector3Normalize(Vector3{-1.0F, 0.0F, 1.0F});
  const Vector3 se = Vector3Normalize(Vector3{1.0F, 0.0F, 1.0F});

  const Vector3 offset{x, 0.0F, z};

  leg_nw =
      offset +
      Vector3{(call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV, 0.0F,
              (call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV} +
      Vector3Scale(nw, body_feet_radius);
  target_leg_nw = leg_nw;
  leg_ne =
      offset +
      Vector3{(call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV, 0.0F,
              (call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV} +
      Vector3Scale(ne, body_feet_radius);
  target_leg_ne = leg_ne;
  leg_sw =
      offset +
      Vector3{(call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV, 0.0F,
              (call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV} +
      Vector3Scale(sw, body_feet_radius);
  target_leg_sw = leg_sw;
  leg_se =
      offset +
      Vector3{(call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV, 0.0F,
              (call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV} +
      Vector3Scale(se, body_feet_radius);
  target_leg_se = leg_se;
}

void Walker::draw(const Model &model) {
  const Matrix rotationMatrix = get_rotation_matrix_about_y(rotation);
  // draw body
  DrawModel(Model{.transform = model.transform * rotationMatrix,
                  .meshCount = model.meshCount,
                  .materialCount = model.materialCount,
                  .meshes = model.meshes,
                  .materials = model.materials,
                  .meshMaterial = model.meshMaterial,
                  .boneCount = model.boneCount,
                  .bones = model.bones,
                  .bindPose = model.bindPose},
            Vector3{body_pos.x,
                    body_pos.y + BODY_IDLE_MOVE_AMOUNT *
                                     std::sin(body_idle_move_timer + PI),
                    body_pos.z},
            1.0F, WHITE);

  // draw legs
  DrawModel(Model{.transform = model.transform * rotationMatrix,
                  .meshCount = model.meshCount,
                  .materialCount = model.materialCount,
                  .meshes = model.meshes,
                  .materials = model.materials,
                  .meshMaterial = model.meshMaterial,
                  .boneCount = model.boneCount,
                  .bones = model.bones,
                  .bindPose = model.bindPose},
            leg_nw, 1.0F, WHITE);
  DrawModel(Model{.transform = model.transform * rotationMatrix,
                  .meshCount = model.meshCount,
                  .materialCount = model.materialCount,
                  .meshes = model.meshes,
                  .materials = model.materials,
                  .meshMaterial = model.meshMaterial,
                  .boneCount = model.boneCount,
                  .bones = model.bones,
                  .bindPose = model.bindPose},
            leg_ne, 1.0F, WHITE);
  DrawModel(Model{.transform = model.transform * rotationMatrix,
                  .meshCount = model.meshCount,
                  .materialCount = model.materialCount,
                  .meshes = model.meshes,
                  .materials = model.materials,
                  .meshMaterial = model.meshMaterial,
                  .boneCount = model.boneCount,
                  .bones = model.bones,
                  .bindPose = model.bindPose},
            leg_sw, 1.0F, WHITE);
  DrawModel(Model{.transform = model.transform * rotationMatrix,
                  .meshCount = model.meshCount,
                  .materialCount = model.materialCount,
                  .meshes = model.meshes,
                  .materials = model.materials,
                  .meshMaterial = model.meshMaterial,
                  .boneCount = model.boneCount,
                  .bones = model.bones,
                  .bindPose = model.bindPose},
            leg_se, 1.0F, WHITE);
}

void Walker::set_body_pos(Vector3 pos) {
  if (!Vector3Equals(Vector3{target_body_pos.x, target_body_pos.y - body_height,
                             target_body_pos.z},
                     pos)) {
    target_body_pos = pos;
    target_body_pos.y += body_height;

    Vector3 direction = Vector3Subtract(pos, body_pos);
    target_rotation = std::atan2(-direction.z, direction.x);
    while (target_rotation < 0.0F) {
      target_rotation += PI * 2.0F;
    }
    while (target_rotation > PI * 2.0F) {
      target_rotation -= PI * 2.0F;
    }

    flags &= ~3;
    flags |= 1;
  }
}

void Walker::set_player_controlled(bool player_controlled) {
  if (player_controlled) {
    flags &= ~0x3B;
    flags |= 8;
    target_body_pos = body_pos;
    roaming_timer = 0.0F;
    roaming_time =
        call_js_get_random() * ROAMING_WAIT_VARIANCE + ROAMING_WAIT_AMOUNT;
  } else {
    flags &= ~0x38;
  }
}

void Walker::player_idle() { flags &= ~0x30; }

void Walker::player_turn_left() {
  flags &= ~0x30;
  flags |= 0x10;
  target_body_pos = body_pos;
}

void Walker::player_turn_right() {
  flags &= ~0x30;
  flags |= 0x20;
  target_body_pos = body_pos;
}

void Walker::player_go_forward() { flags |= 0x30; }

bool Walker::player_is_idle() const { return (flags & 0x30) == 0; }

bool Walker::player_is_turning_left() const { return (flags & 0x30) == 0x10; }

bool Walker::player_is_turning_right() const { return (flags & 0x30) == 0x20; }

bool Walker::player_is_going_forward() const { return (flags & 0x30) == 0x30; }

BoundingBox Walker::get_body_bb() const {
  return BoundingBox{
      .min = body_pos - Vector3{0.5F,
                                -BODY_IDLE_MOVE_AMOUNT *
                                    std::sin(body_idle_move_timer + PI),
                                0.5F},
      .max = body_pos + Vector3{0.5F,
                                1.0F + BODY_IDLE_MOVE_AMOUNT *
                                           std::sin(body_idle_move_timer + PI),
                                0.5F}};
}

float Walker::get_rotation() const { return rotation; }

Vector3 Walker::get_body_pos() const { return body_pos; }

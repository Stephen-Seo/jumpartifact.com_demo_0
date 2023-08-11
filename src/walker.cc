#include "walker.h"

// standard library includes
#include <cmath>

// third party includes
#include <raymath.h>

// local includes
#include "3d_helpers.h"
#include "ems.h"

Walker::Walker(float body_height, float body_feet_radius, float feet_radius)
    : body_pos{0.0F, body_height, 0.0F},
      target_body_pos{0.0F, body_height, 0.0F},
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
      body_idle_move_timer(0.0F) {
  const Vector3 nw = Vector3Normalize(Vector3{-1.0F, 0.0F, -1.0F});
  const Vector3 ne = Vector3Normalize(Vector3{1.0F, 0.0F, -1.0F});
  const Vector3 sw = Vector3Normalize(Vector3{-1.0F, 0.0F, 1.0F});
  const Vector3 se = Vector3Normalize(Vector3{1.0F, 0.0F, 1.0F});

  leg_nw = Vector3Add(
      Vector3{(call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV, 0.0F,
              (call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV},
      Vector3Scale(nw, body_feet_radius));
  target_leg_nw = leg_nw;
  leg_ne = Vector3Add(
      Vector3{(call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV, 0.0F,
              (call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV},
      Vector3Scale(ne, body_feet_radius));
  target_leg_ne = leg_ne;
  leg_sw = Vector3Add(
      Vector3{(call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV, 0.0F,
              (call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV},
      Vector3Scale(sw, body_feet_radius));
  target_leg_sw = leg_sw;
  leg_se = Vector3Add(
      Vector3{(call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV, 0.0F,
              (call_js_get_random() - 0.5F) / FEET_INIT_POS_VARIANCE_DIV},
      Vector3Scale(se, body_feet_radius));
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
  if (!Vector3Equals(target_body_pos, pos)) {
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

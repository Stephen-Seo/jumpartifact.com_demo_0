#include "surface_triangle.h"

// third party includes
#include <raylib.h>
#include <raymath.h>

// local includes
#include "3d_helpers.h"
#include "ems.h"

SurfaceTriangle::SurfaceTriangle()
    : triangle_coords{Vector3{0.5F, 0.0F, -0.5F}, Vector3{-0.5F, 0.0F, -0.5F},
                      Vector3{-0.5F, 0.0F, 0.5F}},
      triangle_pos{0.0F, 0.0F, 0.0F},
      rotate_axis{0.0F, 1.0F, 0.0F},
      pos_move_dir{0.0F, 1.0F, 0.0F},
      rotation(0.0F),
      move_amount(0.0F) {}

SurfaceTriangle::SurfaceTriangle(Vector3 a, Vector3 b, Vector3 c, Vector3 pos)
    : triangle_coords{a, b, c},
      triangle_pos{pos},
      rotate_axis{call_js_get_random() * 2.0F - 1.0F,
                  call_js_get_random() * 2.0F - 1.0F,
                  call_js_get_random() * 2.0F - 1.0F},
      pos_move_dir{call_js_get_random() * 2.0F - 1.0F,
                   call_js_get_random() * 2.0F - 1.0F,
                   call_js_get_random() * 2.0F - 1.0F},
      rotation(0.0F),
      move_amount(0.0F) {
  if (FloatEquals(rotate_axis.x, 0.0F) && FloatEquals(rotate_axis.x, 0.0F) &&
      FloatEquals(rotate_axis.x, 0.0F)) {
    rotate_axis.x = 1.0F;
  }
  if (FloatEquals(pos_move_dir.x, 0.0F) && FloatEquals(pos_move_dir.x, 0.0F) &&
      FloatEquals(pos_move_dir.x, 0.0F)) {
    pos_move_dir.x = 1.0F;
  }

  rotate_axis = Vector3Normalize(rotate_axis);
  pos_move_dir = Vector3Normalize(pos_move_dir);
}

void SurfaceTriangle::update(float dt) {
  rotation += dt * SURFACE_TRIANGLE_ROTATION_RATE;
  move_amount += dt * SURFACE_TRIANGLE_MOVE_RATE;
}

void SurfaceTriangle::draw(Color color) {
  const auto mat =
      MatrixRotate(rotate_axis, rotation) *
      MatrixTranslate(triangle_pos.x + pos_move_dir.x * move_amount,
                      triangle_pos.y + pos_move_dir.y * move_amount,
                      triangle_pos.z + pos_move_dir.z * move_amount);
  Vector3 a = mat * triangle_coords[0];
  Vector3 b = mat * triangle_coords[1];
  Vector3 c = mat * triangle_coords[2];

  DrawTriangle3D(a, b, c, color);
}

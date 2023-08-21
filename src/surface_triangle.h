#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_SURFACE_TRIANGLE_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_SURFACE_TRIANGLE_H_

// standard library includes
#include <array>

// third party includes
#include <raylib.h>

// local includes
#include "common_constants.h"

constexpr float SURFACE_TRIANGLE_ROTATION_RATE = 0.4F;
constexpr float SURFACE_TRIANGLE_MOVE_RATE = 1.0F;

struct SurfaceTriangle {
  SurfaceTriangle();
  SurfaceTriangle(Vector3 a, Vector3 b, Vector3 c, Vector3 pos);

  std::array<Vector3, 3> triangle_coords;
  Vector3 triangle_pos;
  Vector3 rotate_axis;
  Vector3 pos_move_dir;
  float rotation;
  float move_amount;

  void update(float dt);
  void draw(Color color);
};

template <typename SurfaceUnitOptT, std::size_t ASize>
extern std::array<SurfaceTriangle, ASize * 2> surface_to_triangles(
    const std::array<SurfaceUnitOptT, ASize> &surface,
    const std::size_t width) {
  std::array<SurfaceTriangle, ASize * 2> triangles;

  for (std::size_t idx = 0; idx < ASize; ++idx) {
    std::size_t x = idx % width;
    std::size_t y = idx / width;

    float posx = ((float)x) - SURFACE_X_OFFSET;
    float posz = ((float)y) - SURFACE_Y_OFFSET;

    std::size_t toffset = x * 2 + y * width * 2;

    if (!surface[idx].has_value()) {
      triangles.at(toffset) = SurfaceTriangle();
      triangles.at(toffset + 1) = SurfaceTriangle();
      continue;
    }

    const auto &surface_unit = surface[idx].value();
    triangles.at(toffset) = SurfaceTriangle(
        Vector3{0.5F, surface_unit.ne, -0.5F},
        Vector3{-0.5F, surface_unit.nw, -0.5F},
        Vector3{-0.5F, surface_unit.sw, 0.5F},
        Vector3{posx,
                (surface_unit.ne + surface_unit.nw + surface_unit.sw) / 3.0F,
                posz});
    triangles.at(toffset + 1) = SurfaceTriangle(
        Vector3{0.5F, surface_unit.ne, -0.5F},
        Vector3{-0.5F, surface_unit.sw, 0.5F},
        Vector3{0.5F, surface_unit.se, 0.5F},
        Vector3{posx,
                (surface_unit.ne + surface_unit.sw + surface_unit.se) / 3.0F,
                posz});
  }

  return triangles;
}

#endif

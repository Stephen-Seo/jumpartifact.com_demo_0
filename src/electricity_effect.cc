#include "electricity_effect.h"

// standard library includes
#include <queue>
#include <tuple>

// third party includes
#include <raylib.h>
#include <raymath.h>

// local includes
#include "3d_helpers.h"
#include "ems.h"

ElectricityEffect::ElectricityEffect(Vector3 center, float radius,
                                     int line_count, float lifetime)
    : end_points(),
      center(center),
      radius(radius),
      lifetime(lifetime),
      timer(0.0F) {
  end_points.reserve(line_count);

  const float line_max_length = radius * QUAD_LINE_MAX_LENGTH_RATIO;

  // Generate end_points.
  std::queue<std::tuple<Vector3, int>> positions;
  std::tuple<Vector3, int> next;
  Vector3 next_pos, dir;
  for (unsigned int idx = 0; idx < QUAD_SPLIT_COUNT && line_count > 0;
       ++idx, --line_count) {
    end_points.push_back(EndPoint{.next_idx = -1,
                                  .point = center,
                                  .mdir = Vector3Normalize(Vector3{
                                      call_js_get_random() * 2.0F - 1.0F,
                                      call_js_get_random() * 2.0F - 1.0F,
                                      call_js_get_random() * 2.0F - 1.0F,
                                  })});
    positions.push({center, end_points.size() - 1});
  }
  while (line_count-- > 0 && !positions.empty()) {
    next = positions.front();
    next_pos = std::get<Vector3>(next);
    positions.pop();

    dir = Vector3Normalize(center - next_pos);
    dir = Vector3Normalize(Vector3{call_js_get_random() * 2.0F - 1.0F,
                                   call_js_get_random() * 2.0F - 1.0F,
                                   call_js_get_random() * 2.0F - 1.0F} +
                           dir);

    auto coll = GetRayCollisionSphere(
        Ray{.position = next_pos, .direction = dir}, center, radius);

    if (coll.distance > line_max_length) {
      coll.point =
          next_pos + Vector3Normalize(coll.point - next_pos) * line_max_length;
    }

    end_points.push_back(EndPoint{
        .next_idx = std::get<int>(next),
        .point = coll.point,
        .mdir = Vector3Normalize(Vector3{call_js_get_random() * 2.0F - 1.0F,
                                         call_js_get_random() * 2.0F - 1.0F,
                                         call_js_get_random() * 2.0F - 1.0F})});

    dir = Vector3Normalize(center - coll.point);
    coll.point = coll.point + dir * (radius * QUAD_EDGE_OFFSET);
    for (unsigned int idx = 0; idx < QUAD_SPLIT_COUNT; ++idx) {
      positions.push({coll.point, end_points.size() - 1});
    }
  }
}

bool ElectricityEffect::update(float dt) {
  timer += dt;

  for (auto &end_point : end_points) {
    end_point.point = end_point.point + end_point.mdir * (dt * QUAD_MOVE_RATE);
    if (Vector3Distance(end_point.point, center) > radius) {
      end_point.point =
          end_point.point - end_point.mdir * (dt * QUAD_MOVE_RATE);
      end_point.mdir =
          from_edge_to_sphere_random(center, end_point.point, radius);
    }
  }

  return timer >= lifetime;
}

void ElectricityEffect::draw(Color color, Vector3 camera_pos) {
  float ratio = timer < lifetime ? (1.0F - timer / lifetime) : 0.0F;

  for (const auto &end_point : end_points) {
    if (end_point.next_idx >= 0) {
      std::array<Vector3, 4> quad = get_quad_from_start_end(
          end_point.point, end_points[end_point.next_idx].point,
          camera_pos - end_point.point, QUAD_MAX_WIDTH * ratio);
      DrawTriangle3D(quad[0], quad[1], quad[2], color);
      DrawTriangle3D(quad[0], quad[2], quad[3], color);
    }
  }
}

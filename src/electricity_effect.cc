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
    : cylinders(),
      center(center),
      radius(radius),
      lifetime(lifetime),
      timer(0.0F) {
  cylinders.reserve(line_count);

  const float line_max_length = radius * CYLINDER_LINE_MAX_LENGTH_RATIO;

  // Generate cylinders.
  std::queue<std::tuple<Vector3, int>> positions;
  std::tuple<Vector3, int> next;
  Vector3 next_pos, dir;
  for (unsigned int idx = 0; idx < CYLINDER_SPLIT_COUNT && line_count > 0;
       ++idx, --line_count) {
    cylinders.push_back(Cylinder{.next_idx = -1,
                                 .point = center,
                                 .mdir = Vector3Normalize(Vector3{
                                     call_js_get_random() * 2.0F - 1.0F,
                                     call_js_get_random() * 2.0F - 1.0F,
                                     call_js_get_random() * 2.0F - 1.0F,
                                 })});
    positions.push({center, cylinders.size() - 1});
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

    cylinders.push_back(Cylinder{
        .next_idx = std::get<int>(next),
        .point = coll.point,
        .mdir = Vector3Normalize(Vector3{call_js_get_random() * 2.0F - 1.0F,
                                         call_js_get_random() * 2.0F - 1.0F,
                                         call_js_get_random() * 2.0F - 1.0F})});

    dir = Vector3Normalize(center - coll.point);
    coll.point = coll.point + dir * (radius * CYLINDER_EDGE_OFFSET);
    for (unsigned int idx = 0; idx < CYLINDER_SPLIT_COUNT; ++idx) {
      positions.push({coll.point, cylinders.size() - 1});
    }
  }
}

bool ElectricityEffect::update(float dt) {
  timer += dt;

  for (auto &cylinder : cylinders) {
    cylinder.point = cylinder.point + cylinder.mdir * (dt * CYLINDER_MOVE_RATE);
    if (Vector3Distance(cylinder.point, center) > radius) {
      cylinder.point =
          cylinder.point - cylinder.mdir * (dt * CYLINDER_MOVE_RATE);
      cylinder.mdir =
          from_edge_to_sphere_random(center, cylinder.point, radius);
    }
  }

  return timer >= lifetime;
}

void ElectricityEffect::draw(Color color) {
  float ratio = timer < lifetime ? (1.0F - timer / lifetime) : 0.0F;

  for (const auto &cylinder : cylinders) {
    if (cylinder.next_idx >= 0) {
      DrawCylinderEx(cylinder.point, cylinders.at(cylinder.next_idx).point,
                     CYLINDER_MAX_RADIUS * ratio, CYLINDER_MAX_RADIUS * ratio,
                     CYLINDER_SIDES, color);
    }
  }
}

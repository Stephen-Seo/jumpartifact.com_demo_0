#include "electricity_effect.h"

// standard library includes
#include <queue>

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
      cylinder_radius(CYLINDER_MAX_RADIUS),
      lifetime(lifetime),
      timer(0.0F) {
  cylinders.reserve(line_count);

  // Generate cylinders.
  std::queue<Vector3> positions;
  Vector3 next, dir;
  for (unsigned int idx = 0; idx < CYLINDER_SPLIT_COUNT; ++idx) {
    positions.push(center);
  }
  while (line_count-- > 0 && !positions.empty()) {
    next = positions.front();
    positions.pop();

    dir = Vector3Normalize(Vector3{call_js_get_random() * 2.0F - 1.0F,
                                   call_js_get_random() * 2.0F - 1.0F,
                                   call_js_get_random() * 2.0F - 1.0F});

    auto coll = GetRayCollisionSphere(Ray{.position = next, .direction = dir},
                                      center, radius);

    cylinders.push_back(Cylinder{.start = next, .end = coll.point});

    dir = Vector3Normalize(center - coll.point);
    coll.point = coll.point + dir * (radius * CYLINDER_EDGE_OFFSET);
    for (unsigned int idx = 0; idx < CYLINDER_SPLIT_COUNT; ++idx) {
      positions.push(coll.point);
    }
  }
}

bool ElectricityEffect::update(float dt) {
  timer += dt;

  return timer >= lifetime;
}

void ElectricityEffect::draw(Color color) {
  float ratio = timer < lifetime ? (1.0F - timer / lifetime) : 0.0F;

  for (const auto &cylinder : cylinders) {
    DrawCylinderEx(cylinder.start, cylinder.end, CYLINDER_MAX_RADIUS * ratio,
                   CYLINDER_MAX_RADIUS * ratio, CYLINDER_SIDES, color);
  }
}

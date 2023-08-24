#include "electricity_effect.h"

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
  int sub_count = 0;
  Vector3 dir, pos;
  for (; line_count > 0; --line_count) {
    if (sub_count == 0) {
      dir.x = call_js_get_random() * 2.0F - 1.0F;
      dir.y = call_js_get_random() * 2.0F - 1.0F;
      dir.z = call_js_get_random() * 2.0F - 1.0F;

      dir = Vector3Normalize(dir);

      pos = center + dir * (radius * 0.7F);

      dir.x = call_js_get_random() * 2.0F - 1.0F;
      dir.y = call_js_get_random() * 2.0F - 1.0F;
      dir.z = call_js_get_random() * 2.0F - 1.0F;

      dir = Vector3Normalize(dir);

      auto coll = GetRayCollisionSphere(
          Ray{.position = center, .direction = dir}, center, radius);

      cylinders.push_back(Cylinder{.start = pos, .end = coll.point});

      pos = coll.point;
    } else {
      dir = Vector3Normalize(center - pos);

      pos = pos + dir * (radius * CYLINDER_EDGE_OFFSET);

      for (unsigned int idx = 0; idx < CYLINDER_SPLIT_COUNT; ++idx) {
        dir.x = call_js_get_random() * 2.0F - 1.0F;
        dir.y = call_js_get_random() * 2.0F - 1.0F;
        dir.z = call_js_get_random() * 2.0F - 1.0F;

        dir = Vector3Normalize(dir);

        auto coll = GetRayCollisionSphere(
            Ray{.position = pos, .direction = dir}, center, radius);

        cylinders.push_back(Cylinder{.start = pos, .end = coll.point});

        if (idx == CYLINDER_SPLIT_COUNT - 1) {
          pos = coll.point;
        }
      }
    }
    if (++sub_count >= CYLINDER_SUB_COUNT_MAX) {
      sub_count = 0;
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

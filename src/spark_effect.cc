#include "spark_effect.h"

// local includes
#include "3d_helpers.h"
#include "ems.h"

SparkEffect::SparkEffect(int count, float lifetime, Vector3 pos,
                         float pos_xz_variance, float radius)
    : sparks(), lifetime(lifetime), timer(0.0F) {
  sparks.reserve(count);

  Vector3 above_pos = pos;
  above_pos.y += radius;
  for (; count > 0; --count) {
    sparks.push_back(Spark{
        .pos = pos + Vector3{call_js_get_random() * pos_xz_variance * 2 -
                                 pos_xz_variance,
                             0.0F,
                             call_js_get_random() * pos_xz_variance * 2 -
                                 pos_xz_variance},
        .vel =
            from_edge_to_sphere_random(above_pos, pos, radius) *
            (SPARK_VEL_RATE + call_js_get_random() * SPARK_VEL_VARIANCE * 2.0F -
             SPARK_VEL_VARIANCE)});
  }
}

bool SparkEffect::update(float dt) {
  timer += dt;

  for (auto &spark : sparks) {
    spark.vel.y -= SPARK_ACC_RATE * dt;
    spark.pos = spark.pos + spark.vel * dt;
  }

  return timer > lifetime;
}

void SparkEffect::draw(Color color) {
  float ratio = timer < lifetime ? (1.0F - timer / lifetime) : 0.0F;

  for (const auto &spark : sparks) {
    DrawSphere(spark.pos, SPARK_RADIUS * ratio, color);
  }
}

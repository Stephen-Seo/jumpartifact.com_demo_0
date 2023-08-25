#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_SPARK_EFFECT_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_SPARK_EFFECT_H_

// standard library includes
#include <vector>

// third party includes
#include <raylib.h>

constexpr float SPARK_RADIUS = 0.03F;
constexpr float SPARK_VEL_RATE = 5.0F;
constexpr float SPARK_VEL_VARIANCE = 1.0F;
constexpr float SPARK_ACC_RATE = 10.0F;

class SparkEffect {
 public:
  SparkEffect(int count, float lifetime, Vector3 pos, float pos_xz_variance,
              float radius);

  /// Returns true if end of lifetime.
  bool update(float dt);

  /// Assumes draw mode is active when called.
  void draw(Color color);

 private:
  struct Spark {
    Vector3 pos, vel;
  };

  std::vector<Spark> sparks;
  float lifetime;
  float timer;
};

#endif

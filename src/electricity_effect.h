#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_ELECTRICITY_EFFECT_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_ELECTRICITY_EFFECT_H_

// standard library includes
#include <vector>

// third party includes
#include <raylib.h>

constexpr int CYLINDER_SUB_COUNT_MAX = 3;
constexpr int CYLINDER_SPLIT_COUNT = 3;
constexpr int CYLINDER_SIDES = 3;
constexpr float CYLINDER_MAX_RADIUS = 0.03F;
constexpr float CYLINDER_EDGE_OFFSET = 0.01F;

class ElectricityEffect {
 public:
  ElectricityEffect(Vector3 center, float radius, int line_count,
                    float lifetime);

  /// Returns true if lifetime ended.
  bool update(float dt);
  /// Assumes draw mode is active.
  void draw(Color color);

 private:
  struct Cylinder {
    Vector3 start, end;
  };

  std::vector<Cylinder> cylinders;
  Vector3 center;
  float cylinder_radius;
  float lifetime;
  float timer;
};

#endif

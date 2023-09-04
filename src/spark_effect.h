#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_SPARK_EFFECT_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_SPARK_EFFECT_H_

// standard library includes
#include <optional>
#include <vector>

// third party includes
#include <raylib.h>

constexpr float SPARK_RADIUS = 0.04F;
constexpr float SPARK_VEL_RATE = 5.0F;
constexpr float SPARK_VEL_VARIANCE = 1.0F;
constexpr float SPARK_ACC_RATE = 8.0F;

class SparkEffect {
 public:
  SparkEffect(int count, float lifetime, Vector3 pos, float pos_xz_variance,
              float radius, Color color);

  /// Returns true if end of lifetime.
  bool update(float dt);

  /// Assumes draw mode is active when called.
  void draw(Camera *camera);

  static Shader get_shader();
  static void cleanup_shader();
  static void update_shader_height();

 private:
  struct Spark {
    Vector3 pos, vel;
  };

  static std::optional<Shader> shader;
  static int uniform_screen_height;
  static int uniform_spark_radius;
  static int uniform_spark_pos;
  std::vector<Spark> sparks;
  Color color;
  float lifetime;
  float timer;

  static void update_shader_uniforms(float radius, Vector2 pos);
  static void init_shader();
};

#endif

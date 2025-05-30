#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_ELECTRICITY_EFFECT_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_ELECTRICITY_EFFECT_H_

// standard library includes
#include <optional>
#include <vector>

// third party includes
#include <raylib.h>

constexpr int QUAD_SPLIT_COUNT = 3;
constexpr float QUAD_MAX_WIDTH = 0.06F;
constexpr float QUAD_EDGE_OFFSET = 0.01F;
constexpr float QUAD_LINE_MAX_LENGTH_RATIO = 0.85F;
constexpr float QUAD_MOVE_RATE = 0.05F;

class ElectricityEffect {
 public:
  ElectricityEffect(Vector3 center, float radius, int line_count,
                    float lifetime, Color color);

  /// Returns true if lifetime ended.
  bool update(float dt);
  /// Assumes draw mode is active.
  void draw(Camera *camera);

  static Shader get_shader();
  static void cleanup_shader();
  static void update_shader_height();

 private:
  struct EndPoint {
    int next_idx;
    Vector3 point, mdir;
  };

  static std::optional<Shader> shader;
  static int uniform_screen_height;
  static int uniform_side_pos_a;
  static int uniform_side_dir_a;
  static int uniform_side_pos_b;
  static int uniform_side_dir_b;
  static int uniform_width;
  std::vector<EndPoint> end_points;
  Vector3 center;
  Color color;
  float radius;
  float lifetime;
  float timer;

  static void update_shader_sides(Vector2 a, Vector2 adir, Vector2 b,
                                  Vector2 bdir, float width);
  static void init_shader();
};

#endif

#include "spark_effect.h"

#include <raylib.h>
#include <raymath.h>

#ifndef NDEBUG
#define DEBUG_PRINT_VEC2(v2)                                           \
  do {                                                                 \
    std::cout << "Loading " << #v2 << " with value " << (v2).x << ", " \
              << (v2).y << std::endl;                                  \
  } while (false);
#define DEBUG_PRINT_FLOAT(f)                                             \
  do {                                                                   \
    std::cout << "Loading " << #f << " with value " << (f) << std::endl; \
  } while (false);
#else
#define DEBUG_PRINT_VEC2(v2)
#define DEBUG_PRINT_FLOAT(f)
#endif

// standard library includes
#ifndef NDEBUG
#include <iostream>
#endif

// local includes
#include "3d_helpers.h"
#include "ems.h"

std::optional<Shader> SparkEffect::shader = std::nullopt;

SparkEffect::SparkEffect(int count, float lifetime, Vector3 pos,
                         float pos_xz_variance, float radius, Color color)
    : sparks(), color(color), lifetime(lifetime), timer(0.0F) {
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

void SparkEffect::draw(Camera *camera) {
  float ratio = timer < lifetime ? (1.0F - timer / lifetime) : 0.0F;
  float radius = SPARK_RADIUS * ratio;

  for (const auto &spark : sparks) {
    auto circle = get_circle_facing_viewer(
        spark.pos, camera->position - spark.pos, radius);

    Vector2 screen_pos = GetWorldToScreen(spark.pos, *camera);
    Vector2 circle_edge_pos = GetWorldToScreen(circle[1], *camera);
    update_shader_uniforms(Vector2Distance(screen_pos, circle_edge_pos),
                           screen_pos);
    BeginShaderMode(get_shader());
    for (decltype(circle.size()) idx = 1; idx < circle.size() - 1; ++idx) {
      DrawTriangle3D(circle[0], circle[idx], circle[idx + 1], color);
    }
    DrawTriangle3D(circle[0], circle[circle.size() - 1], circle[1], color);
    EndShaderMode();
  }
}

Shader SparkEffect::get_shader() {
  if (!shader.has_value()) {
    init_shader();
  }

  return shader.value();
}

void SparkEffect::cleanup_shader() {
  if (shader.has_value()) {
    UnloadShader(shader.value());
    shader.reset();
  }
}

void SparkEffect::update_shader_height() {
  if (!shader.has_value()) {
    init_shader();
  }
  int uniform_loc = GetShaderLocation(get_shader(), "screen_height");
  float height = GetScreenHeight();
  DEBUG_PRINT_FLOAT(height);
  SetShaderValue(get_shader(), uniform_loc, &height, SHADER_UNIFORM_FLOAT);
}

void SparkEffect::update_shader_uniforms(float radius, Vector2 pos) {
  if (!shader.has_value()) {
    init_shader();
  }

  // radius *= 100.0F;

  int uniform_loc = GetShaderLocation(get_shader(), "spark_radius");
  DEBUG_PRINT_FLOAT(radius);
  SetShaderValue(get_shader(), uniform_loc, &radius, SHADER_UNIFORM_FLOAT);
  uniform_loc = GetShaderLocation(get_shader(), "spark_pos");
  DEBUG_PRINT_VEC2(pos);
  SetShaderValue(get_shader(), uniform_loc, &pos, SHADER_UNIFORM_VEC2);
}

void SparkEffect::init_shader() {
  // Set up spark shader.
  // Vertex shader is exactly the same as Raylib's default vertex shader.
  shader = LoadShaderFromMemory(
      // vertex
      "#version 100                       \n"
      "attribute vec3 vertexPosition;     \n"
      "attribute vec2 vertexTexCoord;     \n"
      "attribute vec4 vertexColor;        \n"
      "varying vec2 fragTexCoord;         \n"
      "varying vec4 fragColor;            \n"
      "uniform mat4 mvp;                  \n"
      "void main()                        \n"
      "{                                  \n"
      "    fragTexCoord = vertexTexCoord; \n"
      "    fragColor = vertexColor;       \n"
      "    gl_Position = mvp*vec4(vertexPosition, 1.0); \n"
      "}                                  \n",

      // fragment
      "#version 100                       \n"
      "precision mediump float;           \n"
      "varying vec2 fragTexCoord;         \n"
      "varying vec4 fragColor;            \n"
      "uniform sampler2D texture0;        \n"
      "uniform vec4 colDiffuse;           \n"
      "uniform float screen_height;       \n"
      "uniform float spark_radius;              \n"
      "uniform vec2 spark_pos;            \n"
      "void main()                        \n"
      "{                                  \n"
      "    vec4 texelColor = texture2D(texture0, fragTexCoord); \n"
      "    vec4 color = texelColor*colDiffuse*fragColor;      \n"
      "    if (spark_radius < 0.00001) {        \n"
      "        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);      \n"
      "        return;                    \n"
      "    }                              \n"
      "    vec2 pos = gl_FragCoord.xy;    \n"
      "    pos.y = screen_height - pos.y; \n"
      "    float dist = distance(pos, spark_pos); \n"
      "    float redge = spark_radius / 3.0; \n"
      "    float inv_redge = spark_radius * 2.0 / 3.0; \n"
      "    if (dist < redge) {     \n"
      "        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); \n"
      "    } else {                       \n"
      "        float lerpVal = min(1.0, (dist - redge) / inv_redge); \n"
      "        gl_FragColor = color * lerpVal + vec4(1.0, 1.0, 1.0, 1.0) * "
      "(1.0 - lerpVal); \n"
      "    }                              \n"
      "}                                  \n");
}

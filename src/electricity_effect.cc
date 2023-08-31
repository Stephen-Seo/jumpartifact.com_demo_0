#include "electricity_effect.h"

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
#include <queue>
#include <tuple>
#ifndef NDEBUG
#include <iostream>
#endif

// third party includes
#include <raylib.h>
#include <raymath.h>

// local includes
#include "3d_helpers.h"
#include "ems.h"

std::optional<Shader> ElectricityEffect::shader = std::nullopt;

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

  // Update shader height if not initialized.
  if (!shader.has_value()) {
    update_shader_height();
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

void ElectricityEffect::draw(Color color, Camera *camera) {
  float ratio = timer < lifetime ? (1.0F - timer / lifetime) : 0.0F;

  for (const auto &end_point : end_points) {
    if (end_point.next_idx >= 0) {
      std::array<Vector3, 4> quad = get_quad_from_start_end(
          end_point.point, end_points[end_point.next_idx].point,
          camera->position - end_point.point, QUAD_MAX_WIDTH * ratio);
      Vector2 q0 = GetWorldToScreen(quad[0], *camera);
      Vector2 q1 = GetWorldToScreen(quad[3], *camera);
      Vector2 q2 = GetWorldToScreen(quad[1], *camera);
      Vector2 q3 = GetWorldToScreen(quad[2], *camera);
      q1 = Vector2Subtract(q0, q1);
      q3 = Vector2Subtract(q2, q3);

      // Update shader values.
      update_shader_sides(q0, q1, q2, q3, Vector2Distance(q0, q2));

      // Draw with shader.
      BeginShaderMode(get_shader());
      DrawTriangle3D(quad[0], quad[1], quad[2], color);
      DrawTriangle3D(quad[0], quad[2], quad[3], color);
      EndShaderMode();
    }
  }
}

Shader ElectricityEffect::get_shader() {
  if (!shader.has_value()) {
    init_shader();
  }
  return shader.value();
}

void ElectricityEffect::cleanup_shader() {
  if (shader.has_value()) {
    UnloadShader(shader.value());
    shader.reset();
  }
}

void ElectricityEffect::update_shader_height() {
  if (!shader.has_value()) {
    init_shader();
  }
  int uniform_loc = GetShaderLocation(get_shader(), "screen_height");
  float height = GetScreenHeight();
  DEBUG_PRINT_FLOAT(height);
  SetShaderValue(get_shader(), uniform_loc, &height, SHADER_UNIFORM_FLOAT);
}

void ElectricityEffect::update_shader_sides(Vector2 a, Vector2 adir, Vector2 b,
                                            Vector2 bdir, float width) {
  if (!shader.has_value()) {
    init_shader();
  }
  int uniform_loc = GetShaderLocation(get_shader(), "sidePosA");
  DEBUG_PRINT_VEC2(a);
  SetShaderValue(get_shader(), uniform_loc, &a, SHADER_UNIFORM_VEC2);
  uniform_loc = GetShaderLocation(get_shader(), "sideDirA");
  DEBUG_PRINT_VEC2(adir);
  SetShaderValue(get_shader(), uniform_loc, &adir, SHADER_UNIFORM_VEC2);
  uniform_loc = GetShaderLocation(get_shader(), "sidePosB");
  DEBUG_PRINT_VEC2(b);
  SetShaderValue(get_shader(), uniform_loc, &b, SHADER_UNIFORM_VEC2);
  uniform_loc = GetShaderLocation(get_shader(), "sideDirB");
  DEBUG_PRINT_VEC2(bdir);
  SetShaderValue(get_shader(), uniform_loc, &bdir, SHADER_UNIFORM_VEC2);
  uniform_loc = GetShaderLocation(get_shader(), "width");
  DEBUG_PRINT_FLOAT(width);
  SetShaderValue(get_shader(), uniform_loc, &width, SHADER_UNIFORM_FLOAT);
}

void ElectricityEffect::init_shader() {
  // Set up electricity shader.
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
      "uniform float width;             \n"
      "uniform vec2 sidePosA;           \n"
      "uniform vec2 sideDirA;           \n"
      "uniform vec2 sidePosB;           \n"
      "uniform vec2 sideDirB;           \n"
      "float dot_get_alpha(vec2 pos, vec2 dir, vec2 point) { \n"
      "  return ((dir.x * point.x + dir.y * point.y)   \n"
      "             - (dir.x * pos.x + dir.y * pos.y)) \n"
      "           / (dir.x * dir.x + dir.y * dir.y);   \n"
      "}                                  \n"
      "vec2 closest_point(vec2 pos, vec2 dir, vec2 point) { \n"
      "  return pos + dir * dot_get_alpha(pos, dir, point); \n"
      "}                                  \n"
      "void main()                        \n"
      "{                                  \n"
      "    vec4 texelColor = texture2D(texture0, fragTexCoord); \n"
      "    vec4 color = texelColor*colDiffuse*fragColor;      \n"
      "    vec2 pos = gl_FragCoord.xy;    \n"
      "    pos.y = screen_height - pos.y; \n"
      "    vec2 closest = closest_point(sidePosA, sideDirA, pos); \n"
      "    float distA = distance(pos, closest); \n"
      "    closest = closest_point(sidePosB, sideDirB, pos); \n"
      "    float distB = distance(pos, closest); \n"
      "    float min_dist = width * 0.3;  \n"
      "    if (min_dist < 0.00001) {      \n"
      "        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); \n"
      "    } else if (distA < distB) {    \n"
      "        if (distA < min_dist) {    \n"
      "            float lerpVal = distA / min_dist; \n"
      "            gl_FragColor = color * (1.0 - lerpVal) \n"
      "                    + vec4(1.0, 1.0, 1.0, 1.0) * lerpVal; \n"
      "        } else {                   \n"
      "            gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); \n"
      "        }                          \n"
      "    } else if (distB < min_dist) { \n"
      "        float lerpVal = distB / min_dist; \n"
      "        gl_FragColor = color * (1.0 - lerpVal) \n"
      "                + vec4(1.0, 1.0, 1.0, 1.0) * lerpVal; \n"
      "    } else {                       \n"
      "        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); \n"
      "    }                              \n"
      "}                                  \n");
}

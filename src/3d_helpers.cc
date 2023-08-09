#include "3d_helpers.h"

// standard library includes
#include <cmath>

// third party includes
#include <raymath.h>

Matrix get_identity_matrix() {
  return Matrix{1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F,
                0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F};
}

// Rotation is
// cos, -sin,
// sin, cos
// probably?
//
// About z-axis:
// cos, -sin, 0, 0,
// sin,  cos, 0, 0,
// 0,    0,   1, 0,
// 0,    0,   0, 1

Matrix get_rotation_matrix_about_z(float radians) {
  // OpenGL is column-major but testing shows that not transposing works.
  return Matrix{std::cos(radians),
                -std::sin(radians),
                0.0F,
                0.0F,
                std::sin(radians),
                std::cos(radians),
                0.0F,
                0.0F,
                0.0F,
                0.0F,
                1.0F,
                0.0F,
                0.0F,
                0.0F,
                0.0F,
                1.0F};
}

// About y-axis
//  cos, 0, sin, 0,
//  0,   1, 0,   0,
// -sin, 0, cos, 0,
//  0,   0, 0,   1

Matrix get_rotation_matrix_about_y(float radians) {
  // OpenGL is column-major but testing shows that not transposing works.
  return Matrix{std::cos(radians),
                0.0F,
                std::sin(radians),
                0.0F,
                0.0F,
                1.0F,
                0.0F,
                0.0F,
                -std::sin(radians),
                0.0F,
                std::cos(radians),
                0.0F,
                0.0F,
                0.0F,
                0.0F,
                1.0F};
}

// About x-axis
// 1, 0,    0,   0
// 0, cos, -sin, 0
// 0, sin,  cos, 0
// 0, 0,    0,   1

Matrix get_rotation_matrix_about_x(float radians) {
  // OpenGL is column-major but testing shows that not transposing works.
  return Matrix{1.0F,
                0.0F,
                0.0F,
                0.0F,
                0.0F,
                std::cos(radians),
                -std::sin(radians),
                0.0F,
                0.0F,
                std::sin(radians),
                std::cos(radians),
                0.0F,
                0.0F,
                0.0F,
                0.0F,
                1.0F};
}

Matrix translate_matrix_x(float distance) {
  // OpenGL is column-major but testing shows that not transposing works.
  return Matrix{1.0F, 0.0F, 0.0F, distance, 0.0F, 1.0F, 0.0F, 0.0F,
                0.0F, 0.0F, 1.0F, 0.0F,     0.0F, 0.0F, 0.0F, 1.0F};
}

Matrix translate_matrix_y(float distance) {
  // OpenGL is column-major but testing shows that not transposing works.
  return Matrix{1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, distance,
                0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F};
}

Matrix translate_matrix_z(float distance) {
  // OpenGL is column-major but testing shows that not transposing works.
  return Matrix{1.0F, 0.0F, 0.0F, 0.0F,     0.0F, 1.0F, 0.0F, 0.0F,
                0.0F, 0.0F, 1.0F, distance, 0.0F, 0.0F, 0.0F, 1.0F};
}

Matrix translate_matrix_xyz(float x, float y, float z) {
  return Matrix{1.0F, 0.0F, 0.0F, x, 0.0F, 1.0F, 0.0F, y,
                0.0F, 0.0F, 1.0F, z, 0.0F, 0.0F, 0.0F, 1.0F};
}

Matrix scale_matrix_xyz(float x, float y, float z) {
  return Matrix{x,    0.0F, 0.0F, 0.0F, 0.0F, y,    0.0F, 0.0F,
                0.0F, 0.0F, z,    0.0F, 0.0F, 0.0F, 0.0F, 1.0F};
}

Matrix operator*(const Matrix &a, const Matrix &b) {
  return MatrixMultiply(a, b);
}

bool ray_to_xz_plane(const Ray &ray, float &x_out, float &z_out) {
  if (ray.direction.y == 0.0F) {
    return false;
  }
  // y = 0 -> amount to set ray.dir to set ray.pos to zero
  // 0 = py + dy * amount
  // amount = -py / dy
  float amount = -ray.position.y / ray.direction.y;

  // x = px + dx * amount
  x_out = ray.position.x + ray.direction.x * amount;

  // z = pz + dz * amount
  z_out = ray.position.z + ray.direction.z * amount;

  return true;
}

std::optional<Vector3> ray_to_plane(const Ray &ray, const Ray &plane) {
  // Ray dir and plane normal.
  float rd_pn = Vector3DotProduct(ray.direction, plane.direction);
  if (rd_pn == 0.0F) {
    return std::nullopt;
  }

  float amount = (Vector3DotProduct(plane.position, plane.direction) -
                  Vector3DotProduct(ray.position, plane.direction)) /
                 rd_pn;

  // Amount * ray_dir + ray_pos == plane intersection.
  return Vector3{ray.position.x + ray.direction.x * amount,
                 ray.position.y + ray.direction.y * amount,
                 ray.position.z + ray.direction.z * amount};
}

std::optional<Vector3> ray_collision_triangle(const Ray &ray, const Vector3 &a,
                                              const Vector3 &b,
                                              const Vector3 &c) {
  const Vector3 ab = Vector3Subtract(a, b);
  const Vector3 cb = Vector3Subtract(c, b);
  Vector3 ortho = Vector3CrossProduct(ab, cb);
  ortho = Vector3Normalize(ortho);

  Ray plane{.position = b, .direction = ortho};

  auto result = ray_to_plane(ray, plane);
  if (!result.has_value()) {
    return std::nullopt;
  }

  Vector3 bary = Vector3Barycenter(result.value(), a, b, c);
  if (bary.x >= 0.0F && bary.x <= 1.0F && bary.y >= 0.0F && bary.y <= 1.0F &&
      bary.z >= 0.0F && bary.z <= 1.0F) {
    float sum = bary.x + bary.y + bary.z;
    if (sum > 0.999F && sum < 1.001) {
      return result;
    }
  }

  return std::nullopt;
}

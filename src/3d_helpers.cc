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
  if (FloatEquals(ray.direction.y, 0.0F)) {
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
  if (FloatEquals(rd_pn, 0.0F)) {
    return std::nullopt;
  }

  float amount = (Vector3DotProduct(plane.position, plane.direction) -
                  Vector3DotProduct(ray.position, plane.direction)) /
                 rd_pn;

  // Negative amount means collision in opposite direction of ray.
  if (amount < 0.0F) {
    return std::nullopt;
  }

  // Amount * ray_dir + ray_pos == plane intersection.
  return Vector3{ray.position.x + ray.direction.x * amount,
                 ray.position.y + ray.direction.y * amount,
                 ray.position.z + ray.direction.z * amount};
}

Vector3 operator+(const Vector3 &a, const Vector3 &b) {
  return Vector3{a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3 operator-(const Vector3 &a, const Vector3 &b) {
  return Vector3{a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 operator*(Vector3 vec3, float factor) {
  return Vector3Scale(vec3, factor);
}

Vector3 operator*(Matrix mat, Vector3 vec3) {
  return Vector3Transform(vec3, mat);
}

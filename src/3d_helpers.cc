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

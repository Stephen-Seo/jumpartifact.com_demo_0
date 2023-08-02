#include "3d_helpers.h"

// standard library includes
#include <cmath>

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
  // OpenGL is column-major.
  return Matrix{std::cos(radians),
                std::sin(radians),
                0.0F,
                0.0F,
                -std::sin(radians),
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
  // OpenGL is column-major.
  return Matrix{std::cos(radians),
                0.0F,
                -std::sin(radians),
                0.0F,
                0.0F,
                1.0F,
                0.0F,
                0.0F,
                std::sin(radians),
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
  // OpenGL is column-major.
  return Matrix{1.0F,
                0.0F,
                0.0F,
                0.0F,
                0.0F,
                std::cos(radians),
                std::sin(radians),
                0.0F,
                0.0F,
                -std::sin(radians),
                std::cos(radians),
                0.0F,
                0.0F,
                0.0F,
                0.0F,
                1.0F};
}

Matrix translate_matrix_z(float distance) {
  return Matrix{1.0F, 0.0F, 0.0F, 0.0F,     0.0F, 1.0F, 0.0F, 0.0F,
                0.0F, 0.0F, 1.0F, distance, 0.0F, 0.0F, 0.0F, 1.0F};
}

Matrix operator*(const Matrix &a, const Matrix &b) {
  return Matrix{a.m0 * b.m0 + a.m1 * b.m4 + a.m2 * b.m8 + a.m3 * b.m12,
                a.m4 * b.m0 + a.m5 * b.m4 + a.m6 * b.m8 + a.m7 * b.m12,
                a.m8 * b.m0 + a.m9 * b.m4 + a.m10 * b.m8 + a.m11 * b.m12,
                a.m12 * b.m0 + a.m13 * b.m4 + a.m14 * b.m8 + a.m15 * b.m12,

                a.m0 * b.m1 + a.m1 * b.m5 + a.m2 * b.m9 + a.m3 * b.m13,
                a.m4 * b.m1 + a.m5 * b.m5 + a.m6 * b.m9 + a.m7 * b.m13,
                a.m8 * b.m1 + a.m9 * b.m5 + a.m10 * b.m9 + a.m11 * b.m13,
                a.m12 * b.m1 + a.m13 * b.m5 + a.m14 * b.m9 + a.m15 * b.m13,

                a.m0 * b.m2 + a.m1 * b.m6 + a.m2 * b.m10 + a.m3 * b.m14,
                a.m4 * b.m2 + a.m5 * b.m6 + a.m6 * b.m10 + a.m7 * b.m14,
                a.m8 * b.m2 + a.m9 * b.m6 + a.m10 * b.m10 + a.m11 * b.m14,
                a.m12 * b.m2 + a.m13 * b.m6 + a.m14 * b.m10 + a.m15 * b.m14,

                a.m0 * b.m3 + a.m1 * b.m7 + a.m2 * b.m11 + a.m3 * b.m15,
                a.m4 * b.m3 + a.m5 * b.m7 + a.m6 * b.m11 + a.m7 * b.m15,
                a.m8 * b.m3 + a.m9 * b.m7 + a.m10 * b.m11 + a.m11 * b.m15,
                a.m12 * b.m3 + a.m13 * b.m7 + a.m14 * b.m11 + a.m15 * b.m15};
}

Vector4 operator*(const Matrix &m, const Vector4 &v) {
  return Vector4{
      m.m0 * v.x + m.m1 * v.y + m.m2 * v.z + m.m3 * v.w,
      m.m4 * v.x + m.m5 * v.y + m.m6 * v.z + m.m7 * v.w,
      m.m8 * v.x + m.m9 * v.y + m.m10 * v.z + m.m11 * v.w,
      m.m12 * v.x + m.m13 * v.y + m.m14 * v.z + m.m15 * v.w,
  };
}

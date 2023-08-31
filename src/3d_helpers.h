#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_3D_HELPERS_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_3D_HELPERS_H_

// standard library includes
#include <array>
#include <optional>

// third party includes
#include <raylib.h>

extern Matrix get_identity_matrix();
extern Matrix get_rotation_matrix_about_z(float radians);
extern Matrix get_rotation_matrix_about_y(float radians);
extern Matrix get_rotation_matrix_about_x(float radians);

extern Matrix translate_matrix_x(float distance);
extern Matrix translate_matrix_y(float distance);
extern Matrix translate_matrix_z(float distance);
extern Matrix translate_matrix_xyz(float x, float y, float z);

extern Matrix scale_matrix_xyz(float x, float y, float z);

extern Matrix operator*(const Matrix &a, const Matrix &b);

/// Returns true if intersects with xz plane.
extern bool ray_to_xz_plane(const Ray &ray, float &x_out, float &z_out);

/// plane.direction is plane normal, plane.position is position on plane.
extern std::optional<Vector3> ray_to_plane(const Ray &ray, const Ray &plane);

extern Vector3 from_edge_to_sphere_random(Vector3 center, Vector3 point,
                                          float radius);

/*
 * start side
 * b -- a
 * |    |
 * |    |
 * c -- d
 * end side
 *
 * (Normal facing outwards from screen.)
 */
extern std::array<Vector3, 4> get_quad_from_start_end(Vector3 start,
                                                      Vector3 end,
                                                      Vector3 normal,
                                                      float width);

// Unimplemented as this function isn't really needed and it exposes some
// weirdness regarding column-major matrices.
// extern Vector4 operator*(const Matrix &m, const Vector4 &v);

extern Vector3 operator+(Vector3 a, Vector3 b);
extern Vector3 operator-(Vector3 a, Vector3 b);

extern Vector3 operator*(Vector3 vec3, float factor);

extern Vector3 operator*(Matrix mat, Vector3 vec3);

extern Vector4 operator*(Vector4 a, Vector4 b);

#endif

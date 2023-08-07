#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_3D_HELPERS_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_3D_HELPERS_H_

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

// Unimplemented as this function isn't really needed and it exposes some
// weirdness regarding column-major matrices.
// extern Vector4 operator*(const Matrix &m, const Vector4 &v);

#endif

#ifndef JUMPARTIFACT_DOT_COM_DEMO_0_3D_HELPERS_H_
#define JUMPARTIFACT_DOT_COM_DEMO_0_3D_HELPERS_H_

// third party includes
#include <raylib.h>

extern Matrix get_identity_matrix();
extern Matrix get_rotation_matrix_about_z(float radians);
extern Matrix get_rotation_matrix_about_y(float radians);
extern Matrix get_rotation_matrix_about_x(float radians);

extern Matrix translate_matrix_z(float distance);

extern Matrix operator*(const Matrix &a, const Matrix &b);
extern Vector4 operator*(const Matrix &m, const Vector4 &v);

#endif

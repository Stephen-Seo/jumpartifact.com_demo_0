// standard library includes
#include <iostream>

// local includes
#include "../3d_helpers.h"

#define ASSERT_TRUE(v)                                                 \
  if (!(v)) {                                                          \
    std::cerr << "False in ASSERT_TRUE at line " << __LINE__ << "!\n"; \
  }

#define ASSERT_FALSE(v)                                                \
  if (v) {                                                             \
    std::cerr << "True in ASSERT_FALSE at line " << __LINE__ << "!\n"; \
  }

#define ASSERT_FLOAT_EQUALS(f, v)                                    \
  if ((f) < (v) - 0.1F || (f) > (v) + 0.1F) {                        \
    std::cerr << "ASSERT_FLOAT_EQUALS: " << (f)                      \
              << " is not (roughly) equal to " << (v) << " at line " \
              << __LINE__ << "!\n";                                  \
  }

int main() {
  std::cout << "Testing 3d_helpers...\n";
  // Note that there is some weirdness regarding column-major ordering of matrix
  // values.
  {
    auto identity = get_identity_matrix();
    ASSERT_TRUE(identity.m0 == 1.0F);
    ASSERT_TRUE(identity.m1 == 0.0F);
    ASSERT_TRUE(identity.m2 == 0.0F);
    ASSERT_TRUE(identity.m3 == 0.0F);

    ASSERT_TRUE(identity.m4 == 0.0F);
    ASSERT_TRUE(identity.m5 == 1.0F);
    ASSERT_TRUE(identity.m6 == 0.0F);
    ASSERT_TRUE(identity.m7 == 0.0F);

    ASSERT_TRUE(identity.m8 == 0.0F);
    ASSERT_TRUE(identity.m9 == 0.0F);
    ASSERT_TRUE(identity.m10 == 1.0F);
    ASSERT_TRUE(identity.m11 == 0.0F);

    ASSERT_TRUE(identity.m12 == 0.0F);
    ASSERT_TRUE(identity.m13 == 0.0F);
    ASSERT_TRUE(identity.m14 == 0.0F);
    ASSERT_TRUE(identity.m15 == 1.0F);

    identity = identity * get_identity_matrix();
    ASSERT_FLOAT_EQUALS(identity.m0, 1.0F);
    ASSERT_FLOAT_EQUALS(identity.m1, 0.0F);
    ASSERT_FLOAT_EQUALS(identity.m2, 0.0F);
    ASSERT_FLOAT_EQUALS(identity.m3, 0.0F);

    ASSERT_FLOAT_EQUALS(identity.m4, 0.0F);
    ASSERT_FLOAT_EQUALS(identity.m5, 1.0F);
    ASSERT_FLOAT_EQUALS(identity.m6, 0.0F);
    ASSERT_FLOAT_EQUALS(identity.m7, 0.0F);

    ASSERT_FLOAT_EQUALS(identity.m8, 0.0F);
    ASSERT_FLOAT_EQUALS(identity.m9, 0.0F);
    ASSERT_FLOAT_EQUALS(identity.m10, 1.0F);
    ASSERT_FLOAT_EQUALS(identity.m11, 0.0F);

    ASSERT_FLOAT_EQUALS(identity.m12, 0.0F);
    ASSERT_FLOAT_EQUALS(identity.m13, 0.0F);
    ASSERT_FLOAT_EQUALS(identity.m14, 0.0F);
    ASSERT_FLOAT_EQUALS(identity.m15, 1.0F);
  }

  {
    // 90 degree rotation and identity matrix.
    auto m = get_rotation_matrix_about_z(PI / 2.0F) * get_identity_matrix();
    ASSERT_FLOAT_EQUALS(m.m0, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m1, 1.0F);
    ASSERT_FLOAT_EQUALS(m.m2, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m3, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m4, -1.0F);
    ASSERT_FLOAT_EQUALS(m.m5, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m6, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m7, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m8, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m9, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m10, 1.0F);
    ASSERT_FLOAT_EQUALS(m.m11, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m12, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m13, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m14, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m15, 1.0F);
  }
  {
    // Double 90 degree rotation about z.
    auto m = get_rotation_matrix_about_z(PI / 2.0F) *
             get_rotation_matrix_about_z(PI / 2.0F);
    ASSERT_FLOAT_EQUALS(m.m0, -1.0F);
    ASSERT_FLOAT_EQUALS(m.m1, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m2, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m3, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m4, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m5, -1.0F);
    ASSERT_FLOAT_EQUALS(m.m6, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m7, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m8, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m9, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m10, 1.0F);
    ASSERT_FLOAT_EQUALS(m.m11, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m12, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m13, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m14, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m15, 1.0F);

    // Rotate back 90 degrees.
    m = get_rotation_matrix_about_z(-PI / 2.0F) * m;
    ASSERT_FLOAT_EQUALS(m.m0, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m1, 1.0F);
    ASSERT_FLOAT_EQUALS(m.m2, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m3, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m4, -1.0F);
    ASSERT_FLOAT_EQUALS(m.m5, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m6, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m7, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m8, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m9, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m10, 1.0F);
    ASSERT_FLOAT_EQUALS(m.m11, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m12, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m13, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m14, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m15, 1.0F);
  }
  {
    // Rotate about y-axis 90 degrees.
    auto m = get_rotation_matrix_about_y(PI / 2.0F);
    ASSERT_FLOAT_EQUALS(m.m0, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m1, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m2, -1.0F);
    ASSERT_FLOAT_EQUALS(m.m3, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m4, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m5, 1.0F);
    ASSERT_FLOAT_EQUALS(m.m6, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m7, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m8, 1.0F);
    ASSERT_FLOAT_EQUALS(m.m9, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m10, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m11, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m12, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m13, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m14, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m15, 1.0F);

    // additional 90 degrees.
    m = get_rotation_matrix_about_y(PI / 2.0F) * m;
    ASSERT_FLOAT_EQUALS(m.m0, -1.0F);
    ASSERT_FLOAT_EQUALS(m.m1, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m2, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m3, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m4, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m5, 1.0F);
    ASSERT_FLOAT_EQUALS(m.m6, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m7, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m8, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m9, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m10, -1.0F);
    ASSERT_FLOAT_EQUALS(m.m11, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m12, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m13, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m14, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m15, 1.0F);
  }

  {
    // About x-axis 90 degrees.
    auto m = get_rotation_matrix_about_x(PI / 2.0F);
    ASSERT_FLOAT_EQUALS(m.m0, 1.0F);
    ASSERT_FLOAT_EQUALS(m.m1, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m2, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m3, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m4, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m5, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m6, 1.0F);
    ASSERT_FLOAT_EQUALS(m.m7, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m8, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m9, -1.0F);
    ASSERT_FLOAT_EQUALS(m.m10, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m11, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m12, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m13, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m14, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m15, 1.0F);

    // Additional 90 degrees.
    m = get_rotation_matrix_about_x(PI / 2.0F) * m;
    ASSERT_FLOAT_EQUALS(m.m0, 1.0F);
    ASSERT_FLOAT_EQUALS(m.m1, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m2, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m3, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m4, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m5, -1.0F);
    ASSERT_FLOAT_EQUALS(m.m6, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m7, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m8, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m9, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m10, -1.0F);
    ASSERT_FLOAT_EQUALS(m.m11, 0.0F);

    ASSERT_FLOAT_EQUALS(m.m12, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m13, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m14, 0.0F);
    ASSERT_FLOAT_EQUALS(m.m15, 1.0F);
  }

  {
    // ray_to_plane tests.
    auto result =
        ray_to_plane(Ray{Vector3{0.0F, 0.0F, 1.0F}, Vector3{0.0F, 1.0F, 0.0F}},
                     Ray{Vector3{0.0F, 2.0F, 0.0F}, Vector3{0.0F, 1.0F, 0.0F}});
    ASSERT_TRUE(result.has_value());
    ASSERT_FLOAT_EQUALS(0.0F, result.value().x);
    ASSERT_FLOAT_EQUALS(2.0F, result.value().y);
    ASSERT_FLOAT_EQUALS(1.0F, result.value().z);

    result =
        ray_to_plane(Ray{Vector3{7.0F, 2.0F, 1.0F}, Vector3{0.0F, 1.0F, 0.0F}},
                     Ray{Vector3{0.0F, 5.0F, 0.0F}, Vector3{0.0F, 1.0F, 0.0F}});
    ASSERT_TRUE(result.has_value());
    ASSERT_FLOAT_EQUALS(7.0F, result.value().x);
    ASSERT_FLOAT_EQUALS(5.0F, result.value().y);
    ASSERT_FLOAT_EQUALS(1.0F, result.value().z);

    // Parallel to plane test.
    result =
        ray_to_plane(Ray{Vector3{7.0F, 2.0F, 1.0F}, Vector3{0.0F, 0.0F, 1.0F}},
                     Ray{Vector3{0.0F, 5.0F, 0.0F}, Vector3{0.0F, 1.0F, 0.0F}});
    ASSERT_FALSE(result.has_value());
  }

  std::cout << "Finished tests.\n";
  return 0;
}

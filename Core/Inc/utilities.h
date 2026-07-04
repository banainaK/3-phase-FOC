#ifndef UTILITIES_H
#define UTILITIES_H

#ifdef __cplusplus
extern "C" {
#endif

struct Vec2 {
  double arr[2];
};
struct Vec3 {
  double arr[3];
};
struct Matrix2x2 {
  double arr[2][2];
};
struct Matrix3x3 {
  double arr[3][3];
};

struct Vec2 matrix_2x2_vector_product(struct Matrix2x2 transform, struct Vec2 input);
struct Vec3 matrix_3x3_vector_product(struct Matrix3x3 transform, struct Vec3 input);

void scalar_multiply(struct Matrix3x3 input, double scalar);
struct Vec3 clarke_transform(double current_1, double current_2, double current_3);
struct Vec2 park_transform(double current_1, double current_2, double phase_angle);

#endif 
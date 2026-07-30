#ifndef UTILITIES_H
#define UTILITIES_H

#ifdef __cplusplus
extern "C" {
#endif

#define M_PIF   3.141592653589793238462643383279502884e+00F

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

struct Matrix3x3 scalar_multiply_matrix(struct Matrix3x3 input, double scalar);
struct Vec3 scalar_multiply_vector(int input[], double scalar);
struct Vec3 scalar_multiply_struct(struct Vec3 input, double scalar);
struct Vec3 add_vectors(struct Vec3 input_1, struct Vec3 input_2);
struct Vec3 clarke_transform(struct Vec3 input);
struct Vec2 park_transform(struct Vec2 input, float electrical_angle);
struct Vec2 inverse_park_transform(struct Vec2 input, float electrical_angle);

void print_array(struct Vec2 currents);

#ifdef __cplusplus
}
#endif

#endif 
#include "utilities.h"
#include <math.h>
#include <stdio.h>

const double SQRT_3_2 = sqrt(3) / 2;

struct Matrix3x3 clarke_matrix = {{
  {1, -0.5f, -0.5f}, 
  {0, SQRT_3_2, -SQRT_3_2}, 
  {0.5f, 0.5f, 0.5f}
}};

struct Vec2 matrix_2x2_vector_product(struct Matrix2x2 transform, struct Vec2 input) {
  struct Vec2 output;
  output.arr[0] = transform.arr[0][0] * input.arr[0] + transform.arr[0][1] * input.arr[1];
  output.arr[1] = transform.arr[1][0] * input.arr[0] + transform.arr[1][1] * input.arr[1];
  return output;
}
struct Vec3 matrix_3x3_vector_product(struct Matrix3x3 transform, struct Vec3 input) {
  struct Vec3 output; 
  output.arr[0] = transform.arr[0][0] * input.arr[0] + transform.arr[0][1] * input.arr[1] + transform.arr[0][2] * input.arr[2];
  output.arr[1] = transform.arr[1][0] * input.arr[0] + transform.arr[1][1] * input.arr[1] + transform.arr[1][2] * input.arr[2];
  output.arr[2] = transform.arr[2][0] * input.arr[0] + transform.arr[2][1] * input.arr[1] + transform.arr[2][2] * input.arr[2];
  return output;
}
struct Matrix3x3 scalar_multiply_matrix(struct Matrix3x3 input, double scalar) {
  struct Matrix3x3 output;
    for (unsigned int i = 0; i < 3; ++i) {
        for (unsigned int j = 0; j < 3; ++j) {
            output.arr[i][j] = scalar * input.arr[i][j];
        }
    };
  return output;
}
struct Vec3 scalar_multiply_vector(int input[], double scalar) {
    struct Vec3 output;
    output.arr[0] = scalar * input[0];
    output.arr[1] = scalar * input[1];
    output.arr[2] = scalar * input[2];
    return output;
}
struct Vec3 scalar_multiply_struct(struct Vec3 input, double scalar) {
    struct Vec3 output;
    output.arr[0] = scalar * input.arr[0];
    output.arr[1] = scalar * input.arr[1];
    output.arr[2] = scalar * input.arr[2];
    return output;
}
struct Vec3 add_vectors(struct Vec3 input_1, struct Vec3 input_2) {
    struct Vec3 output;
    output.arr[0] = input_1.arr[0] + input_2.arr[0];
    output.arr[1] = input_1.arr[1] + input_2.arr[1];
    output.arr[2] = input_1.arr[2] + input_2.arr[2];
    return output;
}
// Clarke transform
struct Vec3 clarke_transform(struct Vec3 input) {
  // struct Vec3 input = {
  //   {current_1, current_2, current_3}
  // };
  return matrix_3x3_vector_product(clarke_matrix, input);
}
// Park transform
struct Vec2 park_transform(struct Vec2 input, float electrical_angle) {
  // convert phase angle to radians
  double rad = (electrical_angle * M_PIF) / 180;
  struct Matrix2x2 park_matrix = {{
    {cos(rad), sin(rad)}, 
    {-sin(rad), cos(rad)}
  }};
  return matrix_2x2_vector_product(park_matrix, input);
}
struct Vec2 inverse_park_transform(struct Vec2 input, float electrical_angle) {
  double rad = (electrical_angle * M_PIF) / 180;
  struct Matrix2x2 inverse_park_matrix = {
    {
      {cos(rad), -sin(rad)}, 
      {sin(rad), cos(rad)}
    }
  };
  return matrix_2x2_vector_product(inverse_park_matrix, input);
}

void print_array(struct Vec2 currents) {
  for (int i = 0; i < 2; i++) {
    if (i == 0) {
      printf("%f,", currents.arr[i]);
    } else {
      printf("%f", currents.arr[i]);
    }
  }
}




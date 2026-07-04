#include <math.h>

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
  output.arr[1] = transform.arr[2][0] * input.arr[0] + transform.arr[2][1] * input.arr[1] + transform.arr[2][2] * input.arr[2];
  return output;
}
void scalar_multiply(struct Matrix3x3 input, double scalar) {
  for (unsigned int i = 0; i < 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      input.arr[i][j] = scalar * input.arr[i][j];
    }
  };
}
// Clarke transform
struct Vec3 clarke_transform(double current_1, double current_2, double current_3) {
  struct Vec3 input = {
    {current_1, current_2, current_3}
  };
  return matrix_3x3_vector_product(clarke_matrix, input);
}
// Park transform
struct Vec2 park_transform(double current_1, double current_2, double phase_angle) {
  // convert phase angle to radians
  double rad = (phase_angle * PI) / 180;
  struct Matrix2x2 park_matrix = {{
    {cos(rad), -sin(rad)}, 
    {sin(rad), cos(rad)}
  }};
  struct Vec2 input = {
    {current_1, current_2}
  };
  return matrix_2x2_vector_product(park_matrix, input);
}




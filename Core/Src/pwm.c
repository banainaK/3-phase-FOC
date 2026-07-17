#include "pwm.h"
#include "utilities.h"
#include <math.h>


void initialize_reference(struct Reference* vec, double angle) {
    vec->angle = angle;
}

double convert_rpm_to_angle(double rpm, double period) {
    double rev_s = rpm / 60;
    double rad_s = 2 * M_PIF * rev_s;
    double rad = rad_s * period;
    double deg = rad * (180 / M_PIF);
    return deg;
}

void initialize_array(struct StateVectors* obj, int CCR) {
    obj->V_100[0] = CCR;
    
    obj->V_110[0] = CCR;
    obj->V_110[1] = CCR;

    obj->V_010[1] = CCR;

    obj->V_011[1] = CCR;
    obj->V_011[2] = CCR;

    obj->V_001[2] = CCR;
    obj->V_101[0] = CCR;
    obj->V_101[2] = CCR;
}

struct Vec3 update_CCR_new(double duty_cycle, double pwm_period, struct Reference* vector) {
    double angle = vector -> angle;
    double theta;
    struct Vec3 output;

    if (angle >= 0 && angle <= 60) {
        theta = angle;
    } else if (angle >= 60 && angle <= 120) {
        theta = angle - 60;
    } else if (angle >= 120 && angle <= 180) {
        theta = angle - 120;
    } else if (angle >= 180 && angle <= 240) {
        theta = angle - 180;
    } else if (angle >= 240 && angle <= 300) {
        theta = angle - 240;
    } else {
        theta = angle - 300;
    }

    double t_1 = pwm_period * duty_cycle * sin((60 - theta) * (M_PIF / 180));
    double t_2 = pwm_period * duty_cycle * sin(theta * (M_PIF / 180));
    double t_0 = pwm_period - t_1 - t_2;


    if (angle >= 0 && angle <= 60) {
        output.arr[0] = 0.5 * t_0; 
        output.arr[1] = 0.5 * t_0 + t_1;
        output.arr[2] = 0.5 * t_0 + t_1 + t_2;
    } else if (angle >= 60 && angle <= 120) {
        output.arr[0] = 0.5 * t_0 + t_2;
        output.arr[1] = 0.5 * t_0;
        output.arr[2] = 0.5 * t_0 + t_1 + t_2;
    } else if (angle >= 120 && angle <= 180) {
        output.arr[0] = 0.5 * t_0 + t_1 + t_2;
        output.arr[1] = 0.5 * t_0;
        output.arr[2] = 0.5 * t_0 + t_1;
    } else if (angle >= 180 && angle <= 240) {
        output.arr[0] = 0.5 * t_0 + t_1 + t_2;
        output.arr[1] = 0.5 * t_0 + t_2;
        output.arr[2] = 0.5 * t_0;
    } else if (angle >= 240 && angle <= 300) {
       output.arr[0] = 0.5 * t_0 + t_1;
       output.arr[1] = 0.5 * t_0 + t_1 + t_2;
       output.arr[2] = 0.5 * t_0;
    } else {
        output.arr[0] = 0.5 * t_0;
        output.arr[1] = 0.5 * t_0 + t_1 + t_2;
        output.arr[2] = 0.5 * t_0 + t_2;
    }
    return output;

}

struct Vec3 update_CCR(struct StateVectors* obj, struct Reference* vector) {
    double angle = vector->angle;
    double percent;

    // 120, 160, 180
    // 0, 40, 60 --> 66.6% 
    // 0  20  60 --> 33.3%

    struct Vec3 new_vector_a; 
    struct Vec3 new_vector_b;

    if (angle >= 0 && angle <= 60 ) {
        // linear combination of vectors
        percent = angle / 60;

        new_vector_a = scalar_multiply_vector(obj->V_100, 1 - percent);
        new_vector_b = scalar_multiply_vector(obj->V_110, percent);
        
    } else if (angle >= 60 && angle <= 120) {
        percent = (angle - 60) / 60;

        new_vector_a = scalar_multiply_vector(obj->V_110, 1 - percent);
        new_vector_b = scalar_multiply_vector(obj->V_010, percent);
        
    } else if (angle >= 120 && angle <= 180) {
        percent = (angle - 120) / 60;

        new_vector_a = scalar_multiply_vector(obj->V_010, 1 - percent);
        new_vector_b = scalar_multiply_vector(obj->V_011, percent);

    } else if (angle >= 180 && angle <= 240) {
        percent = (angle - 180) / 60;
        
        new_vector_a = scalar_multiply_vector(obj->V_011, 1 - percent);
        new_vector_b = scalar_multiply_vector(obj->V_001, percent);
    } else if (angle >= 240 && angle <= 300) {
        percent = (angle - 240) / 60;

        new_vector_a = scalar_multiply_vector(obj->V_001, 1 - percent);
        new_vector_b = scalar_multiply_vector(obj->V_101, percent);

    } else {
        percent = (angle - 300) / 60; 
        
        new_vector_a = scalar_multiply_vector(obj->V_101, 1 - percent);
        new_vector_b = scalar_multiply_vector(obj->V_100, percent);  
    }

    // add components of vector
    struct Vec3 output = add_vectors(new_vector_a, new_vector_b);
    // [TIM1 CCR, TIM8 CCR, TIM8 CCR]
    return output;
}

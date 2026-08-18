#ifndef PWM_H
#define PWM_H

#ifdef __cplusplus
extern "C" {
#endif

struct Reference {
    float angle;
    float magnitude; 
};

// initialize all your state vectors with CCR value
struct StateVectors {
    float V_100[3];
    float V_110[3];
    float V_010[3];
    float V_011[3];
    float V_001[3];
    float V_101[3];
};

void initialize_reference(struct Reference* vec, float angle, float magnitude);
void initialize_array(struct StateVectors* svpwm_obj, float CCR);
float convert_rpm_to_angle(float rpm, float period);
struct Vec3 get_CCR(struct StateVectors* obj, struct Reference* vector);
struct Vec3 update_CCR_new(float duty_cycle, float pwm_period, struct Reference* vector);

#ifdef __cplusplus
}
#endif

#endif

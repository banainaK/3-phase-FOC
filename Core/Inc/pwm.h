#ifndef PWM_H
#define PWM_H

#ifdef __cplusplus
extern "C" {
#endif

struct Reference {
    double angle; 
};

// initialize all your state vectors with CCR value
struct StateVectors {
    int V_100[3];
    int V_110[3];
    int V_010[3];
    int V_011[3];
    int V_001[3];
    int V_101[3];
};

void initialize_reference(struct Reference* vec, double angle);
void initialize_array(struct StateVectors* obj, int CCR);
double convert_rpm_to_angle(double rpm, double period);
struct Vec3 update_CCR(struct StateVectors* obj, struct Reference* vector);
struct Vec3 update_CCR_new(double duty_cycle, double pwm_period, struct Reference* vector);

#ifdef __cplusplus
}
#endif

#endif

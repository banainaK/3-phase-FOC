#include "utilities.h"

class PIController {
    public: 
        PIController(float kp, float ki, float reference);
        void update(float actual, float dt);
        void SetMaxOutput(float value, float max_integral_ratio);

    float kp;
    float ki;

    float reference;
    float error;
    float integral;
    float output; 

    // prevent integral windup
    float max_output;
    float max_integral;
};
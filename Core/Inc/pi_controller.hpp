#include "utilities.h"

class PIController {
    public: 
        PIController(float kp, float ki, float reference);
        void update(float actual, float dt);

    float kp;
    float ki;

    float reference;
    float error;
    float integral;
    float output; 
};
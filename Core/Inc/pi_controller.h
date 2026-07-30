#include "utilities.h"

class PIController {
    public: 
        PIController(float kp, float ki, float iq_reference, float id_reference);
        void update(float id, float iq, float dt);
        void update_kp(float kp);
        void update_ki(float ki);

    float kp;
    float ki;

    float iq_reference;
    float id_reference;

    float error_iq;
    float error_id;

    float integral_q;
    float integral_d;

    struct Vec2 outputs;
};
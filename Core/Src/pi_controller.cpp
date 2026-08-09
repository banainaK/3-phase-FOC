
#include "pi_controller.hpp"

PIController::PIController(float kp, float ki, float reference):
    kp(kp), ki(ki), reference(reference), error(0.0f), integral(0.0f), output(0.0f){

}

void PIController::update(float actual, float dt) {
    error = reference - actual;

    integral += error;
    if (integral > max_integral) {
        integral = max_integral;
    } else if (integral < -max_integral) {
        integral = -max_integral;
    }

    output = kp * error + ki * integral * dt;
    // output = kp * error;
}

void PIController::SetMaxOutput(float value, float max_integral_ratio) {
    max_output = value;
    if (ki != 0) {
        max_integral = max_output / (ki * max_integral_ratio);
    }
}

#include "pi_controller.hpp"

PIController::PIController(float kp, float ki, float reference):
    kp(kp), ki(ki), reference(reference), error(0.0f), integral(0.0f), output(0.0f) {

}

void PIController::update(float actual, float dt) {
    error = reference - actual;
    // 3.0 - (0.3) --> positive
    // 0 - (-18), positive value , higher corrective action
    // negative value, lower corrective action

    integral += error;
    output = kp * error + ki * integral * dt;
}
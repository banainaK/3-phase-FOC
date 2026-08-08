#include "simple_pi_controller.hpp"

SimplePIController::SimplePIController(float kp, float ki, float reference) {
    this -> kp = kp;
    this -> ki = ki;
    this -> reference = reference;
}
void SimplePIController::update(float actual, float dt) {
    float error = reference - actual;

    integral += error * dt;
    output = kp * error + ki * integral;
}
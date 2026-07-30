
#include "pi_controller.h"

PIController::PIController(float kp, float ki, float iq_reference, float id_reference) {
    this -> kp = kp;
    this -> ki = ki;

    this -> iq_reference = iq_reference;
    this -> id_reference = id_reference;
}


void PIController::update(float id, float iq, float dt) {
    this -> kp = kp;
    this -> ki = ki;

    error_id = id_reference - id;
    error_iq = iq_reference - iq;

    integral_d += error_id * dt;
    integral_q += error_iq * dt;

    outputs.arr[0] = kp * error_id + ki * integral_d;
    outputs.arr[1] = kp * error_iq + ki * integral_q;
}
void PIController::update_kp(float kp) {
    this -> kp = kp;
}
void PIController::update_ki(float ki) {
    this -> ki = ki;
}
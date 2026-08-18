
#include "foc.hpp"

FOC::FOC(float r_shunt, float v_ref, float gain, float num_pole_pairs) {
    // inline current sensing
    this -> r_shunt = r_shunt;
    this -> v_ref = v_ref;
    this -> v_offset = 0.5 * v_ref;
    this -> gain = gain;

    // conversion from mechanical to electrical angle
    this -> num_pole_pairs = num_pole_pairs;
}

void FOC::init_FOC_TIM(TIM_HandleTypeDef* htim1, TIM_HandleTypeDef* htim2, TIM_HandleTypeDef* htim3) {
    this -> htim1 = htim1;
    this -> htim2 = htim2;
    this -> htim3 = htim3;
}

void FOC::init_FOC_SPI(SPI_HandleTypeDef* hspi) {
    this -> hspi = hspi;
}

void FOC::set_pwm_period(float timer_frequency, float ARR) {
    this -> sys_clock_frequency = timer_frequency;
    this -> pwm_frequency = sys_clock_frequency / ARR;
    this -> pwm_period = 1 / pwm_frequency;
}








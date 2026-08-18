#include "stm32h5xx_hal.h"

class FOC {
    public: 
        FOC(float r_shunt, float v_ref, float gain, float num_pole_pairs);
        void init_FOC_TIM(TIM_HandleTypeDef* htim1, TIM_HandleTypeDef* htim2, TIM_HandleTypeDef* htim3);
        void init_FOC_SPI(SPI_HandleTypeDef* hspi);
        void set_pwm_period(float timer_frequency, float ARR);

        TIM_HandleTypeDef* htim1;
        TIM_HandleTypeDef* htim2;
        TIM_HandleTypeDef* htim3;

        SPI_HandleTypeDef* hspi;

        // inline current sensing
        float r_shunt;
        float v_ref;
        float v_offset;
        float gain;

        // conversion from mechanical to electrical angle
        float num_pole_pairs;

        float sys_clock_frequency; 
        float pwm_frequency;
        float pwm_period;
};

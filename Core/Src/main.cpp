/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "gpdma.h"
#include "icache.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stm32h5xx_hal.h"
// #include "stm32h5xx_hal_adc.h"
// #include "stm32h5xx_hal_adc_ex.h"
// #include "stm32h5xx_hal_def.h"
// #include "stm32h5xx_hal_dma.h"
// #include "stm32h5xx_hal_gpio.h"
// #include "stm32h5xx_hal_tim.h"
// #include "stm32h5xx_hal_uart.h"
#include "stm32h5xx_nucleo.h"
#include "tle5012b_util.hpp"

#include "utilities.h"
#include "svpwm.h"
#include "pi_controller.hpp"
#include "TLE5012b.hpp"

#include <cmath>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "tests.hpp"
#include "moving_average_filter.hpp"
#include "foc.hpp"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

COM_InitTypeDef BspCOMInit;

/* USER CODE BEGIN PV */

bool timer_done;

uint16_t adc_value[adc_data_size] = {};

// debugging
float current_a;
float current_b;
float current_c;

struct StateVectors svpwm_obj;
struct Reference vec;

FOC hfoc = FOC(0.33, 3.3, 1.53, 7.0);

PIController controller_id = PIController(1.0 , 0.0001, 0.0);
PIController controller_iq = PIController(1.0, 0.0001, 3.0);

MovingAverageFilter ia_filter = MovingAverageFilter();
MovingAverageFilter ib_filter = MovingAverageFilter();
MovingAverageFilter ic_filter = MovingAverageFilter();

Tle5012b encoderDriver;
float mechanical_angle;
float electrical_angle;
float angle_offset;
errorTypes error;

struct Vec2 controller_outputs;
struct Vec2 id_iq;
struct Vec3 svpwm_output;
struct Vec2 clarke;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

void start_pwm();
float convert_adc_to_voltage(uint16_t adc_value);
float convert_voltage_to_current(float out_voltage);
uint16_t get_average_adc_value(int channel);
struct Vec2 get_id_iq(float electrical_angle);
float run_angle_calibration();
float get_electrical_angle(float angle_offset);
void update_average_id_iq(float id, float iq);
void update_CCR(int channel_a, int channel_b, int channel_c);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  controller_id.SetMaxOutput(100, 1000);
  controller_iq.SetMaxOutput(100, 1000);

  initialize_array(&svpwm_obj, 5000.0f);
  initialize_reference(&vec, 0.0, 0.0);

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPDMA1_Init();
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_TIM8_Init();
  MX_TIM1_Init();
  MX_SPI2_Init();
  MX_ADC1_Init();
  MX_TIM15_Init();
  /* USER CODE BEGIN 2 */

  // initialize encoderDriver
  encoderDriver = Tle5012b(hspi2, GPIOC, GPIO_PIN_9);
  encoderDriver.resetFirmware();
  encoderDriver.readBlockCRC();

  hfoc.init_FOC_TIM(&htim1, &htim8, &htim15);
  hfoc.init_FOC_SPI(&hspi2);

  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_value, adc_data_size) != HAL_OK) {
    Error_Handler();
  }

  start_pwm();
  hfoc.set_pwm_period(250000000, 10000);
  angle_offset = run_angle_calibration();
  
  /* USER CODE END 2 */

  /* Initialize leds */
  BSP_LED_Init(LED_GREEN);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (timer_done) {
      electrical_angle = get_electrical_angle(angle_offset);

      id_iq = get_id_iq(electrical_angle);

      controller_id.update(id_iq.arr[0], hfoc.pwm_period);
      controller_iq.update(id_iq.arr[1], hfoc.pwm_period);

      struct Vec2 controller_outputs;

      controller_outputs.arr[0] = controller_id.output;
      controller_outputs.arr[1] = controller_iq.output;

      struct Vec2 va_vb = inverse_park_transform(controller_outputs, electrical_angle + 180);
      vec.angle = atan2f(va_vb.arr[1], va_vb.arr[0]) * (180 / M_PIF);

      if (vec.angle <= 0.0f) {
        vec.angle += 360.0f;
      }

      svpwm_output = get_CCR(&svpwm_obj, &vec);
      update_CCR((int)svpwm_output.arr[0], (int)svpwm_output.arr[1], (int)svpwm_output.arr[2]);

      timer_done = false;
      
    }
    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_CSI;
  RCC_OscInitStruct.CSIState = RCC_CSI_ON;
  RCC_OscInitStruct.CSICalibrationValue = RCC_CSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_CSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 125;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the programming delay
  */
  __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
}

/* USER CODE BEGIN 4 */
float convert_adc_to_voltage(uint16_t adc_value) {
  return (adc_value * hfoc.v_ref) / 4095;
}
float convert_voltage_to_current(float out_voltage) {
  return (out_voltage - hfoc.v_offset) / (hfoc.r_shunt * hfoc.gain);
}

Vec2 get_id_iq(float electrical_angle) {
  float v_a = convert_adc_to_voltage(get_average_adc_value(0));
  float v_b = convert_adc_to_voltage(get_average_adc_value(1));
  float v_c = convert_adc_to_voltage(get_average_adc_value(2));

  float i_a = convert_voltage_to_current(v_a);
  float i_b = convert_voltage_to_current(v_b);
  float i_c = convert_voltage_to_current(v_c);

  current_a = i_a;
  current_b = i_b;
  current_c = i_c;

  ia_filter.process(current_a);
  ib_filter.process(current_b);
  ic_filter.process(current_c);

  struct Vec3 filtered_currents;
  filtered_currents.arr[0] = ia_filter.average;
  filtered_currents.arr[1] = ib_filter.average;
  filtered_currents.arr[2] = ic_filter.average;

  // Clarke transform
  clarke = clarke_transform(filtered_currents);

  // Park transform
  struct Vec2 park = park_transform(clarke, electrical_angle);

  return park;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  // debugging purposes
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim == hfoc.htim1) {
    timer_done = true;
  }
}
uint16_t get_average_adc_value(int channel) {
  float adc_sum = 0;
  if (channel == 0) {
    for (int i = 0; i < adc_data_size; i+=3) {
      adc_sum += adc_value[i];
    }
  } else if (channel == 1) {
    for (int i = 1; i < adc_data_size; i+=3) {
      adc_sum += adc_value[i];
    }
  } else {
    for (int i = 2; i < adc_data_size; i+=3) {
      adc_sum += adc_value[i];
    }
  }
  return adc_sum / 8;
}

float run_angle_calibration() {
  // open loop control 
  float offset = 0.0;

  update_CCR(5000, 0, 0);
  HAL_Delay(500); // Wait for the motor to settle into a slot

  int num_samples = 100;
  for (int i = 0; i < num_samples; i++) {
    error = encoderDriver.getAngleValue(mechanical_angle);
    offset += mechanical_angle;
  }

  offset = offset / num_samples;
  return offset;
}

float get_electrical_angle(float angle_offset) {
  float e_angle;
  error = encoderDriver.getAngleValue(mechanical_angle);

  if (mechanical_angle < 0) {
    e_angle = mechanical_angle + 360;
  } else {
    e_angle = mechanical_angle;
  }

  if (error != NO_ERROR) {
    return -1.0;
  }
  e_angle = fmodf((e_angle - angle_offset) * hfoc.num_pole_pairs, 360.0f);
  
  if (e_angle < 0) {
    e_angle += 360.0f;
  }
  
  return e_angle;
}
void start_pwm() {
  //IN1
  HAL_TIM_PWM_Start(hfoc.htim1, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(hfoc.htim1);

  // IN2
  HAL_TIM_PWM_Start(hfoc.htim2, TIM_CHANNEL_2);
  // IN3
  HAL_TIM_PWM_Start(hfoc.htim2, TIM_CHANNEL_3);

  HAL_TIM_Base_Start_IT(hfoc.htim3);
}

void update_CCR(int channel_a, int channel_b, int channel_c) {
  hfoc.htim1 -> Instance -> CCR1 = channel_a;
  hfoc.htim2 -> Instance -> CCR2 = channel_b;
  hfoc.htim2 -> Instance -> CCR3 = channel_c;
}

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};
  MPU_Attributes_InitTypeDef MPU_AttributesInit = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region 0 and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x08FFF000;
  MPU_InitStruct.LimitAddress = 0x08FFFFFF;
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RO;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Attribute 0 and the memory to be protected
  */
  MPU_AttributesInit.Number = MPU_ATTRIBUTES_NUMBER0;
  MPU_AttributesInit.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);

  HAL_MPU_ConfigMemoryAttributes(&MPU_AttributesInit);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

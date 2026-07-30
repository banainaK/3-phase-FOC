/**
 * @file        spic.hpp
 * @brief       PAL of the SPI cover
 * @date        June 2020
 * @copyright   Copyright (c) 2019-2020 Infineon Technologies AG
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdint.h>
#include "stm32h5xx_hal.h"

#ifndef SPIC_HPP_
#define SPIC_HPP_

/**
 * @addtogroup pal
 * @{
 */

class SPIC {
  public:
    enum Error_t {
        OK = 0,           /**< No error */
        INTF_ERROR = -1,  /**< Interface error */
        CONF_ERROR = -2,  /**< Configuration error */
        READ_ERROR = -3,  /**< Read error */
        WRITE_ERROR = -4, /**< Write error */
    };

    SPIC(SPI_HandleTypeDef& spi, GPIO_TypeDef* port, uint16_t pin);

    /**
	 * @brief       Initializes the SPIC
	 * @return      SPIC error code
	 * @retval      OK if success
	 * @retval      INIT_ERROR if initialization error
	 */
    Error_t init();

    /**
	 * @brief       Deinitializes the SPIC
	 * @return      SPIC error code
	 * @retval      OK if success
	 * @retval      INIT_ERROR if deinitialization error
	 */
    Error_t deinit();

    /**
	 * @brief       Sends a trigger through out the SPI bus
	 * @return      SPIC error code
	 * @retval      OK if success
	 * @retval      INIT_ERROR if deinitialization error
	 */
    Error_t triggerUpdate();

    /**
	 * @brief           Function which allows 3wire SPI (SSC) by sending and receiving data in the same function
	 *
	 * @param sent_data              pointer two 2*unit16_t value for one command word and one data word if something should be written
	 * @param size_of_sent_data      the size of the command word default 1 = only command 2 = command and data word
	 * @param received_data          pointer to data structure buffer for the read data
	 * @param size_of_received_data  size of data words to be read
	 * @return                       SPIC error code
	 * @retval                       OK if success
	 * @retval                       INIT_ERROR if transfer fails
	 */
    Error_t sendReceive(uint16_t* sent_data, uint16_t size_of_sent_data, uint16_t* received_data,
                        uint16_t size_of_received_data);

    Error_t checkErrorStatus();

  private:
    SPI_HandleTypeDef* spi;
    GPIO_TypeDef* port;
    uint16_t pin;
    Error_t errorStatus;
};

/** @} */

#endif /** SPIC_HPP_ **/
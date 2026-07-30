/**
 * @file        spic.cpp
 * @brief       PAL of the SPI cover
 * @date        June 2020
 * @copyright   Copyright (c) 2019-2020 Infineon Technologies AG
 *
 * SPDX-License-Identifier: MIT
 */

#include "spic.hpp"
#include "main.h"

SPIC::Error_t SPIC::checkErrorStatus() {
    return errorStatus;
}

SPIC::SPIC(SPI_HandleTypeDef& spi, GPIO_TypeDef* port, uint16_t pin) {
    this->spi = &spi;
    this->port = port;
    this->pin = pin;
}

SPIC::Error_t SPIC::init() {
    return OK;
}

SPIC::Error_t SPIC::deinit() {
    return OK;
}

SPIC::Error_t SPIC::triggerUpdate() {
    return OK;
}

SPIC::Error_t SPIC::sendReceive(uint16_t* sent_data, uint16_t size_of_sent_data,
                                uint16_t* received_data, uint16_t size_of_received_data) {

    uint8_t conv_sent_data[2 * size_of_sent_data];
    uint8_t conv_received_data[2 * size_of_received_data];
    for (int i = 0; i < size_of_sent_data; i++) {
        conv_sent_data[i * 2] = (uint8_t)sent_data[i] & 0xFF;              // Low byte
        conv_sent_data[i * 2 + 1] = ((uint8_t)(sent_data[i] >> 8)) & 0xFF; // High byte
    }

    HAL_GPIO_WritePin(this->port, this->pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(this->spi, conv_sent_data, size_of_sent_data, 100);
    // Wait the required t_wr period
    for (int i = 0; i < 32; i++) {
        asm("NOP");
    }
    HAL_SPI_Receive(this->spi, conv_received_data, size_of_received_data, 100);
    HAL_GPIO_WritePin(this->port, this->pin, GPIO_PIN_SET);

    for (int i = 0; i < size_of_received_data; i++) {
        received_data[i] = (conv_received_data[i * 2 + 1] << 8) | conv_received_data[i * 2];
    }

    return OK;
}
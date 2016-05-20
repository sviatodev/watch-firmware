/*
 *******************************************************************************
 * Copyright © 2016 Sviatoslav Semchyshyn
 *******************************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *******************************************************************************
 */
/* Includes */
#include <workflow.h>
#include <stdbool.h>
#include "usart.h"
#include "gpio.h"
#include "i2c.h"
#include "mxconstants.h"
#include "SSD1306.h"

#define HAL_TIMEOUT_MAX 0x1000

/* Variables */

/* Functions */
int __io_putchar(int ch)
{
    uint8_t data[1];
    data[0] = ch;
    HAL_UART_Transmit(&huart1, data, 1, HAL_TIMEOUT_MAX);
    return ch;
}

int workflow(void)
{
    HAL_GPIO_WritePin(BT_RES_GPIO_Port, BT_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(BT_RES_GPIO_Port, BT_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(10);

    SSD1306_init(&hi2c1);

    while (true)
    {
        for (size_t x = 0; x < SSD1306_WIDTH; x++)
        {
            for (size_t y = 0; y < SSD1306_HEIGHT; y++)
            {
                SSD1306_setPixel(x, y, INV);
                SSD1306_update(&hi2c1);
            }
        }
    }
    return 0;
}

/*
 *******************************************************************************
 * Copyright © 2016 Sviatoslav Semchyshyn
 *******************************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *******************************************************************************
 */
#include <stdbool.h>
#include <string.h>

#include "gpio.h"
#include "i2c.h"
#include "adc.h"
#include "rtc.h"
#include "usbd_cdc_if.h"
#include "main.h"
#include "u8g/u8g.h"
#include "usb_device.h"

#define HAL_TIMEOUT_MAX 0x1000
#define ADC_CHANNEL_BATTERY ADC_CHANNEL_6

uint32_t adc_read(uint32_t channel)
{
    uint32_t val;
    ADC_ChannelConfTypeDef sConfig;

    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;
    HAL_ADC_ConfigChannel(&hadc, &sConfig);

    HAL_ADC_Start(&hadc);
    HAL_ADC_PollForConversion(&hadc, HAL_TIMEOUT_MAX);
    val = HAL_ADC_GetValue(&hadc);
    HAL_ADC_Stop(&hadc);
    return val;
}

static u8g_t u8g;
static uint8_t control = 0;

void u8g_Delay(uint16_t val)
{
    HAL_Delay(val);
}

uint8_t u8g_com_hw_i2c_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val,
                          void *arg_ptr)
{
    switch (msg)
    {
    case U8G_COM_MSG_ADDRESS:
        //SWITCH FROM DATA TO COMMAND MODE (arg_val == 0 for command mode)
        if (arg_val == 0) control = 0;
        else control = 0x40;
        break;
    case U8G_COM_MSG_WRITE_BYTE:
        //WRITE BYTE TO DEVICE
        HAL_I2C_Mem_Write(&hi2c1, 0x78, control, I2C_MEMADD_SIZE_8BIT, &arg_val,
                          1, HAL_TIMEOUT_MAX);
        break;
    case U8G_COM_MSG_WRITE_SEQ:
    case U8G_COM_MSG_WRITE_SEQ_P:
        //WRITE A SEQUENCE OF BYTES TO THE DEVICE
        HAL_I2C_Mem_Write(&hi2c1, 0x78, control, I2C_MEMADD_SIZE_8BIT, arg_ptr,
                          arg_val, HAL_TIMEOUT_MAX);
        break;
    default:
        break;
    }
    return 1;
}

char weekday[7][4] =
{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

void cdc_receive_callback(uint8_t* buf, uint32_t len)
{

}

void cdc_send(uint8_t* buf, uint32_t len)
{
    if (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED) return;
    if (((USBD_CDC_HandleTypeDef*) hUsbDeviceFS.pClassData)->TxState != 0) return;
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, buf, len);
    USBD_CDC_TransmitPacket(&hUsbDeviceFS);
}

int workflow()
{
    u8g_InitComFn(&u8g, &u8g_dev_ssd1306_128x64_i2c, u8g_com_hw_i2c_fn);
    u8g_Begin(&u8g);

    while (true)
    {
        char str_buf[6][64];
        uint32_t int_temp = adc_read(ADC_CHANNEL_TEMPSENSOR);
        uint32_t int_vref = adc_read(ADC_CHANNEL_VREFINT);
        uint32_t batt = adc_read(ADC_CHANNEL_BATTERY);
        RTC_TimeTypeDef time;
        RTC_DateTypeDef date;
        sprintf(str_buf[0], "temp %lu", int_temp);
        sprintf(str_buf[1], "vref %lu", int_vref);
        sprintf(str_buf[2], "batt %lu %lu", batt, batt / 128);
        sprintf(str_buf[3], "nCHRG=%u nSTDBY=%u",
                HAL_GPIO_ReadPin(nCHRG_GPIO_Port, nCHRG_Pin),
                HAL_GPIO_ReadPin(nSTDBY_GPIO_Port, nSTDBY_Pin));
        HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
        sprintf(str_buf[4], "%02u:%02u:%02u\r\n", time.Hours, time.Minutes,
                time.Seconds);
        sprintf(str_buf[5], "%u/%02u/%u %s", date.Date, date.Month, date.Year,
                weekday[date.WeekDay - 1]);
        cdc_send((uint8_t*)str_buf[4], strlen(str_buf[4]));

        u8g_FirstPage(&u8g);
        do
        {
            if (time.Seconds < 15)
            {
                uint8_t i;
                u8g_SetFont(&u8g, u8g_font_6x10);
                u8g_DrawFrame(&u8g, 0, 0, 128, 64);
                for (i = 0; i < 6; i++)
                    u8g_DrawStr(&u8g, 3, 10 * (i + 1), str_buf[i]);
            }
        } while (u8g_NextPage(&u8g));

        HAL_Delay(1000);
    }
    return 0;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin)
    {
    case BMC_DRDY_Pin:
        break;
    case BMC_INT1_Pin:
        break;
    case BMC_INT2_Pin:
        break;
    case BMC_INT3_Pin:
        break;
    default:
        break;
    }
}

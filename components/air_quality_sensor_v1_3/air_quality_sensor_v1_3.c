/*
    air_quality_sensor_v1_3.c
    Source file for Grove - Air Quality Sensor - v1.3.

    Author		    :   Priyanshu Roy
    Created Time	:   Sep 2024
    Modified Time   :

    The MIT License (MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include <stdio.h>
#include "string.h"

#include "air_quality_sensor_v1_3.h"

#include "esp_check.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SENSOR_DELAY_ON_START 20000

int air_quality_sensor_slope(air_quality_sensor_t *);
int air_quality_sensor_get_value(air_quality_sensor_t *);

void updateStandardVoltage(air_quality_sensor_t *);
void get_raw_sensor_reading(air_quality_sensor_t *);

bool initialize_air_quality_sensor(air_quality_sensor_t *device_handle)
{
    device_handle->_is_initialized = 0;

    adc_oneshot_unit_init_cfg_t adc_handle_int_config = {
        .unit_id = device_handle->_adc_num,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_handle_int_config, &device_handle->_adc_handle));

    adc_oneshot_chan_cfg_t adc_channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(device_handle->_adc_handle, device_handle->_io_num, &adc_channel_config));

    vTaskDelay(pdMS_TO_TICKS(SENSOR_DELAY_ON_START));
    device_handle->_is_initialized = 1;

    get_raw_sensor_reading(device_handle);

    if (10 < device_handle->_sensor_raw_value && device_handle->_sensor_raw_value < 798)
    {
        device_handle->_currentVoltage = device_handle->_sensor_raw_value;
        device_handle->_lastVoltage = device_handle->_currentVoltage;

        device_handle->_standardVoltage = device_handle->_sensor_raw_value;
        device_handle->_lastStdVolUpdated = (uint32_t)esp_timer_get_time() / 1000;
        return true;
    }
    return false;
}

void get_raw_sensor_reading(air_quality_sensor_t *device_handle)
{
    if (1 == device_handle->_is_initialized)
    {
        adc_oneshot_read(device_handle->_adc_handle, device_handle->_io_num, &device_handle->_sensor_raw_value);
    }
}

int air_quality_sensor_get_value(air_quality_sensor_t *device_handle)
{
    return device_handle->_currentVoltage;
}

int air_quality_sensor_slope(air_quality_sensor_t *device_handle)
{
    device_handle->_lastVoltage = device_handle->_currentVoltage;

    get_raw_sensor_reading(device_handle);

    device_handle->_currentVoltage = device_handle->_sensor_raw_value;

    device_handle->_voltageSum += device_handle->_currentVoltage;
    device_handle->_volSumCount += 1;

    updateStandardVoltage(device_handle);

    int voltageDiff = device_handle->_currentVoltage - device_handle->_lastVoltage;
    int voltageDeviation = device_handle->_currentVoltage - device_handle->_standardVoltage;

    if (voltageDiff > 400 && voltageDeviation > 150)
    {
        device_handle->_air_quality = FORCE_SIGNAL;
        return FORCE_SIGNAL;
    }
    else if ((voltageDiff > 400 && voltageDeviation > 50) || voltageDeviation > 150)
    {
        device_handle->_air_quality = HIGH_POLLUTION;
        return HIGH_POLLUTION;
    }
    else if ((voltageDiff > 200 && voltageDeviation > 50) || voltageDeviation > 50)
    {
        device_handle->_air_quality = LOW_POLLUTION;
        return LOW_POLLUTION;
    }
    else
    {
        device_handle->_air_quality = FRESH_AIR;
        return FRESH_AIR;
    }
    device_handle->_air_quality = INVALID;
    return INVALID;
}

void updateStandardVoltage(air_quality_sensor_t *device_handle)
{
    if ((uint32_t)esp_timer_get_time() / 1000 - device_handle->_lastStdVolUpdated > 500000)
    {
        device_handle->_standardVoltage = device_handle->_voltageSum / device_handle->_volSumCount;
        device_handle->_lastStdVolUpdated = (uint32_t)esp_timer_get_time() / 1000;

        device_handle->_voltageSum = 0;
        device_handle->_volSumCount = 0;
    }
}

void air_quality_error_to_message(air_quality_status_e error, char *error_message)
{
    memset(error_message, 0, sizeof(error_message));

    switch (error)
    {
    case FORCE_SIGNAL:
        strcpy(error_message, "High pollution! Force signal active.");
        break;
    case HIGH_POLLUTION:
        strcpy(error_message, "High pollution!");
        break;
    case LOW_POLLUTION:
        strcpy(error_message, "Low pollution!");
        break;
    case FRESH_AIR:
        strcpy(error_message, "Fresh air.");
        break;
    default:
        strcpy(error_message, "Unknown error.");
        break;
    }
}
/*
    air_quality_sensor_v1_3.h
    Header file for Grove - Air Quality Sensor - v1.3.

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
#ifndef AIR_QUALITY_SENSOR_H
#define AIR_QUALITY_SENSOR_H

#include "esp_adc/adc_oneshot.h"

typedef enum air_quality_status
{
    FORCE_SIGNAL = 0,
    HIGH_POLLUTION = 1,
    LOW_POLLUTION = 2,
    FRESH_AIR = 3,
    INVALID = -1,
} air_quality_status_e;

typedef struct air_quality_sensor
{
    uint8_t _is_initialized;
    adc_channel_t _io_num;
    adc_unit_t _adc_num;
    air_quality_status_e _air_quality;
    adc_oneshot_unit_handle_t _adc_handle;
    int _sensor_raw_value;
    int _lastVoltage;
    int _currentVoltage;
    uint32_t _voltageSum;
    int _standardVoltage;
    int _volSumCount;
    uint32_t _lastStdVolUpdated;
} air_quality_sensor_t;

bool initialize_air_quality_sensor(air_quality_sensor_t *);
int air_quality_sensor_slope(air_quality_sensor_t *);
int air_quality_sensor_get_value(air_quality_sensor_t *);
void air_quality_error_to_message(air_quality_status_e,char*);

#endif // AIR_QUALITY_SENSOR_H

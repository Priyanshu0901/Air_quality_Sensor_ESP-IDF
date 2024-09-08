idf_component_register(SRCS "air_quality_sensor_v1_3.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_adc 
                    PRIV_REQUIRES freertos esp_timer)

idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES air_quality_sensor_v1_3
                    PRIV_REQUIRES esp_timer freertos)

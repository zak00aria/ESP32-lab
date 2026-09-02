idf_component_register(SRCS "main.c" "game_of_life.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES  multiplexed_leds_matrix esp_driver_gpio)

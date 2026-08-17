idf_component_register(SRCS "main.c" "game_of_life.c"
                       PRIV_REQUIRES  multiplexed_leds_matrix esp_driver_gpio
                       INCLUDE_DIRS "include")

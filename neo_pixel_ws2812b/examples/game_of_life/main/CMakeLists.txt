idf_component_register(SRCS "main.c" "game_of_life.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES neo_pixel_ws2812b esp_driver_gpio)

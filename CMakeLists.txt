cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(phone_detector)

idf_component_register(
    SRCS "main/main.cpp"
         "main/wifi_sniffer.cpp"
         "main/device_tracker.cpp"
         "main/signal_processor.cpp"
         "main/buzzer_controller.cpp"
         "main/display_manager.cpp"
         "main/oui_database.cpp"
    INCLUDE_DIRS "main"
    REQUIRES esp_wifi driver esp_timer freertos esp_heap_caps nvs_flash
)

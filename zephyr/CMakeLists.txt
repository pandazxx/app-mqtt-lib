# Always safe to include this
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Create the library
zephyr_library_named(app_mqtt_lib)

zephyr_library_sources(
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/app_mqtt_lib.c
)

# Optionally only build/link if config is enabled
if(CONFIG_APP_MQTT_LIB)
  target_link_libraries(app PRIVATE app_mqtt_lib)
endif()


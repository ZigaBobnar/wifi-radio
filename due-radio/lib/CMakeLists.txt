add_library(due-radio-lib STATIC
    due-radio/common.c
    due-radio/app/audio_player.c
    due-radio/app/runtime.c
    due-radio/app/setup.c
    due-radio/app/ui.c
    due-radio/utils/fifo.c
    due-radio/utils/system.c
    due-radio/utils/timeguard.c
    due-radio/drivers/buttons.c
    due-radio/drivers/clock.c
    due-radio/drivers/console.c
    due-radio/drivers/dac.c
    due-radio/drivers/esp_module.c
    due-radio/drivers/lcd.c
)

target_include_directories(due-radio-lib
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)

if(MSVC)
  target_compile_options(due-radio-lib PRIVATE /W4 /WX)
else()
  target_compile_options(due-radio-lib PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

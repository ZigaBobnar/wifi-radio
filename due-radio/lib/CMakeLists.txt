add_library(due-radio-lib STATIC
    due-radio/common.c
    due-radio/utils/fifo.c
)

target_include_directories(due-radio-lib
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)

if(MSVC)
  target_compile_options(due-radio-lib PRIVATE /W4 /WX)
else()
  target_compile_options(due-radio-lib PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

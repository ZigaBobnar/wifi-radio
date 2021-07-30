add_library(due-radio-lib STATIC
    due-radio/due.c
)

target_include_directories(due-radio-lib
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)

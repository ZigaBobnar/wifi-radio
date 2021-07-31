add_executable(due-radio-tests
  test.c
  utils/fifo-test.c
)

target_link_libraries(due-radio-tests
    due-radio-lib
    unity
)

target_include_directories(due-radio-lib
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)

if(MSVC)
  target_compile_options(due-radio-tests PRIVATE /W4 /WX)
else()
  target_compile_options(due-radio-tests PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

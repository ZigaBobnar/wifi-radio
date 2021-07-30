add_executable(due-radio-tests
    test.c
)

target_link_libraries(due-radio-tests
    due-radio-lib
    unity
)

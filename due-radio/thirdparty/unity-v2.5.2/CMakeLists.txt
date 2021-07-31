add_library(unity STATIC
    src/unity.c
)

target_include_directories(unity
    PUBLIC src/
)

target_compile_definitions(unity
    PUBLIC
        UNITY_OUTPUT_COLOR
)
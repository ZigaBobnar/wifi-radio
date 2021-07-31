project("due-radio" C)
cmake_minimum_required(VERSION 3.0)

# Fix for vcpkg issues on cross compile toolchains
# Vcpkg is not used but if installed and integrated it causes problems
set(VCPKG_APPLOCAL_DEPS OFF)

add_compile_definitions(
    FAKE_HARDWARE=1
    FIFO_ERROR_ON_INVALID=1
    FIFO_ENABLE_VALIDITY_CHECKS=1
)

add_subdirectory(lib)
add_subdirectory(thirdparty/unity-v2.5.2)
add_subdirectory(tests)

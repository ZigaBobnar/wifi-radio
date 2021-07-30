# Due radio

This project is firmware for the wifi radio. It executes on Arduino Due board (SAM3X8E).

## Testing

Parts of code can be unit tested (those without direct dependency to ASF library). [Unity](https://www.throwtheswitch.org/unity) is used to run the tests.

The tests are supposed to be cross-platform as there should be no platform dependent code. For simplicity the toolchain for building is based on CMake. Unit testing also simplifies the development process as you do not need to upload to the actual hardware when modifying core parts of the project and can be tested directly on modern desktop hardware.

To build the project and run the tests, create a directory `out/` in the root of this folder. Then `cd` into it and run `cmake ..`. This will prepare and configure the build environment. Then you can move back to root folder and run `test.bat` or `test.sh`, depending on your platform.

You can also build and run separately by using `cmake --build ./out` and `./out/tests/bin/Debug/Tests` commands.

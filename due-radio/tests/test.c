#include <unity.h>
// #include "lib.h"

int aaa() {
    return 49;
}

void test_lib_Something_xX(void) {
    TEST_ASSERT_EQUAL(49, aaa());
}

void test_lib_Something_x1(void) {
    TEST_ASSERT_EQUAL(40, aaa());
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_lib_Something_xX);
    RUN_TEST(test_lib_Something_x1);
    
    return UNITY_END();
}

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

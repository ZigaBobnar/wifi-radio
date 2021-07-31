#include "test.h"
#include "due-radio/app/setup.h"

void setUp (void) {}
void tearDown (void) {}

TEST_F(full_boot_test) {
    setup_begin();
}

int main(void) {
    UnityBegin("due-radio-tests");
    
    runFifoTests();
    RUN_TEST(full_boot_test);
    
    return UnityEnd();
}

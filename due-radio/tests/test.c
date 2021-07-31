#include "test.h"

void setUp (void) {}
void tearDown (void) {}

int main(void) {
    UnityBegin("due-radio-tests");
    
    runFifoTests();
    
    return UnityEnd();
}

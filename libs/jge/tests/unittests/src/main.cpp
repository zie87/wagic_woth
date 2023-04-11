#include "unity/unity.hpp"

// define JGE dummies
int JGEGetTime() { return 0; }
bool JGEToggleFullscreen() { return true; }
void JGECreateDefaultBindings() {}

#if defined(PSP)
#include "JTypes.h"

u8 JGEGetAnalogX() { return 0; }
u8 JGEGetAnalogY() { return 0; }
#endif

static void RunAllTests(void) {
    RUN_TEST_GROUP(jge_math_math);
    RUN_TEST_GROUP(jge_math_vector_2d);
}

int main(int argc, const char* argv[]) { return UnityMain(argc, argv, RunAllTests); }

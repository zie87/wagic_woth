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
    RUN_TEST_GROUP(woth_string_algorithms_trim);
    RUN_TEST_GROUP(woth_string_algorithms_join);
    RUN_TEST_GROUP(woth_string_algorithms_split);
}

int main(int argc, const char* argv[]) { return UnityMain(argc, argv, RunAllTests); }

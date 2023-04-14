#include "unity/unity.hpp"

#if defined(SDL_CONFIG)
#define SDL_MAIN_HANDLED
#include <SDL.h>
#endif

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
    RUN_TEST_GROUP(woth_mana_mana_type);
    RUN_TEST_GROUP(woth_string_algorithms_trim);
    RUN_TEST_GROUP(woth_string_algorithms_join);
    RUN_TEST_GROUP(woth_string_algorithms_split);
}

int main(int argc, char* argv[]) { return UnityMain(argc, (const char**) argv, RunAllTests); }

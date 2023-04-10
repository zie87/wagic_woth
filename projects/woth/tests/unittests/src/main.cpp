#include "unity.hpp"

static void RunAllTests(void) { RUN_TEST_GROUP(mana_base_mana); }

int main(int argc, const char* argv[]) { return UnityMain(argc, argv, RunAllTests); }

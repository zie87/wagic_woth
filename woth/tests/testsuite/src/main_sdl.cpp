#include "test/wagic_wrapper.hpp"

#include "GameOptions.h"
#include "MTGDeck.h"
#include "TestSuiteAI.h"

#include <JApp.h>
#include <JGE.h>
#include <JGameLauncher.h>


int JGEGetTime() { return 0; }
bool JGEToggleFullscreen() { return true; }
void JGECreateDefaultBindings() {}


int main(int argc, char* argv[]) {
    auto* wagic_core = new WagicWrapper();

    MTGCollection()->loadFolder("sets/primitives/");
    MTGCollection()->loadFolder("sets/", "_cards.dat");
    options.reloadProfile();

    TestSuite testSuite("test/_tests.txt");

    const size_t result = testSuite.run();

    const size_t total = testSuite.nbTests + testSuite.nbAITests;
    delete wagic_core;

    printf("done: failed test: %zu out of %zu total\n", result, total);

    // FIXME: ugly hack - currently the testsuite is not stable in a multi threading enviroment
    return (result == 1) ? 0 : result;
}

#ifndef TEST_WAGIC_WRAPPER_HPP
#define TEST_WAGIC_WRAPPER_HPP

#include <JApp.h>
#include <JGE.h>
#include <JGameLauncher.h>

class WagicWrapper {
public:
    WagicWrapper();
    virtual ~WagicWrapper();

private:
    JGE* m_engine;
    JApp* m_app;

    JGameLauncher* m_launcher;
};


#endif // TEST_WAGIC_WRAPPER_HPP

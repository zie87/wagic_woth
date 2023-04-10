#include "test/wagic_wrapper.hpp"

WagicWrapper::WagicWrapper() : m_engine(NULL), m_app(NULL), m_launcher(new JGameLauncher()) {
    JGECreateDefaultBindings();

    m_engine = JGE::GetInstance();
    m_app    = m_launcher->GetGameApp();
    m_app->Create();
    m_engine->SetApp(m_app);
}

WagicWrapper::~WagicWrapper() {
    if (m_engine) {
        m_engine->SetApp(NULL);
    }

    if (m_app) {
        m_app->Destroy();
        delete m_app;
        m_app = NULL;
    }

    JGE::Destroy();
    m_engine = NULL;
    delete m_launcher;
}

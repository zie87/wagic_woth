#include "test/wagic_wrapper.hpp"

WagicWrapper::WagicWrapper() : m_engine(JGE::GetInstance()), m_app(nullptr), m_launcher(new JGameLauncher()) {
    JGECreateDefaultBindings();

    
    m_app    = m_launcher->GetGameApp();
    m_app->Create();
    m_engine->SetApp(m_app);
}

WagicWrapper::~WagicWrapper() {
    if (m_engine) {
        m_engine->SetApp(nullptr);
    }

    if (m_app) {
        m_app->Destroy();
        delete m_app;
        m_app = nullptr;
    }

    JGE::Destroy();
    m_engine = nullptr;
    delete m_launcher;
}

//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#ifndef JPARTICLESYSTEM_H
#define JPARTICLESYSTEM_H

#include <stdlib.h>
#include <list>
#include <vector>

#include "JGE.h"

#include "JParticle.h"

class JParticleEffect;
class JParticleEmitter;

//////////////////////////////////////////////////////////////////////////
/// The particle system of JGE++ is built on a key frame concept with
/// multiple emitters for each effect. It's inspired by the particle system
/// of Torque Game Builder. At each key frame, you can specify the value
/// of the changeable states of the particle. This gives you lots of
/// control over the particles and making it possible to create
/// almost all of the spectacular effects out of your imagination.
///
//////////////////////////////////////////////////////////////////////////
class JParticleSystem {
public:
    //////////////////////////////////////////////////////////////////////////
    /// Constructor.
    ///
    //////////////////////////////////////////////////////////////////////////
    JParticleSystem();

    ~JParticleSystem();

    //////////////////////////////////////////////////////////////////////////
    /// Update all active effects.
    ///
    /// @param dt - Delta time since last update (in second).
    ///
    //////////////////////////////////////////////////////////////////////////
    void Update(float dt);

    //////////////////////////////////////////////////////////////////////////
    /// Render all active effects.
    ///
    //////////////////////////////////////////////////////////////////////////
    void Render();

    //////////////////////////////////////////////////////////////////////////
    /// Start an effect.
    ///
    /// @param effect - Effect to start playing.
    ///
    //////////////////////////////////////////////////////////////////////////
    void StartEffect(JParticleEffect* effect);

    //////////////////////////////////////////////////////////////////////////
    /// Stop all effects.
    ///
    //////////////////////////////////////////////////////////////////////////
    void StopAllEffects();

    //////////////////////////////////////////////////////////////////////////
    /// Delete all effects from memory.
    ///
    //////////////////////////////////////////////////////////////////////////
    void ClearAll();

    //////////////////////////////////////////////////////////////////////////
    /// Check if the particle system is active or not.
    ///
    /// @return True if active.
    ///
    //////////////////////////////////////////////////////////////////////////
    bool IsActive() const;

    //////////////////////////////////////////////////////////////////////////
    /// Set active flag.
    ///
    /// @param flag - Active flag.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetActive(bool flag);

private:
    bool mActive;

    std::list<JParticleEffect*> mEffects;
};

#endif

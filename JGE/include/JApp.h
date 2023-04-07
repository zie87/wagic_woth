//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#ifndef _JAPP_H_
#define _JAPP_H_

class JGE;

//////////////////////////////////////////////////////////////////////////
/// Main application class for the system to run. The core game class
/// should be derived from this base class.
///
//////////////////////////////////////////////////////////////////////////
class JApp {
public:
    JApp();
    virtual ~JApp();

    //////////////////////////////////////////////////////////////////////////
    /// Initialization function.
    ///
    //////////////////////////////////////////////////////////////////////////
    virtual void Create() = 0;

    //////////////////////////////////////////////////////////////////////////
    /// Cleanup function before exiting from the game.
    ///
    //////////////////////////////////////////////////////////////////////////
    virtual void Destroy() = 0;

    //////////////////////////////////////////////////////////////////////////
    /// Update function to be called for each frame update. Should perform
    /// all the game logic here.
    ///
    /// @par Example: A simple Update() implementation:
    /// @code
    /// void Update()
    /// {
    ///		float dt = JGE::GetInstance()->GetDelta();
    ///		mX += mSpeed*dt;
    /// }
    /// @endcode
    ///
    //////////////////////////////////////////////////////////////////////////
    virtual void Update() = 0;

    //////////////////////////////////////////////////////////////////////////
    /// Render function to be called for each frame update. Should do all the
    /// game rendering here.
    ///
    /// @par Example: A simple Render() implementation:
    /// @code
    /// void Render()
    /// {
    ///		JRenderer *r = JRenderer::GetInstance();
    ///		r->FillRect(0,0,480,272,ARGB(255,0,0,0));
    /// }
    /// @endcode
    ///
    //////////////////////////////////////////////////////////////////////////
    virtual void Render() = 0;

    //////////////////////////////////////////////////////////////////////////
    /// Callback function called when the game is paused by the system.
    ///
    //////////////////////////////////////////////////////////////////////////
    virtual void Pause() = 0;

    //////////////////////////////////////////////////////////////////////////
    /// Callback function called when the game is resumed by the system.
    ///
    //////////////////////////////////////////////////////////////////////////
    virtual void Resume() = 0;

    virtual void OnScroll(int inXVelocity, int inYVelocity) = 0;
};

#endif

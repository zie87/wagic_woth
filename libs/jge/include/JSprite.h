//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#ifndef JSPRITE_H
#define JSPRITE_H

#if defined(PSP)
#include <fastmath.h>
#else
#include <math.h>
#endif

#include <vector>

#include "JRenderer.h"

//////////////////////////////////////////////////////////////////////////
/// Sprite is a container of single static image or animation frames.
///
//////////////////////////////////////////////////////////////////////////
class JSprite {
public:
    //////////////////////////////////////////////////////////////////////////
    /// Constructor.
    ///
    /// @param tex - Texture for the first frame and the following frames.
    ///					NULL to indicate no starting frame.
    /// @param x - X of the frame in texture.
    /// @param y - Y of the frame in texture.
    /// @param width - Width of the frame.
    /// @param height - Height of the frame.
    /// @param flipped - Indicate if the frame is horizontally flipped.
    ///
    //////////////////////////////////////////////////////////////////////////
    JSprite(JTexture* tex = nullptr, float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f,
            bool flipped = false);

    virtual ~JSprite();

    //////////////////////////////////////////////////////////////////////////
    /// Update animation.
    ///
    /// @param dt - Delta time since last update (in second).
    ///
    //////////////////////////////////////////////////////////////////////////
    virtual void Update(float dt);

    //////////////////////////////////////////////////////////////////////////
    /// Render current frame.
    ///
    //////////////////////////////////////////////////////////////////////////
    virtual void Render();

    //////////////////////////////////////////////////////////////////////////
    /// Set animation type.
    ///
    /// @param type - Animation type.
    ///
    /// @code
    ///		ANIMATION_TYPE_LOOPING - Repeat playing (Default).
    ///		ANIMATION_TYPE_ONCE_AND_GONE - Play animation once only.
    ///		ANIMATION_TYPE_ONCE_AND_BACK - Play to end and then stay at first frame.
    ///		ANIMATION_TYPE_PINGPONG - Play forward then backward and repeat.
    /// @endcode
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetAnimationType(int type);

    //////////////////////////////////////////////////////////////////////////
    /// Enable/Disable sprite.
    ///
    /// @param f - True to enable, false to disable.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetActive(bool f);

    //////////////////////////////////////////////////////////////////////////
    /// Get current active status.
    ///
    /// @return Active status.
    ///
    //////////////////////////////////////////////////////////////////////////
    bool IsActive() const;

    //////////////////////////////////////////////////////////////////////////
    /// Give sprite an id.
    ///
    /// @param id - Id.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetId(int id);

    //////////////////////////////////////////////////////////////////////////
    /// Get sprite id.
    ///
    /// @return Sprite id.
    //////////////////////////////////////////////////////////////////////////
    int GetId() const;

    //////////////////////////////////////////////////////////////////////////
    /// Flip a frame or all frames horizontally when rendering.
    ///
    /// @param flip - True to flip.
    /// @param index - Frame index, -1 to flip all frames.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetFlip(bool flip, int index = -1);

    //////////////////////////////////////////////////////////////////////////
    /// Add new animation frame.
    ///
    /// @param x - X of the frame in texture.
    /// @param y - Y of the frame in texture.
    /// @param width - Width of the frame.
    /// @param height - Height of the frame.
    /// @param flipped - Indicate if the frame is horizontally flipped.
    ///
    //////////////////////////////////////////////////////////////////////////
    void AddFrame(float x, float y, float width, float height, bool flipped = false);

    //////////////////////////////////////////////////////////////////////////
    /// Add new animation frame.
    ///
    /// @param tex - Texture for this frame and the following frames.
    /// @param x - X of the frame in texture.
    /// @param y - Y of the frame in texture.
    /// @param width - Width of the frame.
    /// @param height - Height of the frame.
    /// @param flipped - Indicate if the frame is horizontally flipped.
    ///
    //////////////////////////////////////////////////////////////////////////
    void AddFrame(JTexture* tex, float x, float y, float width, float height, bool flipped = false);

    //////////////////////////////////////////////////////////////////////////
    /// Set playback duration for each frame.
    ///
    /// @param duration - Playback duration (in second) for each frame.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetDuration(float duration);

    //////////////////////////////////////////////////////////////////////////
    /// Get index of current frame.
    ///
    /// @return Index of current frame.
    ///
    //////////////////////////////////////////////////////////////////////////
    int GetCurrentFrameIndex() const;

    //////////////////////////////////////////////////////////////////////////
    /// Set current frame to a particular index.
    ///
    /// @param frame - The new index of current frame.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetCurrentFrameIndex(int frame);

    //////////////////////////////////////////////////////////////////////////
    /// Get current frame image (quad).
    ///
    /// @return Quad object.
    ///
    //////////////////////////////////////////////////////////////////////////
    JQuad* GetCurrentFrame();

    //////////////////////////////////////////////////////////////////////////
    /// Get numer of animation frames.
    ///
    /// @return Numer of animation frames.
    ///
    //////////////////////////////////////////////////////////////////////////
    int GetFrameCount();

    //////////////////////////////////////////////////////////////////////////
    /// Get frame image (quad).
    ///
    /// @return Quad object.
    ///
    //////////////////////////////////////////////////////////////////////////
    JQuad* GetFrame(int index);

    //////////////////////////////////////////////////////////////////////////
    /// Restart animation.
    ///
    //////////////////////////////////////////////////////////////////////////
    void RestartAnimation();

    //////////////////////////////////////////////////////////////////////////
    /// Start animation.
    ///
    //////////////////////////////////////////////////////////////////////////
    void StartAnimation();

    //////////////////////////////////////////////////////////////////////////
    /// Stop animation.
    ///
    //////////////////////////////////////////////////////////////////////////
    void StopAnimation();

    //////////////////////////////////////////////////////////////////////////
    /// Get animation status.
    ///
    /// @return animation status
    ///
    //////////////////////////////////////////////////////////////////////////
    bool IsAnimating() const;

    //////////////////////////////////////////////////////////////////////////
    /// Move some distance from the current position.
    ///
    /// @param x - X distance to move.
    /// @param y - Y distance to move.
    ///
    //////////////////////////////////////////////////////////////////////////
    void Move(float x, float y);

    //////////////////////////////////////////////////////////////////////////
    /// Set position of the sprite.
    ///
    /// @param x - X position.
    /// @param y - Y position.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetPosition(float x, float y);

    //////////////////////////////////////////////////////////////////////////
    /// Set X position of the sprite.
    ///
    /// @param x - X position.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetX(float x);

    //////////////////////////////////////////////////////////////////////////
    /// Set Y position of the sprite.
    ///
    /// @param y - Y position.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetY(float y);

    //////////////////////////////////////////////////////////////////////////
    /// Get X position of the sprite.
    ///
    /// @return X position.
    ///
    //////////////////////////////////////////////////////////////////////////
    float GetX() const;

    //////////////////////////////////////////////////////////////////////////
    /// Get Y position of the sprite.
    ///
    /// @return Y position.
    ///
    //////////////////////////////////////////////////////////////////////////
    float GetY() const;

    //////////////////////////////////////////////////////////////////////////
    /// Get X velocity.
    ///
    /// @return X velocity.
    ///
    //////////////////////////////////////////////////////////////////////////
    float GetXVelocity() const;

    //////////////////////////////////////////////////////////////////////////
    /// Get Y velocity.
    ///
    /// @return Y velocity.
    ///
    //////////////////////////////////////////////////////////////////////////
    float GetYVelocity() const;

    //////////////////////////////////////////////////////////////////////////
    /// Set alpha value for rendering.
    ///
    /// @param alpha - Alpha value.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetAlpha(float alpha);

    //////////////////////////////////////////////////////////////////////////
    /// Get alpha value.
    ///
    /// @return Alpha value.
    ///
    //////////////////////////////////////////////////////////////////////////
    float GetAlpha() const;

    //////////////////////////////////////////////////////////////////////////
    /// Set scale of the sprite.
    ///
    /// @param hscale - Horizontal scale.
    /// @param vscale - Vertical scale.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetScale(float hscale, float vscale);

    //////////////////////////////////////////////////////////////////////////
    /// Set scale of the sprite.
    ///
    /// @param scale - Scale for both horizontal and vertical dimension.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetScale(float scale);

    //////////////////////////////////////////////////////////////////////////
    /// Get scale of the sprite.
    ///
    /// @return Scale of horizontal (assume same as the vertical).
    ///
    //////////////////////////////////////////////////////////////////////////
    float GetScale() const;

    //////////////////////////////////////////////////////////////////////////
    /// Set rotation factor of the sprite.
    ///
    /// @param rot - Rotation angle in radian.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetRotation(float rot);

    //////////////////////////////////////////////////////////////////////////
    /// Get rotation factor of the sprite.
    ///
    /// @return Rotation angle in radian.
    ///
    //////////////////////////////////////////////////////////////////////////
    float GetRotation() const;

    //////////////////////////////////////////////////////////////////////////
    /// Set moving speed of the sprite.
    ///
    /// @param speed - Moving speed.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetSpeed(float speed);

    //////////////////////////////////////////////////////////////////////////
    /// Get moving speed of the sprite.
    ///
    /// @return Moving speed.
    ///
    //////////////////////////////////////////////////////////////////////////
    float GetSpeed() const;

    //////////////////////////////////////////////////////////////////////////
    /// Set moving direction of the sprite.
    ///
    /// @param angle - Moving angle in radian.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetDirection(float angle);

    //////////////////////////////////////////////////////////////////////////
    /// Set moving direction of the sprite based on a targeting position.
    ///
    /// @param x - X position of the target.
    /// @param y - Y position of the target.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetDirection(float x, float y);

    //////////////////////////////////////////////////////////////////////////
    /// Get moving direction of the sprite.
    ///
    /// @return Moving angle in radian.
    ///
    //////////////////////////////////////////////////////////////////////////
    float GetDirection() const;

    //////////////////////////////////////////////////////////////////////////
    /// Set anchor point of a frame or all frames of the sprite. All rotation
    /// and collision operations are based on this anchor point.
    ///
    /// @param x - X position of the anchor point.
    /// @param y - Y position of the anchor point.
    /// @param index - Frame index, -1 for all frames.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetHotSpot(float x, float y, int index = -1);

    //////////////////////////////////////////////////////////////////////////
    /// Set color of the sprite for rendering.
    ///
    /// @param color - Color.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetColor(PIXEL_TYPE color);

    //////////////////////////////////////////////////////////////////////////
    /// \enum ANIMATION_TYPE
    ///
    /// Type of animation.
    ///
    //////////////////////////////////////////////////////////////////////////
    enum ANIMATION_TYPE {
        ANIMATION_TYPE_LOOPING,        ///< Repeat playing (Default).
        ANIMATION_TYPE_ONCE_AND_STAY,  ///< Play to the end and stay at last frame
        ANIMATION_TYPE_ONCE_AND_BACK,  ///< Play to end and then stay at first frame.
        ANIMATION_TYPE_ONCE_AND_GONE,  ///< Play animation once only.
        ANIMATION_TYPE_PINGPONG        ///< Play forward then backward and repeat.

    };

protected:
    static JRenderer* mRenderer;

    JTexture* mTex;
    std::vector<JQuad*> mFrames;
    float mDuration;
    float mTimer;
    int mFrameCount;
    int mCurrentFrame;
    int mAnimationType;
    int mDelta;
    bool mAnimating;

    float mAlpha;
    PIXEL_TYPE mColor;

    float mVScale;
    float mHScale;

    float mRotation;
    float mDirection;
    float mSpeed;

    int mId;
    bool mActive;

    float mX;
    float mY;
};

class JSpriteList {
protected:
    int mCount;
    JSprite** mList;
    // JSpriteList** mVictims;
    // JCollisionListener* mCollisionListener;

public:
    JSpriteList(int count);
    ~JSpriteList();
    void Update(float dt);
    void Render();
    void AddSprite(JSprite* sprite);  //, JSpriteList* victim);

    // bool CheckCollision(JSprite* sprite);							// check collision
    // against the provided list void SetCollisionListener(JCollisionListener *listener);
    JSprite* Activate(float x, float y);
    void Activate(float x, float y, int index);

    JSprite* GetSprite(int index);

    void EnableAll(bool flag);
};

#endif
#ifndef WFONT_H_
#define WFONT_H_

#include <JLBFont.h>
#include <JRenderer.h>
#include <JSprite.h>
#include "config.h"

namespace Fonts {
enum Font_Type { MAIN_FONT = 0, MENU_FONT = 1, OPTION_FONT = 1, MAGIC_FONT = 2, SMALLFACE_FONT = 3 };

// when using gbk languages and we need to keep around single byte font variants,
// the single byte fonts will be offset by this value
const unsigned int kSingleByteFontOffset = 100;
}  // namespace Fonts

class WFont {
public:
    int mFontID;
    // Rendering text to screen.
    // Note:
    // align=JGETEXT_LEFT,   string region (x-leftOffset, x-leftOffset+StringWidth), display window (x, x+displayWidth)
    // align=JGETEXT_CENTER, string region (x-leftOffset-StringWidth/2, x-leftOffset+StringWidth/2), display window
    // (x-displayWidth/2, x+displayWidth/2) align=JGETEXT_RIGHT,  string region (x-leftOffset-StringWidth,
    // x-leftOffset), display window (x-displayWidth, x) Only when width is NOT zero, characters outside the display
    // window are not rendered.
    //
    virtual void DrawString(const char* s, float x, float y, int align = JGETEXT_LEFT, float leftOffset = 0,
                            float displayWidth = 0) = 0;
    virtual void DrawString(std::string s, float x, float y, int align = JGETEXT_LEFT, float leftOffset = 0,
                            float displayWidth = 0) = 0;
    // Set font color.
    virtual void SetColor(PIXEL_TYPE color) = 0;
    // Get font color.
    virtual PIXEL_TYPE GetColor() const = 0;
    // Set scale for rendering.
    virtual void SetScale(float scale) = 0;
    // Get rendering scale.
    virtual float GetScale() const = 0;
    // Get height of font.
    virtual float GetHeight() const = 0;
    // Get width of rendering string on screen.
    virtual float GetStringWidth(const char* s) const = 0;
    // Set font tracking.
    virtual void SetTracking(float tracking) = 0;
    // Set Base for the character set to use.
    virtual void SetBase(int base) = 0;
    // Format text.
    virtual void FormatText(string& s, std::vector<string>& output) = 0;
    WFont(int inID) : mFontID(inID){};
    virtual ~WFont(){};
};

class WLBFont : public WFont {
public:
    WLBFont(int inFontID, const char* fontname, int lineheight, bool useVideoRAM = false);
    ~WLBFont() override { SAFE_DELETE(it); };

    void DrawString(const char* s, float x, float y, int align = JGETEXT_LEFT, float leftOffset = 0,
                    float width = 0) override {
        it->DrawString(s, x, y, align, leftOffset, width);
    };
    void DrawString(std::string s, float x, float y, int align = JGETEXT_LEFT, float leftOffset = 0,
                    float width = 0) override {
        it->DrawString(s, x, y, align, leftOffset, width);
    };
    void SetColor(PIXEL_TYPE color) override { it->SetColor(color); };
    PIXEL_TYPE GetColor() const override { return it->GetColor(); };
    void SetScale(float scale) override { it->SetScale(scale); };
    float GetScale() const override { return it->GetScale(); };
    float GetHeight() const override { return it->GetHeight(); };
    float GetStringWidth(const char* s) const override { return it->GetStringWidth(s); };
    void SetTracking(float tracking) override { it->SetTracking(tracking); };
    void SetBase(int base) override { it->SetBase(base); };
    void FormatText(string& s, std::vector<string>& output) override;

private:
    JLBFont* it;
};

class WFBFont : public WFont {
public:
    WFBFont(int inFontID, const char* fontname, int lineheight, bool useVideoRAM = false);
    WFBFont(int inFontID) : WFont(inFontID){};  // Legacy : remove it when possible
    ~WFBFont() override;

    void DrawString(std::string s, float x, float y, int align = JGETEXT_LEFT, float leftOffset = 0,
                    float width = 0) override;
    void SetColor(PIXEL_TYPE color) override;
    PIXEL_TYPE GetColor() const override { return mColor0; };
    void SetScale(float scale) override;
    float GetScale() const override;
    float GetHeight() const override;
    float GetStringWidth(const char* s) const override;
    void SetTracking(float tracking) override{};
    void SetBase(int base) override{};
    void FormatText(string& s, std::vector<string>& output) override{};

    void DrawString(const char* s, float x, float y, int align = JGETEXT_LEFT, float leftOffset = 0,
                    float width = 0) override;
    virtual int GetCode(const u8* ch, int* charLength) const = 0;
    virtual int GetMana(const u8* ch) const                  = 0;

protected:
    static JRenderer* mRenderer;

    u16* mIndex;
    u8* mStdFont;
    u8* mExtraFont;

    PIXEL_TYPE mColor0;
    PIXEL_TYPE mColor;
    unsigned int mFontSize;
    float mScale;
    unsigned int mBytesPerChar;
    unsigned int mBytesPerRow;

    int mCacheImageWidth;
    int mCacheImageHeight;
    int mCol;
    int mRow;
    int mCacheSize;
    JTexture* mTexture;
    JQuad** mSprites;
    int* mGBCode;
    int mCurr;

    u32* mCharBuffer;

    virtual int PreCacheChar(const u8* ch);
};

class WGBKFont : public WFBFont {
public:
    WGBKFont(int inFontID, const char* fontname, int lineheight, bool useVideoRAM = false);

    int PreCacheChar(const u8* ch) override;
    float GetStringWidth(const char* s) const override;
    void DrawString(const char* s, float x, float y, int align = JGETEXT_LEFT, float leftOffset = 0,
                    float width = 0) override;
    int GetCode(const u8* ch, int* charLength) const override;
    int GetMana(const u8* ch) const override;
    void FormatText(string& s, std::vector<string>& output) override;
};

class WUFont : public WFBFont {
public:
    WUFont(int inFontID, const char* fontname, int lineheight, bool useVideoRAM = false)
        : WFBFont(inFontID, fontname, lineheight, useVideoRAM){};

    int GetCode(const u8* ch, int* charLength) const override;
    int GetMana(const u8* ch) const override;
    void FormatText(string& s, std::vector<string>& output) override;
};

#endif

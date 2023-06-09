//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#ifndef JRESOURCEMANAGER_H
#define JRESOURCEMANAGER_H

#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include <stdio.h>
#include <vector>
#include <map>
#include <string>

#define INVALID_ID -1
#define ALREADY_EXISTS -2

class JRenderer;
class JSample;
class JMusic;
class JTexture;
class JQuad;
class JLBFont;

class JResourceManager {
public:
    JResourceManager();
    virtual ~JResourceManager();

    // void SetResourceRoot(const std::string& resourceRoot);
    bool LoadResource(const std::string& resourceName);

    virtual void RemoveAll();
    virtual void RemoveJLBFonts();

    virtual int CreateTexture(const std::string& textureName);
    virtual JTexture* GetTexture(const std::string& textureName);
    virtual JTexture* GetTexture(int id);

    virtual int CreateQuad(const std::string& quadName,
                           const std::string& textureName,
                           float x,
                           float y,
                           float width,
                           float height);
    virtual JQuad* GetQuad(const std::string& quadName);
    virtual JQuad* GetQuad(int id);

    virtual JLBFont* LoadJLBFont(const std::string& fontName, int height);
    virtual JLBFont* GetJLBFont(const std::string& fontName);
    virtual JLBFont* GetJLBFont(int id);

protected:
    std::vector<JTexture*> mTextureList;
    std::map<std::string, int> mTextureMap;

    std::vector<JQuad*> mQuadList;
    std::map<std::string, int> mQuadMap;

    std::vector<JLBFont*> mFontList;
    std::map<std::string, int> mFontMap;
};

#endif

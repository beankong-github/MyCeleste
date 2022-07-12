#pragma once
#include "CRes.h"
class CXMLData :
    public CRes
{
private:
    wstring m_wstrAtlasKey;
    map<wstring, tSprite> m_SpriteData;
    
public:
    tSprite* FindSpriteInfo(wstring _key);
    const map<wstring, tSprite> GetSpriteDatas() { return m_SpriteData; }
public:
    virtual int Load(const wstring& _strFilePath) override;

public:
    CXMLData();
    ~CXMLData();
};


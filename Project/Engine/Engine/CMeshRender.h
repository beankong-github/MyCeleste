#pragma once
#include "CRenderComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

class CMeshRender :
    public CRenderComponent
{
private:
    wstring             m_strAtlasKey;
    wstring             m_strSpriteKey;
    tSprite*            m_tTexInfo;

public:
    void SetAtlasKey(wstring _key) {  m_strAtlasKey = _key; }
    void SetSpritekey(wstring _key) { m_strSpriteKey = _key; }
    
    wstring GetAtlasKey() { return m_strAtlasKey; }
    wstring GetSpriteKey() { return m_strSpriteKey; }

    tSprite* GetTexInfo() { return m_tTexInfo; }

public:
    virtual void finalupdate() override;
    virtual void render() override;

public:
    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;
    CLONE(CMeshRender)

public:
    CMeshRender();    
    ~CMeshRender();
};


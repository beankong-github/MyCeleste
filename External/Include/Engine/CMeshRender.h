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
    
    Vec4                m_vColor;

public:
    void SetAtlasKey(wstring _key) {  m_strAtlasKey = _key; }
    void SetSpritekey(wstring _key) { m_strSpriteKey = _key; }
    void SetColor(Vec4 _color) { m_vColor = _color; }

    wstring GetAtlasKey() { return m_strAtlasKey; }
    wstring GetSpriteKey() { return m_strSpriteKey; }
    Vec4 GetColor() { return m_vColor; }


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


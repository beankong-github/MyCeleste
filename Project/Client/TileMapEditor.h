#pragma once
#include "UI.h"

#include <Engine\CGameObject.h>
#include <Engine\CTileMap.h>
#include "LandScapeEditor.h"

class TileMapEditor :
    public UI
{
private:
    CGameObject*        m_pTargetGameObject;
    CTileMap*           m_pTargetTileMap;

    LandScapeEditor*    m_pLSEditor;
    vector<tTileData>   m_vecCopyTileMap;
    Vec2                m_vecCopyTileMapSize;

public:
    void SetTargetObject(CGameObject* _pObj);
    void SetLSEditor(LandScapeEditor* _pEditor) { m_pLSEditor = _pEditor; }
    void CopyTileMap(vector<tTileData> _vecTileData);
    
    CGameObject* GetTargetGameObject() { return m_pTargetGameObject; }

    void Clear();
    void Apply(); 
    void Revert();

public:
    virtual void update() override;
    virtual void render_update() override;
   

public:
    TileMapEditor();
    ~TileMapEditor();
};


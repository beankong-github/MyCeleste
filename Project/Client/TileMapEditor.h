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

    LandScapeEditor*    m_pLSEditor;

    // TileMap
    CTileMap*           m_pTargetTileMap;
    vector<tTileData>   m_vecCopyTileMap;
    Vec2                m_vecCopyTileMapSize;

    // Colliders
    vector<CGameObject*> m_vecChildCol;
    CGameObject*         m_pSelecterCol;
    Vec2                 m_vOffsetPos;
    Vec2                 m_vOffsetScale;
    

public:
    void SetTargetObject(CGameObject* _pObj); 
    void SetLSEditor(LandScapeEditor* _pEditor) { m_pLSEditor = _pEditor; }
    void SetSelectedCollider(CGameObject* _pCol) { m_pSelecterCol = _pCol; }
    
    void CopyTileMap(vector<tTileData> _vecTileData);
    void AddCollider(CGameObject* _pCol) { m_vecChildCol.push_back(_pCol); }

    CGameObject* GetTargetGameObject() { return m_pTargetGameObject; }

    // tilemap
    void Clear();
    void Apply(); 
    void Revert();

    // collider
    void Add();

public:
    virtual void update() override;
    virtual void render_update() override;

private:
    // render_update() -> ...
    void DrawTile(int iTileIndex);
    void DrawCollider();
    void AddPrefab(int iTileIndex);

public:
    TileMapEditor();
    ~TileMapEditor();
};


#pragma once
#include "UI.h"

#include <Engine\CGameObject.h>

class TileMapEditor;

enum class LS_EDIT_TYPE
{
    NONE,
    TILE_MAP,
    COLLIDER,
    PREFAB,
    END
};

static const char* strLSEditType[(int)LS_EDIT_TYPE::END] =
{
    "NONE",
    "TILE_MAP",
    "COLLIDER",
    "PREFAB"
};

class LandScapeEditor :
    public UI
{
private:
    LS_EDIT_TYPE    m_eType;
    CGameObject*    m_pTargetGameObject;
    
    TileMapEditor*  m_pTileMapEditor;

    int             m_iCurTileIdx;

public:
    LS_EDIT_TYPE    GetType()                               { return m_eType; }
    CGameObject*    GetTargetGameObject()                   { return m_pTargetGameObject; }

    void            SetType(LS_EDIT_TYPE _eType)            { m_eType = _eType; }
    void            SetTargetObject(CGameObject* _pObj);


public:
    virtual void update() override;
    virtual void render_update() override;

public:
    int GetCurTileIdx() { return m_iCurTileIdx; }

private:
    void TileMapMode();
    void ColliderMode();
    void PrefabMode();

public:
    void TextureSelect(DWORD_PTR _texture);

public:
    LandScapeEditor();
    ~LandScapeEditor();
};


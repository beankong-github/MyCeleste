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

public:
    void SetTargetObject(CGameObject* _pObj) 
    {  
        // 타겟 게임 오브젝트는 TileMap 컴포넌트가 붙은 오브젝트여야한다.
        assert(_pObj->TileMap());
       
        // 초기화
        Clear();

        // 멤버 변수 다시 세팅
        m_pTargetTileMap = _pObj->TileMap();
        m_pTargetGameObject = _pObj;
        CopyTileMap(m_pTargetTileMap->GetTilaDataVector());
    }
    void SetLSEditor(LandScapeEditor* _pEditor) { m_pLSEditor = _pEditor; }
    void CopyTileMap(vector<tTileData> _vecTileData)
    {
        m_vecCopyTileMap.resize(_vecTileData.size());
        std::copy(_vecTileData.begin(), _vecTileData.end(), m_vecCopyTileMap.begin());
    }

    CGameObject*        GetTargetGameObject() { return m_pTargetGameObject; }

    void Clear();

public:
    virtual void update() override;
    virtual void render_update() override;
   

public:
    TileMapEditor();
    ~TileMapEditor();
};


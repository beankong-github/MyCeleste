#pragma once
#include "CRenderComponent.h"

#include "CTexture.h"
#include "CStructuredBuffer.h"

class CTileMap :
    public CRenderComponent
{
private:
    Ptr<CTexture>       m_pAtlasTex;        // 타일 이미지 아틀라스
    Vec2                m_vSlicePixel;      // 1 타일 크기
    Vec2                m_vSliceUV;         // 1 타일 UV

    UINT                m_iRowCount;        // 아틀라스 가로 타일 개수
    UINT                m_iColCount;        // 아틀라스 세로 타일 개수

    UINT                m_iTileCountX;      // 타일맵 x축 크기 (타일 단위)
    UINT                m_iTileCountY;      // 타일맵 y축 크기 (타일 단위)
    vector<tTileData>   m_vecTileData;      // 타일맵 데이터
    CStructuredBuffer*  m_pBuffer;          // 타일맵 버퍼

    bool                m_bBufferUpdated;   // 버퍼에 번경사항이 업데이트 되었는지 체크한다.

public:
    void SetAtlasTex(Ptr<CTexture> _pAtlasTex)
    {
        m_pAtlasTex = _pAtlasTex;
        m_vSliceUV = m_vSlicePixel / Vec2(m_pAtlasTex->Width(), m_pAtlasTex->Height());
    }
    //void SetAtlasTileCount(Vec2 _v)
    //{
    //    m_iRowCount = _v.x;
    //    m_iColCount = _v.y;
    //}

    void SetTileSize(Vec2 _vPixelSize)
    {
        m_vSlicePixel = _vPixelSize;
        if (nullptr != m_pAtlasTex)
        {
            // 타일 UV 계산하기
            m_vSliceUV = m_vSlicePixel / Vec2(m_pAtlasTex->Width(), m_pAtlasTex->Height());
            
            // 아틀라스의 가로 세로 타일 수 계산하기
            m_iColCount = m_pAtlasTex->Width() / m_vSlicePixel.x;
            m_iRowCount = m_pAtlasTex->Height() / m_vSlicePixel.y;

        }
    }

    void SetTileMapSize(UINT _iCountX, UINT _iCountY);
    void SetTileMapSize(Vec2 _vecSize);
    void SetTileData(int _iTileIdx, int _iImgIdx);

    
    vector<tTileData>   GetTilaDataVector()             { return m_vecTileData; }
    tTileData           GetTileData(int _iTileIdx)      { return m_vecTileData[_iTileIdx]; }

    CTexture*   GetAtlasTex()                 { return m_pAtlasTex.Get(); }
    Vec2        GetAtlasTileCount()           { return Vec2{ m_iColCount, m_iRowCount }; }
    Vec2        GetTileMapSize()              { return Vec2{ m_iTileCountX, m_iTileCountY }; }
    Vec2        GetTileSize()                 { return m_vSlicePixel; }
    Vec2        GetTileSliceUV()              { return m_vSliceUV; }

    void ClearTileData();


public:
    virtual void finalupdate() override;
    virtual void render() override;
    virtual void UpdateData() override;

public:
    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;
    CLONE(CTileMap)

public:
    CTileMap();
    CTileMap(const CTileMap& _origin);
    ~CTileMap();
};


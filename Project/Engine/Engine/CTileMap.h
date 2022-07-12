#pragma once
#include "CRenderComponent.h"

#include "CTexture.h"
#include "CStructuredBuffer.h"

class CTileMap :
    public CRenderComponent
{
private:
    Ptr<CTexture>       m_pAtlasTex;        // Ÿ�� �̹��� ��Ʋ��
    Vec2                m_vSlicePixel;      // 1 Ÿ�� ũ��
    Vec2                m_vSliceUV;         // 1 Ÿ�� UV

    UINT                m_iRowCount;        // ��Ʋ�� ���� Ÿ�� ����
    UINT                m_iColCount;        // ��Ʋ�� ���� Ÿ�� ����

    UINT                m_iTileCountX;      // Ÿ�ϸ� x�� ũ�� (Ÿ�� ����)
    UINT                m_iTileCountY;      // Ÿ�ϸ� y�� ũ�� (Ÿ�� ����)
    vector<tTileData>   m_vecTileData;      // Ÿ�ϸ� ������
    CStructuredBuffer*  m_pBuffer;          // Ÿ�ϸ� ����

    bool                m_bBufferUpdated;   // ���ۿ� ��������� ������Ʈ �Ǿ����� üũ�Ѵ�.

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
            // Ÿ�� UV ����ϱ�
            m_vSliceUV = m_vSlicePixel / Vec2(m_pAtlasTex->Width(), m_pAtlasTex->Height());
            
            // ��Ʋ���� ���� ���� Ÿ�� �� ����ϱ�
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


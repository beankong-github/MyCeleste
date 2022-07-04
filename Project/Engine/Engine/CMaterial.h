#pragma once
#include "CRes.h"

#include "CGraphicsShader.h"
#include "CTexture.h"


class CMaterial :
    public CRes
{
private:
    tScalarParam                m_Param;
    Ptr<CTexture>               m_arrTex[(UINT)TEX_PARAM::END];
    Ptr<CGraphicsShader>        m_pShader;

    vector<tScalarParamInfo>    m_vecScalarParamInfo;
    vector<tTexParamInfo>       m_vecTexParamInfo;

    CMaterial*                  m_pMasterMtrl;      // 원본 Material

public:
    void SetShader(Ptr<CGraphicsShader> _pShader);                          // 쉐이더 설정
    Ptr<CGraphicsShader> GetShader() { return m_pShader; }

    void SetScalarParam(SCALAR_PARAM _eType, void* _pData);                 // Type으로 Scalar Parameter 전달
    void SetScalarParam(const wstring& _strParamName, void* _pData);        // 이름으로 Scalar Parameter 전달

    void SetTexParam(TEX_PARAM _eType, Ptr<CTexture> _pTex);                // Type으로 Texture Parameter 전달
    void SetTexParam(const wstring& _strParamName, Ptr<CTexture> _pTex);    // 이름으로 Texture Parameter 전달

    const void* GetScalarParam(SCALAR_PARAM _eType);
    Ptr<CTexture> GetTexParam(TEX_PARAM _eType);

    void UpdateData();

    CMaterial* GetMtrlInst();
    CMaterial* GetMasterMtrl() { return m_pMasterMtrl; }

public:
    virtual int Save(const wstring& _strFilePath) override;
    virtual int Load(const wstring& _strFilePath) override;

    CLONE(CMaterial);

public:
    CMaterial();
    ~CMaterial();
};


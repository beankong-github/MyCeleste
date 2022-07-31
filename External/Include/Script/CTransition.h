#pragma once
#include <Engine\CScript.h>


class CTransition :
    public CScript
{

private:
    float                   m_fSpeed;
    Vec4                    m_vColor;
    int                     m_iCurIdx;
    vector<CTexture*>       m_vTex;

public:
    virtual void start();
    virtual void update();
    virtual void lateupdate();

    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;


    CLONE(CTransition)

public:
    CTransition();
    ~CTransition();
};


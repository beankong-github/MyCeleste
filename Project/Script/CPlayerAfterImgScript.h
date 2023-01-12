#pragma once
#include <Engine\CScript.h>

class CPlayerScript;

class CPlayerAfterImgScript :
    public CScript
{
private:
    CPlayerScript*            m_pPlayer;
    vector<CGameObject*>    m_vAfterImgs;
    vector<float>           m_vTimers;

    Vec4    m_vColor;
    UINT    m_iCurIdx;
    float   m_fTerm;
    float   m_fLifeTime;
    bool    m_bPlay;

public:
    virtual void start() override;
    virtual void update() override;
    virtual void lateupdate() override;

    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;

    CLONE(CPlayerAfterImgScript)

public:
    CPlayerAfterImgScript();
    ~CPlayerAfterImgScript();

};


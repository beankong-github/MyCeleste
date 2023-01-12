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
    
    bool                    m_bPlay;
    bool                    m_bSceneEnd;
    UINT                    m_iStartFrm;
    UINT                    m_iEndFrm;

public:
    void SceneStart();
    void SceneEnd();

    void Play();
    void Stop();
public:
    virtual void start() override;
    virtual void update() override;
    virtual void lateupdate() override;

    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;

    CLONE(CTransition)

public:
    CTransition();
    ~CTransition();
};


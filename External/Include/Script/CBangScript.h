#pragma once
#include <Engine\CScript.h>

class CPlayerScript;

class CBangScript :
    public CScript
{
    // 기본 가상함수

private:
    CPlayerScript* m_pPlayerScript;

    Vec4    m_vColor;

    Vec4    m_vDefaultHairColor;
    Vec4    m_vUseDashHairColor;

    float   m_fWaveTime;

public:
    void SetColor(Vec4 _color) { m_vColor = _color; }
    Vec4 GetColor() { return m_vColor; }
    
public:
    virtual void start();
    virtual void update();
    virtual void lateupdate();

    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;


    CLONE(CBangScript)

public:
    CBangScript();
    ~CBangScript();
};


#pragma once
#include <Engine/CScript.h>

class CCameraMoveScript :
    public CScript
{
private:
    CGameObject*    m_Filter;
    float           m_fCamSpeed;

public:
    void SetFilter(CGameObject* _pFilterObject){m_Filter = _pFilterObject;}

public:
    virtual void update() override;
    
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);

    CLONE(CCameraMoveScript)
public:
    CCameraMoveScript();
    ~CCameraMoveScript();
};


#pragma once
#include <Engine\CScript.h>

class CEditorCamScript :
    public CScript
{
private:
    float m_fSpeed;

    CLONE(CEditorCamScript);

public:
    virtual void update() override;

public:
    CEditorCamScript();
    ~CEditorCamScript();
};


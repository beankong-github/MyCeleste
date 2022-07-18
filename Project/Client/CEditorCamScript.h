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

    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile)override;

public:
    CEditorCamScript();
    ~CEditorCamScript();
};


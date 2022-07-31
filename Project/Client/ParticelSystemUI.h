#pragma once
#include "ComponentUI.h"
class ParticelSystemUI :
    public ComponentUI
{

private:
    int m_iSelectedIdx;

public:
    virtual void update() override;
    virtual void render_update() override;
    
private:
    void CSSelect(DWORD_PTR _param);
    void TexSelect(DWORD_PTR _param);

public:
    ParticelSystemUI();
    ~ParticelSystemUI();
};


#pragma once
#include "ResInfoUI.h"

class ComponentUI;

class PrefabUI :
    public ResInfoUI
{
private:
    void ShowOBJInfo();

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    PrefabUI();
    virtual ~PrefabUI();
};


#pragma once
#include "ComponentUI.h"
class TileMapUI :
    public ComponentUI
{
private:

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    TileMapUI();
    ~TileMapUI();
};


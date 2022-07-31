#pragma once

#include "componentUI.h"

class MeshRenderUI :
    public ComponentUI
{
private:

public:
    virtual void update() override;
    virtual void render_update() override;

private:
    void MeshSelect(DWORD_PTR _param);
    void MtrlSelect(DWORD_PTR _param);
    void TextureSelect(DWORD_PTR _param);
    void SpriteKeySelect(DWORD_PTR _param);

public:
    MeshRenderUI();
    ~MeshRenderUI();
};


#pragma once
#include "ComponentUI.h"

enum class LIGHT_COLOR_TYPE
{
    DIFFUSE,
    SPECULAR,
    AMBIENT,
    END
};
class Light2DUI :
    public ComponentUI
{

private:
    bool                m_bOpenPopup;
    LIGHT_COLOR_TYPE    m_eSelcetLightType;


private:
    void OpenColorPicker(ImVec4& _color);

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    Light2DUI();
    ~Light2DUI();
};


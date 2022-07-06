#pragma once
#include "UI.h"


class MenuUI :
    public UI
{
private:
    string  m_strPlayPause;

    bool    m_bPackaging;
    bool    m_bSceneSave;
    bool    m_bSceneLoad;
    bool    m_bScenePlay;   
    bool    m_bScenePause;   
    bool    m_bSceneStop;

public:
    virtual void update() override;
    virtual void render() override;
    virtual void render_update() override;

private:
    void Task();
    void AddComponent(CGameObject* _pTarge, COMPONENT_TYPE _type);

public:
    MenuUI();
    ~MenuUI();
};


#pragma once
#include "UI.h"

#include <Engine/CGameObject.h>

class ComponentUI;
class ScriptUI;
class ResInfoUI;
class CRes;
class Create2DAnimationUI;

class InspectorUI :
    public UI
{
private:
    CGameObject*        m_pTargetObject;
    CRes*               m_pTargetRes;

    ComponentUI*        m_arrComUI[(UINT)COMPONENT_TYPE::END];
    ResInfoUI*          m_arrResUI[(UINT)RES_TYPE::END];

    vector<ScriptUI*>   m_vecScriptUI;
    Create2DAnimationUI* m_pCreate2DAnim;

public:
    CGameObject* GetTargetObject() { return m_pTargetObject; }
    ComponentUI** GetCompUIArr() { return m_arrComUI; }

    void SetTargetObject(CGameObject* _pTarget);
    void SetTargetResource(CRes* _pTargetRes);

private:
    ScriptUI* AddScriptUI();

public:
    virtual void update() override;
    virtual void render_update() override;


    friend class PrefabUI;


public:
    InspectorUI();
    ~InspectorUI();
};


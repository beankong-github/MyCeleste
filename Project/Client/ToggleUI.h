#pragma once
#include "UI.h"

#include <Engine\CKeyMgr.h>

class ToggleUI;

class ToggleNode
{
private:
    ToggleUI*               m_pToggleUI;
    ToggleNode*             m_pParent;
    vector<ToggleNode*>     m_vecChild;

    bool                    m_bHasChild;
    bool                    m_bSelected;

    string                  m_strName;
    //DWORD_PTR               m_dwData;

public:
    void update();
    void render_update();

public:
    void AddChild(ToggleNode* _pChild)
    {
        _pChild->m_pParent = this;
        m_vecChild.push_back(_pChild);
        m_bHasChild = true;
    }

    ToggleNode*         GetParent() { return m_pParent; }
    const string&       GetName() { return m_strName; }
    //DWORD_PTR         GetData() { return m_dwData; }

private:
    void CheckMouseEvent();

public:
    ToggleNode();
    ToggleNode(const string& _strName);
    //TreeNode(const string& _strName, DWORD_PTR _dwData);
    ~ToggleNode();

    friend class ToggleUI;

};

typedef void(UI::* CLICKED)(DWORD_PTR);
typedef CLICKED KEY_FUNC;

class ToggleUI :
    public UI
{
private:
    vector<ToggleNode*> m_vecItems;

    // Clicked Delegate
    UI* m_pCInst;
    CLICKED             m_CFunc;

    // Double Clicked
    UI* m_pDBCInst;
    CLICKED             m_DBCFunc;

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    void AddToggleItem(ToggleNode* _node) { m_vecItems.push_back(_node); }

    void SetClickedDelegate(UI* _pInst, CLICKED _Func) { m_pCInst = _pInst; m_CFunc = _Func; }
    void SetDoubleClickedDelegate(UI* _pInst, CLICKED _Func) { m_pDBCInst = _pInst; m_DBCFunc = _Func; }

    void Clear();

public:
    ToggleUI();
    ~ToggleUI();


friend class TreeNode;
};


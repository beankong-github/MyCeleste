#pragma once
#include "UI.h"

#include "TreeUI.h"

class CGameObject;

class SceneOutliner :
    public UI
{
private:
    TreeUI*     m_TreeUI;

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    void Reset();

    TreeUI* GetTreeUI() { return m_TreeUI; }
    
    void AddGameObjectToTree(CGameObject* _pObject, TreeNode* _pDestNode);
    
    void OnRightClicked();
    void OnObjectNodeClicked(DWORD_PTR _dw);
    void OnPressDelete(DWORD_PTR _dw);
    void DragAndDropDelegate(DWORD_PTR _dwDrag, DWORD_PTR _dwDrop);
    bool DropCheckDelegate();
    void ResDrop(DWORD_PTR _resPtr);

public:
    SceneOutliner();
    ~SceneOutliner();
};


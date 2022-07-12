#pragma once
#include "UI.h"

class TreeUI;

class ResourceUI :
    public UI
{
private:
    TreeUI*     m_pTreeUI;
    CRes*       m_pRClickedRes;

    vector<wstring> m_vecResPath;               // ���ҽ��� ��� ��� ����

    bool        m_bToggleOn;

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    TreeUI* GetTreeUI() { return m_pTreeUI; }
    void Reset();

private:
    void Reload();                                      // content ������ �������� �����Ǹ� ���ҽ��� �ٽ� �ε��Ѵ�. 
    void Renew();                                       // resource treeUI�� �����Ѵ�.
    void FindFileName(const wstring& _strFolderPath);   // Content ���� ������ ��� ������ ����θ� �˾Ƴ���.

    void ItemClicked(DWORD_PTR _dwNode);                // ���ҽ� Ŭ���� ȣ��
    void ItemRightClicked(DWORD_PTR _dwNode);           // ���ҽ� ��Ŭ���� ȣ��
    void ItemDoubleClicked(DWORD_PTR _dwNode);          // ���ҽ� ���� Ŭ���� ȣ��
    void DragAndDropDelegate(DWORD_PTR _dwDrag, DWORD_PTR _dwDrop); // DragDrop

    RES_TYPE GetResTypeFromExt(const wstring& _strExt); // ���� Ȯ���ڷ� ���ҽ��� ������ �˾Ƴ���.

public:
    ResourceUI();
    ~ResourceUI();
};


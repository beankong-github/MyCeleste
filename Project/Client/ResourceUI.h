#pragma once
#include "UI.h"

class TreeUI;

class ResourceUI :
    public UI
{
private:
    TreeUI* m_TreeUI;
    vector<wstring> m_vecResPath;                       // ���ҽ��� ��� ��ε��� ���� 
public:
    virtual void update() override;
    virtual void render_update() override;

public:
    void Reset();    
    TreeUI* GetTreeUI() { return m_TreeUI; }

private:
    void Reload();                                      // content ������ �������� �����Ǹ� ���ҽ��� �ٽ� �ε��Ѵ�. 
    void Renew();                                       // resource treeUI�� �����Ѵ�.
    void FindFileName(const wstring& _strFolderPath);   // Content ���� ������ ��� ������ ����θ� �˾Ƴ���.

    void ItemClicked(DWORD_PTR _dwNode);                // ���ҽ� Ŭ���� ȣ��
    void ItemDoubleClicked(DWORD_PTR _dwNode);          // ���ҽ� ���� Ŭ���� ȣ��


    void DragAndDropDelegate(DWORD_PTR _dwDrag, DWORD_PTR _dwDrop); // DragDrop

    RES_TYPE GetResTypeFromExt(const wstring& _strExt); // ���� Ȯ���ڷ� ���ҽ��� ������ �˾Ƴ���.

public:
    ResourceUI();
    ~ResourceUI();
};


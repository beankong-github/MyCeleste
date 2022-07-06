#pragma once
#include "UI.h"

class TreeUI;

class ResourceUI :
    public UI
{
private:
    TreeUI* m_TreeUI;
    vector<wstring> m_vecResPath;                       // 리소스의 상대 경로들을 담은 
public:
    virtual void update() override;
    virtual void render_update() override;

public:
    void Reset();    
    TreeUI* GetTreeUI() { return m_TreeUI; }

private:
    void Reload();                                      // content 폴더에 변경점이 감지되면 리소스를 다시 로드한다. 
    void Renew();                                       // resource treeUI를 갱신한다.
    void FindFileName(const wstring& _strFolderPath);   // Content 폴더 밑으로 모든 파일의 상대경로를 알아낸다.

    void ItemClicked(DWORD_PTR _dwNode);                // 리소스 클릭시 호출
    void ItemDoubleClicked(DWORD_PTR _dwNode);          // 리소스 더블 클릭시 호출


    void DragAndDropDelegate(DWORD_PTR _dwDrag, DWORD_PTR _dwDrop); // DragDrop

    RES_TYPE GetResTypeFromExt(const wstring& _strExt); // 파일 확장자로 리소스의 종류를 알아낸다.

public:
    ResourceUI();
    ~ResourceUI();
};


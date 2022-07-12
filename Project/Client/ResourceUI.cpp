#include "pch.h"
#include "ResourceUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CEventMgr.h>
#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>

#include <Script/CSceneSaveLoad.h>

#include "CImGuiMgr.h"
#include "InspectorUI.h"
#include "TreeUI.h"


ResourceUI::ResourceUI()
	: UI("Resource")
{
	m_pTreeUI = new TreeUI(true);
	m_pTreeUI->SetTitle("Resource");
	m_pTreeUI->UseFrame(true);
	m_pTreeUI->UseDragDropOuter(true);
	m_pTreeUI->UseDragDropSelf(true);

	AddChild(m_pTreeUI);

	// Clicked Delegate ���
	m_pTreeUI->SetClickedDelegate(this, (CLICKED)&ResourceUI::ItemClicked);

	// Right Delegate ���
	m_pTreeUI->SetRightClickedDelegate(this, (CLICKED)&ResourceUI::ItemRightClicked);

	// Double Clicked Delegate ���
	m_pTreeUI->SetDoubleClickedDelegate(this, (CLICKED)&ResourceUI::ItemDoubleClicked);
	// Drag Drop Delegate ���
	m_pTreeUI->SetDragAndDropDelegate(this, (DRAG_DROP)&ResourceUI::DragAndDropDelegate);

	Reset();
}

ResourceUI::~ResourceUI()
{
}

void ResourceUI::update()
{
	// ���ҽ� ������� ����
	if (KEY_PRESSED(KEY::LCTRL) && KEY_TAP(KEY::S))
	{
		CResMgr::GetInst()->SaveChangedRes();
	}

	UI::update();
}

void ResourceUI::render_update()
{	
	if (CEventMgr::GetInst()->HasOccurObjEvent())
	{
		Renew();
	}

	// ========
	//	Toggle
	// ========
	if (m_bToggleOn)
	{
		// Ű �˻�
		if (KEY_TAP(KEY::ESC))
		{
			m_bToggleOn = false;
			return;
		}

		// PopUp Open
		ImGui::OpenPopup("##Popup Toggle");
	}
	if (ImGui::BeginPopup("##Popup Toggle"))
	{
		if (ImGui::MenuItem("Delete"))
		{
			tEventInfo info = {};

			info.eType = EVENT_TYPE::DELETE_RES;
			info.lParam = (DWORD_PTR)m_pRClickedRes;
			CEventMgr::GetInst()->AddEvent(info);

			Reset();

			m_pRClickedRes = nullptr;
			m_bToggleOn = false;
		}

		ImGui::EndPopup();
	}
}

void ResourceUI::Reset()
{
	// Content ���� ���� ���ҽ� �ε�
	Reload();

	Renew();
}

void ResourceUI::Reload()
{
	// Content ���� ������ ��� ������ ����θ� �˾Ƴ���.
	m_vecResPath.clear();
	FindFileName(CPathMgr::GetInst()->GetContentPath());

	// Ű�� Ȯ��
	for (size_t i = 0; i < m_vecResPath.size(); ++i)
	{
		// 1. ���ϸ��� ���ؼ� ���ҽ��� ������ �˾Ƴ���.
		RES_TYPE eType = GetResTypeFromExt(m_vecResPath[i]);

		// 2. ����θ� Ű������ �ؼ� ���ҽ��� �ε��Ѵ�.
		switch (eType)
		{
		case RES_TYPE::PREFAB:
			CResMgr::GetInst()->Load<CPrefab>(m_vecResPath[i], m_vecResPath[i]);
			break;
		case RES_TYPE::MESHDATA:

			break;
		case RES_TYPE::MATERIAL:
			CResMgr::GetInst()->Load<CMaterial>(m_vecResPath[i], m_vecResPath[i]);
			break;
		case RES_TYPE::MESH:

			break;
		case RES_TYPE::TEXTURE:
			CResMgr::GetInst()->Load<CTexture>(m_vecResPath[i], m_vecResPath[i]);
			break;
		case RES_TYPE::SOUND:

			break;
		case RES_TYPE::SCENEFILE:
			CResMgr::GetInst()->Load<CSceneFile>(m_vecResPath[i], m_vecResPath[i]);
			break;
		case RES_TYPE::XMLData:
			CResMgr::GetInst()->Load<CXMLData>(m_vecResPath[i], m_vecResPath[i]);
			break;
		}
	}

	// 3. �ε��Ǿ��ִ� ���ҽ����� ���� ���Ϸ� �����ϴ��� Ȯ��
	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		wstring strContentPath = CPathMgr::GetInst()->GetContentPath();

		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList((RES_TYPE)i);
		for (const auto& pair : mapRes)
		{
			if (pair.second->IsEngineRes())
				continue;

			// File Exist üũ
			if (!filesystem::exists(strContentPath + pair.second->GetRelativePath()))
			{
				// 4. ������ ���ҽ��Ŵ������� �޸� ����
				MessageBox(nullptr, L"�������� ���� ��", L"���Ϻ��� ����", MB_OK);

				if (0 == pair.second->GetRefCount())
				{
					// ����
					tEventInfo info;
					info.eType = EVENT_TYPE::DELETE_RES;
					info.lParam = (DWORD_PTR)pair.second;
					CEventMgr::GetInst()->AddEvent(info);

					MessageBox(nullptr, L"���ҽ� �޸� ����", L"���Ϻ��� ����", MB_OK);
				}

				else
				{
					MessageBox(nullptr, L"��� ���� ���ҽ�\n �޸� ���� ����", L"���Ϻ��� ����", MB_OK);
				}
			}
		}
	}
}

void ResourceUI::Renew()
{
	m_pTreeUI->Clear();

	// ���ҽ��� Tree �� ǥ��
	for (int i = 0; i < (int)RES_TYPE::END; ++i)
	{
		// TreeUI �� DummyRoot �� ����ϱ� ������, ���ҽ� �׸� ������ ���� �ڽ����� ��
		TreeNode* pResNode = m_pTreeUI->AddTreeNode(nullptr, ToString((RES_TYPE)i));

		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList((RES_TYPE)i);

		for (const auto& pair : mapRes)
		{
			// �� ���ҽ� ������ �ش� ���ҽ� �׸� �ڽ����� ��
			m_pTreeUI->AddTreeNode(pResNode, string(pair.first.begin(), pair.first.end()), (DWORD_PTR)pair.second);
		}
	}
}

void ResourceUI::FindFileName(const wstring& _strFolderPath)
{
	wstring strContent = _strFolderPath + L"*.*";

	// ���� ������ ����ü
	WIN32_FIND_DATA FindFileData = {};

	// ���� �˻� �ڵ�
	HANDLE hFind = nullptr;

	// ������ ����� ù��° ������ HANDLE�� ��� ��ȯ
	hFind = FindFirstFile(strContent.c_str(), &FindFileData);

	// ������ ������ return
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	// FindNextFile : FindFirstFile() ������ ������ �������� ã���ش�
	while (FindNextFile(hFind, &FindFileData))
	{
		// ã�� �����Ͱ� ���丮(����)�� ���
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// ���丮���� ..�� ��� �Ѿ�� 
			if (0 == wcscmp(FindFileData.cFileName, L".."))
				continue;

			// ���丮 ������ �� ���� ã�⸦ ����Ѵ�
			FindFileName(_strFolderPath + FindFileData.cFileName + L"\\");
			continue;
		}

		// ã�� �����Ͱ� ������ ���,
		// �ش� ������ ��� ��θ� m_vecResPath�� ����ش� 
		wstring strRelativePath = _strFolderPath + FindFileData.cFileName;
		strRelativePath = CPathMgr::GetInst()->GetRelativePath(strRelativePath);

		m_vecResPath.push_back(strRelativePath);

	}

	// ���� �˻� �ڵ��� �ݴ´�
	FindClose(hFind);
}

void ResourceUI::ItemClicked(DWORD_PTR _dwNode)
{
	TreeNode* pNode = (TreeNode*)_dwNode;

	string strKey = pNode->GetName();
	CRes* pResource = (CRes*)pNode->GetData();

	// ������ ��尡 ���ȴٸ� �ƹ��ϵ� ����.
	if (nullptr == pResource)
		return;

	// InspectorUI �� ����
	InspectorUI* pInspectorUI = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
	pInspectorUI->SetTargetResource(pResource);
}
void ResourceUI::ItemRightClicked(DWORD_PTR _dwNode)
{
	TreeNode* pNode = (TreeNode*)_dwNode;

	// ��忡 ��� ���ҽ� ������ ��������
	m_pRClickedRes = (CRes*)pNode->GetData();

	// ������ ��尡 ���ȴٸ� �ƹ��ϵ� ����.
	if (nullptr == m_pRClickedRes)
		return;

	// Toggle �ѱ�
	m_bToggleOn = true;
}


void ResourceUI::ItemDoubleClicked(DWORD_PTR _dwNode)
{
	TreeNode* pNode = (TreeNode*)_dwNode;

	string strKey = pNode->GetName();
	CRes* pResource = (CRes*)pNode->GetData();

	// ������ ��尡 ���Ȱų� SceneFile�� �ƴ� ���ҽ� ��带 �����ٸ� return
	if (nullptr == pResource || pNode->GetParent()->GetName() != "SCENEFILE")
		return;

	// Scene �ε�
	// ���� Scene ����
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	pCurScene->SetSceneState(SCENE_STATE::STOP);

	// �ε��� Scene ������ ��� ���
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += pResource->GetRelativePath();

	// �ε��� ���� ���� ������ ����
	CScene* pNewScene = CSceneSaveLoad::LoadScene(strFilePath);
	CSceneMgr::GetInst()->ChangeScene(pNewScene);
}

void ResourceUI::DragAndDropDelegate(DWORD_PTR _dwDrag, DWORD_PTR _dwDrop)
{
	// Prefab -> SceneOutliner
	//CPrefab* pPref = (CPrefab*) CResMgr::GetInst()->FindRes<CPrefab>(wstring(m_TreeUI->GetDragNode()->GetName().begin(), m_TreeUI->GetDragNode()->GetName().end())).Get();
	//if (nullptr != pPref)
	//{
	//	tEventInfo info = {};

	//	info.eType = EVENT_TYPE::CREATE_OBJ;
	//	info.lParam = (DWORD_PTR)pPref->Instantiate();
	//	info.wParam = (DWORD_PTR)1;

	//	CEventMgr::GetInst()->AddEvent(info);
	//}

	// SceneOutliner -> Resource
	//CGameObject* pObject = (CGameObject*)_dwDrag;
	//TreeNode* pDropTargetObject = m_TreeUI->GetDropNode();
	//
	//// ��� ������
	//if (nullptr != pDropTargetObject)
	//{
	//	if (m_TreeUI->GetDummyNode() == pDropTargetObject->GetParent() &&
	//		"PREFAB" == pDropTargetObject->GetName())
	//	{
	//		CPrefab* pPrf = new CPrefab;
	//		pPrf->SetProto(pObject->Clone());
	//		wstring name = L"prefab\\" + pObject->GetName() + L".pref";
	//		CResMgr::GetInst()->AddRes<CPrefab>(name, pPrf, false);

	//		Renew();
	//	}
	//}
}

RES_TYPE ResourceUI::GetResTypeFromExt(const wstring& _strExt)
{
	wchar_t szExt[50] = {};
	_wsplitpath_s(_strExt.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, 50);

	wstring strExt = szExt;

	if (L".mtrl" == strExt)
		return RES_TYPE::MATERIAL;
	
	else if (L".png" == strExt || L".jpeg" == strExt || L".bmp" == strExt
		|| L".jpg" == strExt || L".tga" == strExt || L".dds" == strExt)
		return RES_TYPE::TEXTURE;
	
	else if (L".mp3" == strExt || L".wav" == strExt || L".ogg" == strExt)
		return RES_TYPE::SOUND;
	
	else if (L".pref" == strExt)
		return RES_TYPE::PREFAB;
	
	else if (L".mesh" == strExt)
		return RES_TYPE::MESH;
	
	else if (L".scene" == strExt)
		return RES_TYPE::SCENEFILE;

	else if (L".xml" == strExt)
		return RES_TYPE::XMLData;

	return RES_TYPE::END;
}


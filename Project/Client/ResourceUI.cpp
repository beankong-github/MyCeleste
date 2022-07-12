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

	// Clicked Delegate 등록
	m_pTreeUI->SetClickedDelegate(this, (CLICKED)&ResourceUI::ItemClicked);

	// Right Delegate 등록
	m_pTreeUI->SetRightClickedDelegate(this, (CLICKED)&ResourceUI::ItemRightClicked);

	// Double Clicked Delegate 등록
	m_pTreeUI->SetDoubleClickedDelegate(this, (CLICKED)&ResourceUI::ItemDoubleClicked);
	// Drag Drop Delegate 등록
	m_pTreeUI->SetDragAndDropDelegate(this, (DRAG_DROP)&ResourceUI::DragAndDropDelegate);

	Reset();
}

ResourceUI::~ResourceUI()
{
}

void ResourceUI::update()
{
	// 리소스 변경상태 저장
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
		// 키 검사
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
	// Content 폴더 밑의 리소스 로딩
	Reload();

	Renew();
}

void ResourceUI::Reload()
{
	// Content 폴더 밑으로 모든 파일의 상대경로를 알아낸다.
	m_vecResPath.clear();
	FindFileName(CPathMgr::GetInst()->GetContentPath());

	// 키값 확인
	for (size_t i = 0; i < m_vecResPath.size(); ++i)
	{
		// 1. 파일명을 통해서 리소스의 종류를 알아낸다.
		RES_TYPE eType = GetResTypeFromExt(m_vecResPath[i]);

		// 2. 상대경로를 키값으로 해서 리소스를 로딩한다.
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

	// 3. 로딩되어있는 리소스들이 실제 파일로 존재하는지 확인
	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		wstring strContentPath = CPathMgr::GetInst()->GetContentPath();

		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList((RES_TYPE)i);
		for (const auto& pair : mapRes)
		{
			if (pair.second->IsEngineRes())
				continue;

			// File Exist 체크
			if (!filesystem::exists(strContentPath + pair.second->GetRelativePath()))
			{
				// 4. 없으면 리소스매니저에서 메모리 해제
				MessageBox(nullptr, L"원본파일 삭제 됨", L"파일변경 감지", MB_OK);

				if (0 == pair.second->GetRefCount())
				{
					// 삭제
					tEventInfo info;
					info.eType = EVENT_TYPE::DELETE_RES;
					info.lParam = (DWORD_PTR)pair.second;
					CEventMgr::GetInst()->AddEvent(info);

					MessageBox(nullptr, L"리소스 메모리 해제", L"파일변경 감지", MB_OK);
				}

				else
				{
					MessageBox(nullptr, L"사용 중인 리소스\n 메모리 해제 실패", L"파일변경 감지", MB_OK);
				}
			}
		}
	}
}

void ResourceUI::Renew()
{
	m_pTreeUI->Clear();

	// 리소스를 Tree 에 표시
	for (int i = 0; i < (int)RES_TYPE::END; ++i)
	{
		// TreeUI 가 DummyRoot 를 사용하기 때문에, 리소스 항목 노드들은 더미 자식으로 들어감
		TreeNode* pResNode = m_pTreeUI->AddTreeNode(nullptr, ToString((RES_TYPE)i));

		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList((RES_TYPE)i);

		for (const auto& pair : mapRes)
		{
			// 각 리소스 노드들은 해당 리소스 항목 자식으로 들어감
			m_pTreeUI->AddTreeNode(pResNode, string(pair.first.begin(), pair.first.end()), (DWORD_PTR)pair.second);
		}
	}
}

void ResourceUI::FindFileName(const wstring& _strFolderPath)
{
	wstring strContent = _strFolderPath + L"*.*";

	// 파일 데이터 구조체
	WIN32_FIND_DATA FindFileData = {};

	// 파일 검색 핸들
	HANDLE hFind = nullptr;

	// 지정한 경로의 첫번째 파일을 HANDLE에 담아 반환
	hFind = FindFirstFile(strContent.c_str(), &FindFileData);

	// 파일이 없으면 return
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	// FindNextFile : FindFirstFile() 다음의 파일을 차근차근 찾아준다
	while (FindNextFile(hFind, &FindFileData))
	{
		// 찾은 데이터가 디렉토리(폴더)인 경우
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// 디렉토리명이 ..인 경우 넘어간다 
			if (0 == wcscmp(FindFileData.cFileName, L".."))
				continue;

			// 디렉토리 안으로 들어가 파일 찾기를 계속한다
			FindFileName(_strFolderPath + FindFileData.cFileName + L"\\");
			continue;
		}

		// 찾은 데이터가 파일인 경우,
		// 해당 파일의 상대 경로를 m_vecResPath에 담아준다 
		wstring strRelativePath = _strFolderPath + FindFileData.cFileName;
		strRelativePath = CPathMgr::GetInst()->GetRelativePath(strRelativePath);

		m_vecResPath.push_back(strRelativePath);

	}

	// 파일 검색 핸들을 닫는다
	FindClose(hFind);
}

void ResourceUI::ItemClicked(DWORD_PTR _dwNode)
{
	TreeNode* pNode = (TreeNode*)_dwNode;

	string strKey = pNode->GetName();
	CRes* pResource = (CRes*)pNode->GetData();

	// 프레임 노드가 눌렸다면 아무일도 없다.
	if (nullptr == pResource)
		return;

	// InspectorUI 를 얻어옴
	InspectorUI* pInspectorUI = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
	pInspectorUI->SetTargetResource(pResource);
}
void ResourceUI::ItemRightClicked(DWORD_PTR _dwNode)
{
	TreeNode* pNode = (TreeNode*)_dwNode;

	// 노드에 담긴 리소스 데이터 가져오기
	m_pRClickedRes = (CRes*)pNode->GetData();

	// 프레임 노드가 눌렸다면 아무일도 없다.
	if (nullptr == m_pRClickedRes)
		return;

	// Toggle 켜기
	m_bToggleOn = true;
}


void ResourceUI::ItemDoubleClicked(DWORD_PTR _dwNode)
{
	TreeNode* pNode = (TreeNode*)_dwNode;

	string strKey = pNode->GetName();
	CRes* pResource = (CRes*)pNode->GetData();

	// 프레임 노드가 눌렸거나 SceneFile이 아닌 리소스 노드를 눌렀다면 return
	if (nullptr == pResource || pNode->GetParent()->GetName() != "SCENEFILE")
		return;

	// Scene 로딩
	// 현재 Scene 정지
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	pCurScene->SetSceneState(SCENE_STATE::STOP);

	// 로딩할 Scene 파일의 경로 계산
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += pResource->GetRelativePath();

	// 로드한 씬을 현재 씬으로 지정
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
	//// 드롭 목적지
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


#include "pch.h"
#include "SceneOutliner.h"

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>

#include <Engine/CEventMgr.h>
#include <Engine/CKeyMgr.h>

#include "CImGuiMgr.h"
#include "TreeUI.h"
#include "InspectorUI.h"
#include "ResourceUI.h"

SceneOutliner::SceneOutliner()
	: UI("SceneOutliner")
	, m_bToggleOn(false)
{
	m_pTreeUI = new TreeUI(true);
	m_pTreeUI->SetTitle("SceneOutliner");
	m_pTreeUI->UseFrame(false);
	m_pTreeUI->ShowDummyRoot(false);
	m_pTreeUI->UseDragDropOuter(true);
	m_pTreeUI->UseDragDropSelf(true);


	AddChild(m_pTreeUI);

	// Clicked Delegate 등록
	m_pTreeUI->SetClickedDelegate(this, (CLICKED)&SceneOutliner::OnObjectNodeClicked);

	// Right Delegate 등록
	m_pTreeUI->SetRightClickedDelegate(this, (CLICKED) & SceneOutliner::OnRightClicked);

	// Drag and Drop Delegate 등록
	m_pTreeUI->SetDragAndDropDelegate(this, (DRAG_DROP)&SceneOutliner::DragAndDropDelegate);

	// Key Delegate 등록
	m_pTreeUI->SetKeyBinding(KEY::DEL, this, (CLICKED)&SceneOutliner::OnPressDelete);
	
	// 외부UI로부터 Drop 체크 Delegate 등록
	m_pTreeUI->SetDropCheckDelegate(this, (DROPCHECK)&SceneOutliner::DropCheckDelegate);

	Reset();
}

SceneOutliner::~SceneOutliner()
{

}

void SceneOutliner::update()
{
	if (CEventMgr::GetInst()->HasOccurObjEvent())
	{
		Reset();
	}

	UI::update();
}

void SceneOutliner::render_update()
{
	// 우클릭 감지
	//Vec2 vWinLT = Vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	//Vec2 vWinRB = vWinLT + Vec2(ImGui::GetWindowContentRegionMax().x, ImGui::GetWindowContentRegionMax().y);
	//if (ImGui::IsMouseHoveringRect(vWinLT, vWinRB) && ImGui::IsMouseClicked(ImGuiButtonFlags_MouseButtonRight >> 1))
	//	OnRightClicked();


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
		if (ImGui::MenuItem("Copy"))
		{
			tEventInfo info = {};

			info.eType = EVENT_TYPE::CREATE_OBJ;
			info.lParam = (DWORD_PTR)m_pRClickedOject->Clone();
			info.wParam = (DWORD_PTR)m_pRClickedOject->GetLayerIndex();
			CEventMgr::GetInst()->AddEvent(info);

			m_pRClickedOject = nullptr;
			m_bToggleOn = false;
		}

		if (ImGui::MenuItem("Delete"))
		{
			tEventInfo info = {};

			info.eType = EVENT_TYPE::DELETE_OBJ;
			info.lParam = (DWORD_PTR)m_pRClickedOject;
			CEventMgr::GetInst()->AddEvent(info);

			m_pRClickedOject = nullptr;
			m_bToggleOn = false;
		}

		if (ImGui::MenuItem("Make Prefab"))
		{
			// 프리팹 추가
			MakePrefab(m_pRClickedOject);

			m_pRClickedOject = nullptr;
			m_bToggleOn = false;
		}

		ImGui::EndPopup();
	}
}

void SceneOutliner::Reset()
{
	m_pTreeUI->Clear();

	// 현재 Scene 을 가져온다.
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurScene->GetLayer(i);

		vector<CGameObject*>& vecRoots = pLayer->GetRootObjects();

		for (size_t i = 0; i < vecRoots.size(); ++i)
		{
			AddGameObjectToTree(vecRoots[i], m_pTreeUI->GetDummyNode());
		}
	}

	// InspectorUI의 타겟 Object를 비워준다
	InspectorUI* pInspectorUI = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
	pInspectorUI->SetTargetObject(nullptr);
}

void SceneOutliner::OnObjectNodeClicked(DWORD_PTR _dw)
{
	TreeNode* pNode = (TreeNode*)_dw;

	string strKey = pNode->GetName();
	CGameObject* pObject = (CGameObject*)pNode->GetData();
		
	assert(pObject);		

	// InspectorUI 를 얻어옴
	InspectorUI* pInspectorUI = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
	pInspectorUI->SetTargetObject(pObject);
}

void SceneOutliner::MakePrefab(CGameObject* _pObj)
{
	wstring key = L"prefab\\" + _pObj->GetName() + L".pref";

	CPrefab* pref = CResMgr::GetInst()->FindRes<CPrefab>(key).Get();

	// 중복키가 있다면
	if (nullptr != pref)
	{
		map<wstring, CRes*> mPrefs = CResMgr::GetInst()->GetResList(RES_TYPE::PREFAB);
		map<wstring, CRes*>::iterator iter = mPrefs.begin();

		int count = 0;
		for (; iter != mPrefs.end(); ++iter)
		{
			CPrefab* p = (CPrefab*)iter->second;
			if (p->GetProto()->GetName() == _pObj->GetName())
			{
				++count;
			}
		}

		key = L"prefab\\" + _pObj->GetName() + L"("+ std::to_wstring(count) + L")" + L".pref";
	}

	CPrefab* pPref = new CPrefab;
	pPref->SetProto(_pObj->Clone());
	CResMgr::GetInst()->AddRes<CPrefab>(key, pPref);

	// 리소스 뷰 갱신
	ResourceUI* pResUI = (ResourceUI*)CImGuiMgr::GetInst()->FindUI("Resource");
	if (nullptr != pResUI)
	{
		pResUI->Reset();
	}
}

void SceneOutliner::AddGameObjectToTree(CGameObject* _pObject, TreeNode* _pDestNode)
{
	TreeNode* pNode = m_pTreeUI->AddTreeNode(_pDestNode
											, string(_pObject->GetName().begin(), _pObject->GetName().end())
											, (DWORD_PTR)_pObject);

	const vector<CGameObject*>& vecChild = _pObject->GetChild();

	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		AddGameObjectToTree(vecChild[i], pNode);
	}
}

void SceneOutliner::OnRightClicked(DWORD_PTR _dw)
{
	TreeNode* pNode = (TreeNode*)_dw;

	string strKey = pNode->GetName();
	m_pRClickedOject = (CGameObject*)pNode->GetData();

	assert(m_pRClickedOject);

	m_bToggleOn = true;
}

void SceneOutliner::OnPressDelete(DWORD_PTR _dw)
{
	TreeNode* pNode = (TreeNode*)_dw;

	if (nullptr == pNode)
		return;

	CGameObject* pTargetObj = (CGameObject*)pNode->GetData();
	pTargetObj->Destroy();

	// InspectorUI 를 찾아서 Object 를 nullptr 로 세팅한다.
	InspectorUI* pInspectorUI = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
	pInspectorUI->SetTargetObject(nullptr);
}

void SceneOutliner::DragAndDropDelegate(DWORD_PTR _dwDrag, DWORD_PTR _dwDrop)
{
	CGameObject* pChildObject = (CGameObject*)_dwDrag;
	CGameObject* pDropTargetObject = (CGameObject*)_dwDrop;


	// 드롭 목적지가 제대로 들어 온 경우
	if (nullptr != pDropTargetObject)
	{
		// 같은 오브젝트에 드롭했거나 현재 오브젝트의 부모에 드롭했다면 실행 x
		if (pChildObject == pDropTargetObject
			|| pDropTargetObject->IsAncestor(pChildObject))
		{
			return;
		}

		CSceneMgr::GetInst()->AddChild(pDropTargetObject, pChildObject);
	}

	// 자식 오브젝트의 목적지가 nullptr 인 경우
	else
	{
		// 이미 최상위 부모 오브젝트는 이벤트 처리가 필요없다.
		if (nullptr == pChildObject->GetParent())
		{
			return;
		}

		CSceneMgr::GetInst()->DisconnectParent(pChildObject);		
	}
}

bool SceneOutliner::DropCheckDelegate()
{
	// Resource -> SceneOuliner
	if (ImGui::BeginDragDropTarget())
	{
		DWORD_PTR dwData = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource"))
		{
			memcpy(&dwData, payload->Data, sizeof(DWORD_PTR));
			ResDrop(dwData);
		}

		ImGui::EndDragDropTarget();

		return (dwData != NULL);
	}
}

void SceneOutliner::ResDrop(DWORD_PTR _resPtr)
{
	ResourceUI* pResUI = (ResourceUI*) CImGuiMgr::GetInst()->FindUI("Resource");
	assert(pResUI);

	TreeNode* node = pResUI->GetTreeUI()->GetDragNodeforOther();
	if (node != nullptr)
	{
		wstring name = wstring(node->GetName().begin(), node->GetName().end());
		CPrefab* pPref = (CPrefab*)CResMgr::GetInst()->FindRes<CPrefab>(name).Get();
		
		tEventInfo info = {};

		info.eType = EVENT_TYPE::CREATE_OBJ;
		info.lParam = (DWORD_PTR)pPref->Instantiate();
		info.wParam = (DWORD_PTR)1;

		CEventMgr::GetInst()->AddEvent(info);

		pResUI->GetTreeUI()->SetDragNodeforOther(nullptr);
	}
}

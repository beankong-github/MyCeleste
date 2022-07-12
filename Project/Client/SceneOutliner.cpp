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

	// Clicked Delegate ���
	m_pTreeUI->SetClickedDelegate(this, (CLICKED)&SceneOutliner::OnObjectNodeClicked);

	// Right Delegate ���
	m_pTreeUI->SetRightClickedDelegate(this, (CLICKED) & SceneOutliner::OnRightClicked);

	// Drag and Drop Delegate ���
	m_pTreeUI->SetDragAndDropDelegate(this, (DRAG_DROP)&SceneOutliner::DragAndDropDelegate);

	// Key Delegate ���
	m_pTreeUI->SetKeyBinding(KEY::DEL, this, (CLICKED)&SceneOutliner::OnPressDelete);
	
	// �ܺ�UI�κ��� Drop üũ Delegate ���
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
	// ��Ŭ�� ����
	//Vec2 vWinLT = Vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	//Vec2 vWinRB = vWinLT + Vec2(ImGui::GetWindowContentRegionMax().x, ImGui::GetWindowContentRegionMax().y);
	//if (ImGui::IsMouseHoveringRect(vWinLT, vWinRB) && ImGui::IsMouseClicked(ImGuiButtonFlags_MouseButtonRight >> 1))
	//	OnRightClicked();


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
			// ������ �߰�
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

	// ���� Scene �� �����´�.
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

	// InspectorUI�� Ÿ�� Object�� ����ش�
	InspectorUI* pInspectorUI = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
	pInspectorUI->SetTargetObject(nullptr);
}

void SceneOutliner::OnObjectNodeClicked(DWORD_PTR _dw)
{
	TreeNode* pNode = (TreeNode*)_dw;

	string strKey = pNode->GetName();
	CGameObject* pObject = (CGameObject*)pNode->GetData();
		
	assert(pObject);		

	// InspectorUI �� ����
	InspectorUI* pInspectorUI = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
	pInspectorUI->SetTargetObject(pObject);
}

void SceneOutliner::MakePrefab(CGameObject* _pObj)
{
	wstring key = L"prefab\\" + _pObj->GetName() + L".pref";

	CPrefab* pref = CResMgr::GetInst()->FindRes<CPrefab>(key).Get();

	// �ߺ�Ű�� �ִٸ�
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

	// ���ҽ� �� ����
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

	// InspectorUI �� ã�Ƽ� Object �� nullptr �� �����Ѵ�.
	InspectorUI* pInspectorUI = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
	pInspectorUI->SetTargetObject(nullptr);
}

void SceneOutliner::DragAndDropDelegate(DWORD_PTR _dwDrag, DWORD_PTR _dwDrop)
{
	CGameObject* pChildObject = (CGameObject*)_dwDrag;
	CGameObject* pDropTargetObject = (CGameObject*)_dwDrop;


	// ��� �������� ����� ��� �� ���
	if (nullptr != pDropTargetObject)
	{
		// ���� ������Ʈ�� ����߰ų� ���� ������Ʈ�� �θ� ����ߴٸ� ���� x
		if (pChildObject == pDropTargetObject
			|| pDropTargetObject->IsAncestor(pChildObject))
		{
			return;
		}

		CSceneMgr::GetInst()->AddChild(pDropTargetObject, pChildObject);
	}

	// �ڽ� ������Ʈ�� �������� nullptr �� ���
	else
	{
		// �̹� �ֻ��� �θ� ������Ʈ�� �̺�Ʈ ó���� �ʿ����.
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

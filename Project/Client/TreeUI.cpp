#include "pch.h"
#include "TreeUI.h"

#include "ImGui/imgui_internal.h"

// ========
// TreeNode
// ========
TreeNode::TreeNode()
	: m_pTreeUI(nullptr)
	, m_pParent(nullptr)
	, m_bLeaf(true)
	, m_bSelected(false)
	, m_dwData(0)
{
}

TreeNode::TreeNode(const string& _strName, DWORD_PTR _dwData)
	: m_pTreeUI(nullptr)
	, m_pParent(nullptr)
	, m_bLeaf(true)
	, m_bSelected(false)
	, m_strName(_strName)
	, m_dwData(_dwData)
{
}

TreeNode::~TreeNode()
{
	Safe_Del_Vec(m_vecChild);
}


void TreeNode::update()
{
	// update 작성


	// child update
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->update();
	}
}

static int a = 0;

void TreeNode::render_update()
{
	// ImGuiTreeNodeFlags_	
	ImGuiTreeNodeFlags eFlag = 0;

	if (m_bLeaf)
		eFlag |= ImGuiTreeNodeFlags_Leaf;
	if (m_pTreeUI->m_bUseFrame &&  (nullptr == m_pParent || m_pParent->m_strName == "DummyRoot"))
		eFlag |= ImGuiTreeNodeFlags_Framed;
	if (m_bSelected)
		eFlag |= ImGuiTreeNodeFlags_Selected;
	
	if (ImGui::TreeNodeEx(m_strName.c_str(), eFlag))
	{	
		// 노드 클릭 체크
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{			
			m_pTreeUI->SetSelectedNode(this);
		}

		// 노드 우클릭 체크
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{			
			m_pTreeUI->SetRightClickedNode(this);
		}

		// 노드 더블 클릭 체크
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			m_pTreeUI->SetDoubleClickedNode(this);
		}

		// 내부 또는 외부로 드래그 드롭을 사용 할 경우
		if (m_pTreeUI->m_bUseDragDropSelf || m_pTreeUI->m_bUseDragDropOuter)
		{
			if (ImGui::BeginDragDropSource())
			{				
				ImGui::SetDragDropPayload(m_pTreeUI->GetParentUI()->GetName().c_str()
					, &m_dwData, sizeof(DWORD_PTR));
				ImGui::Text(m_strName.c_str());
				ImGui::EndDragDropSource();

				m_pTreeUI->m_pDragNode = this;
				m_pTreeUI->m_pDragNodeforOther = this;
			}

			// 내부 드롭을 허용한 경우에 드롭이 발생했는지 체크한다.
			if (m_pTreeUI->m_bUseDragDropSelf)
			{
				if (ImGui::BeginDragDropTarget())
				{
					DWORD_PTR dwData = 0;
					const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(m_pTreeUI->GetParentUI()->GetName().c_str());
					if (nullptr != payload)
					{
						memcpy(&dwData, payload->Data, sizeof(DWORD_PTR));

						// Drop Node 를 알림
						m_pTreeUI->m_pDropNode = this;
					}

					ImGui::EndDragDropTarget();
				}
			}
		}
		
		for (size_t i = 0; i < m_vecChild.size(); ++i)
		{
			m_vecChild[i]->render_update();
		}

		ImGui::TreePop();
	}
}




// ======
// TreeUI
// ======
TreeUI::TreeUI(bool _bDummyRoot)
	: UI("##TreeUI")
	, m_pRootNode(nullptr)
	, m_pSelectedNode(nullptr)
	, m_pDragNode(nullptr)
	, m_pDropNode(nullptr)
	, m_pDragNodeforOther(nullptr)
	, m_bUseDummyRoot(_bDummyRoot)
	, m_bShowDummy(false)	
	, m_bUseFrame(false)
	, m_bUseDragDropSelf(false)
	, m_bUseDragDropOuter(false)
	, m_pCInst(nullptr)
	, m_CFunc(nullptr)
	, m_pDBCInst(nullptr)
	, m_DBCFunc(nullptr)
	, m_pDADInst(nullptr)
	, m_DADFunc(nullptr)
{
	if (m_bUseDummyRoot)
	{
		AddTreeNode(nullptr, "DummyRoot");
	}
}

TreeUI::~TreeUI()
{
	SAFE_DELETE(m_pRootNode);
}

void TreeUI::update()
{
	if (nullptr == m_pRootNode)
	{
		m_pSelectedNode = nullptr;
		return;
	}
	
	// 트리UI 가 부착된 부모 UI 의 사이즈를 받아온다.
	Vec2 vSize = GetParentUI()->GetSize();
	vSize.y -= 39.f;
	SetSize(vSize);

	m_pRootNode->update();
		

	// 자식 UI 업데이트
	UI::update();
}

void TreeUI::render_update()
{
	if (nullptr == m_pRootNode)
		return;

	if (m_bUseDummyRoot)
	{
		if(m_bShowDummy)
			m_pRootNode->render_update();
		else
		{
			for (size_t i = 0; i < m_pRootNode->m_vecChild.size(); ++i)
			{
				m_pRootNode->m_vecChild[i]->render_update();
			}
		}
	}

	else
	{
		m_pRootNode->render_update();
	}

	// Drop이 발생했을 때 Drag로 등록된 노드와 Drop으로 등록된 노드를 인자로 함수 포인터를 호출한다.
	if ( (m_pDragNode && m_pDropNode)
		|| m_pDragNode && KEY_AWAY(KEY::LBTN))
	{
		if (m_pDADInst && m_DADFunc)
		{
			if (nullptr == m_pDropNode)
			{
				(m_pDADInst->*m_DADFunc)(m_pDragNode->GetData(), 0);
			}
			else
			{
				(m_pDADInst->*m_DADFunc)(m_pDragNode->GetData(), m_pDropNode->GetData());
			}
			
		}
		m_pDragNode = nullptr;
		m_pDropNode = nullptr;
	}


	// KeyBinding 호출
	if (ImGui::IsWindowFocused())
	{
		for (size_t i = 0; i < m_vecKeyBind.size(); ++i)
		{
			if (KEY_TAP(m_vecKeyBind[i].eKey))
			{
				(m_vecKeyBind[i].pInst->*m_vecKeyBind[i].pFunc)((DWORD_PTR)m_pSelectedNode);
			}
		}
	}
}

void TreeUI::DropCheck()
{
	// 외부(특정 UI)에서 발생한 드롭이 있는지 체크한다
	if (m_pDCheckInst && m_DCheckFunc)
		m_bIsDropFromOuter = (m_pDCheckInst->*m_DCheckFunc)();
}

TreeNode* TreeUI::AddTreeNode(TreeNode* _pParentNode, const string& _strName, DWORD_PTR _dwData)
{
	TreeNode* pNewNode = new TreeNode(_strName, _dwData);
	pNewNode->m_pTreeUI = this;

	// 부모를 지정함
	if (nullptr != _pParentNode)
	{
		_pParentNode->AddChild(pNewNode);
	}
	// 부모가 지정되지 않고
	else if (nullptr == _pParentNode)
	{
		// 루트노드가 존재함
		if (nullptr != m_pRootNode)
			m_pRootNode->AddChild(pNewNode);

		// 루트노드가 존재하지 않음
		else
			m_pRootNode = pNewNode;
	}	

	return pNewNode;
}


void TreeUI::SetKeyBinding(KEY _eKey, UI* _pInst, KEY_FUNC _Func)
{
	m_vecKeyBind.push_back(tTreeKey{_eKey, _pInst, _Func});
}

void TreeUI::Clear()
{
	SAFE_DELETE(m_pRootNode);
	m_pRootNode = nullptr;
	m_pSelectedNode = nullptr;

	if (m_bUseDummyRoot)
	{
		AddTreeNode(nullptr, "DummyRoot");
	}
}

void TreeUI::SetSelectedNode(TreeNode* _pNode)
{
	// 이전에 선택된 노드가 있으면 초기화하고 새로 선택된 노드를 등록한다
	if (nullptr != m_pSelectedNode)
	{
		m_pSelectedNode->m_bSelected = false;
	}

	m_pSelectedNode = _pNode;
	m_pSelectedNode->m_bSelected = true;


	// 델리게이트 호출
	if (nullptr != m_pCInst && nullptr != m_CFunc)
	{
		(m_pCInst->*m_CFunc)((DWORD_PTR)m_pSelectedNode);
	}	
}

void TreeUI::SetRightClickedNode(TreeNode* _pNode)
{
	// 이전에 선택된 노드가 있으면 초기화하고 새로 선택된 노드를 등록한다
	if (nullptr != m_pRightClickedNode)
	{
		m_pRightClickedNode->m_bSelected = false;
	}

	m_pRightClickedNode = _pNode;
	m_pRightClickedNode->m_bSelected = true;


	// 델리게이트 호출
	if (nullptr != m_pRCInst && nullptr != m_RCFunc)
	{
		(m_pRCInst->*m_RCFunc)((DWORD_PTR)m_pRightClickedNode);
	}
}

void TreeUI::SetDoubleClickedNode(TreeNode* _pNode)
{	
	// 델리게이트 호출
	if (nullptr != m_pDBCInst && nullptr != m_DBCFunc)
	{
		(m_pDBCInst->*m_DBCFunc)((DWORD_PTR)m_pSelectedNode);
	}
}

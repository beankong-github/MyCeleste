#include "pch.h"
#include "ToggleUI.h"

// Toggle Node ========================================================
ToggleNode::ToggleNode()
	: m_pParent(nullptr)
	, m_pToggleUI(nullptr)
	, m_bHasChild(false)
	, m_bSelected(false)
	, m_strName("")
	, m_vecChild{nullptr}
{
}

ToggleNode::ToggleNode(const string& _strName)
	: m_pParent(nullptr)
	, m_pToggleUI(nullptr)
	, m_bHasChild(false)
	, m_bSelected(false)
	, m_strName(_strName)
	, m_vecChild{ nullptr }
{
}

ToggleNode::~ToggleNode()
{
	Safe_Del_Vec(m_vecChild);
}

void ToggleNode::update()
{
}

void ToggleNode::render_update()
{
	if (m_bHasChild)
	{
		if (ImGui::BeginMenu(m_strName.c_str()))
		{
			for (size_t i = 0; i < m_vecChild.size(); i++)
			{
				m_vecChild[i]->render_update();
			}

			ImGui::EndMenu();
		}
	}

	else
	{
		ImGui::MenuItem(m_strName.c_str(), "", m_bSelected);

	}
}

void ToggleNode::CheckMouseEvent()
{
	//// 노드 클릭 체크
	//if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	//{
	//	m_pToggleUI->SetSelectedNode(this);
	//}

	//// 노드 더블 클릭 체크
	//if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	//{
	//	m_pToggleUI->SetDoubleClickedNode(this);
	//}
}

// Toggle UI =========================================================
ToggleUI::ToggleUI()
	: UI("ToggleUI")
	, m_vecItems{}
	, m_pCInst(nullptr)
	, m_CFunc(nullptr)
	, m_pDBCInst(nullptr)
	, m_DBCFunc(nullptr)
{
}

ToggleUI::~ToggleUI()
{
	Safe_Del_Vec(m_vecItems);
}


void ToggleUI::update()
{
	if (IsActive())
		ImGui::OpenPopup("##Popup Toggle");
}

void ToggleUI::render_update()
{
	if (ImGui::BeginPopup("##Popup Toggle"))
	{
		for (size_t i = 0; i < m_vecItems.size(); i++)
		{
			m_vecItems[i]->render_update();
		}

		ImGui::EndPopup();
	}
}

void ToggleUI::Clear()
{
}
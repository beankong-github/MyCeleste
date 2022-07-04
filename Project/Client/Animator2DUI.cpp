#include "pch.h"
#include "Animator2DUI.h"

#include "CImGuiMgr.h"
#include "Create2DAnimationUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>

Animator2DUI::Animator2DUI() :
	ComponentUI("Animator2DUI", COMPONENT_TYPE::ANIMATOR2D),
	pCreate2DAnimUI(nullptr),
	m_vecAnimList{},
	m_iAnimIdx(0)
{																	
	SetSize(Vec2(0.f, 120.f));
}

Animator2DUI::~Animator2DUI()
{
}

void Animator2DUI::update()
{
}

void Animator2DUI::render_update()
{
	ComponentUI::render_update();
	ImGui::SameLine(0.f, 3.f);
	//이 버튼을 누르면 Create2DAnimationUI 창이 열리도록 한다
	//버튼이 눌리고 창이 열릴 때 Animator2DUI가 가리키고 있던 TargetObject 정보를 넘겨주도록 한다

	ImGui::PushItemWidth(200.f);

	if (ImGui::Button("Create 2D Animation##1"))
	{
		//pCreate2DAnimUI->SetTargetObj(GetTargetObject());

		pCreate2DAnimUI->Activate();
	}

	ImGui::Bullet();
	ImGui::Text("Animation List");
	ImGui::SameLine(0.f, 10.f);

	// 애니메이션 결정할 ComboBox
	if (m_vecAnimList.size() > 0)
	{
		if (ImGui::BeginCombo("##Animation List Combo_1", m_vecAnimList[m_iAnimIdx].c_str()))
		{
			for (int i = 0; i < m_vecAnimList.size(); i++)
			{
				//if (ImGui::IsItemHovered(ImGuiHoveredFlags_))
				if (ImGui::Selectable(m_vecAnimList[i].c_str()))
				{
					m_iAnimIdx = i;
					wstring _wStr = wstring(m_vecAnimList[i].begin(), m_vecAnimList[i].end());
					GetTargetObject()->Animator2D()->Play(_wStr);
				}
			}
			ImGui::EndCombo();
		}
	}
}

void Animator2DUI::Activate()
{
	UI::Activate();
	
	//if (nullptr == GetTargetObject()) return;
	//if (nullptr == GetTargetObject()->GetComponent(COMPONENT_TYPE::ANIMATOR2D)) return;

	pCreate2DAnimUI = (Create2DAnimationUI*)CImGuiMgr::GetInst()->FindUI("Create 2D Animation UI");
	if (pCreate2DAnimUI != nullptr)
	{
		pCreate2DAnimUI->SetAddListEvent(this, (AnimListEvent)&Animator2DUI::AddAnimList);
		pCreate2DAnimUI->SetTargetObj(GetTargetObject());
	}

	const map<wstring, CAnimation2D*>& animList = GetTargetObject()->Animator2D()->GetAnimList();

	for (const auto& pair : animList)
	{
		m_vecAnimList.push_back(string(pair.first.begin(), pair.first.end()));
	}

}

void Animator2DUI::Deactivate()
{
	UI::Deactivate();

	pCreate2DAnimUI = nullptr;
}

void Animator2DUI::AddAnimList(DWORD_PTR _param)
{
	string AnimName = (char*)_param;

	m_vecAnimList.push_back(AnimName);
}

#include "pch.h"
#include "TransformUI.h"

#include <Engine/CTransform.h>

TransformUI::TransformUI()
	: ComponentUI("Transform", COMPONENT_TYPE::TRANSFORM)	
{
	SetSize(Vec2(0.f, 150.f));
}

TransformUI::~TransformUI()
{
}

void TransformUI::update()
{
	ComponentUI::update();

	CGameObject* pTargetObj = GetTargetObject();
	if (nullptr == pTargetObj)
		return;
		
	CTransform* pTransform = pTargetObj->Transform();

	m_vRelativePos = pTransform->GetRelativePos();
	m_vRelativeScale = pTransform->GetRelativeScale();
	m_vRelativeRot = pTransform->GetRelativeRotation();
}

void TransformUI::render_update()
{
	ComponentUI::render_update();

	CGameObject* pTargetObject = GetTargetObject();
	CTransform* pTrans = pTargetObject->Transform();
	Vec3 vPos = pTrans->GetRelativePos();
	Vec3 vScale = pTrans->GetRelativeScale();
	Vec3 vRot = pTrans->GetRelativeRotation();
	vRot.ToDegree();


	ImGui::PushItemWidth(200); // Float3 ???? ???? ????
	
	ImGui::Text("Relative Position");	
	ImGui::SameLine();
	ImGui::InputFloat3("##Position", vPos);
	pTrans->SetRelativePos(vPos);

	ImGui::Text("Relative Scale   ");
	ImGui::SameLine();	
	ImGui::InputFloat3("##Scale", vScale);
	pTrans->SetRelativeScale(vScale);

	ImGui::Text("Relative Rotation");
	ImGui::SameLine();	
	ImGui::InputFloat3("##Rotation", vRot);
	vRot.ToRadian();
	pTrans->SetRelativeRotation(vRot);

	ImGui::Text("IngnoreParentScale");
	ImGui::SameLine();
	m_bignore = pTrans->IsIgnoreParentScale();
	if(ImGui::Checkbox("##IngPrntScale", &m_bignore))
		pTrans->SetIgnoreParentScale(m_bignore);

	ImGui::Text("SameRatioWithSprite");
	ImGui::SameLine();
	m_bsame= pTrans->IsSameRatioWithSprite();
	if(ImGui::Checkbox("##SameRatio", &m_bsame))
		pTrans->SetSameRatioWithSprite(m_bsame);
}
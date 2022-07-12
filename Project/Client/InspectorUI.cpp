#include "pch.h"
#include "InspectorUI.h"

#include <Engine/CKeyMgr.h>
#include <Engine/CEventMgr.h>
#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CScript.h>

#include "CImGuiMgr.h"
#include "SceneOutliner.h"

#include "ScriptUI.h"
#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "CameraUI.h"
#include "TileMapUI.h"
#include "Collider2DUI.h"
#include "Animator2DUI.h"
#include "Light2DUI.h"

#include "MaterialUI.h"
#include "TextureUI.h"
#include "PrefabUI.h"
#include "CImGuiMgr.h"
#include "Create2DAnimationUI.h"

InspectorUI::InspectorUI()
	: UI("Inspector")
	, m_pTargetObject(nullptr)
	, m_pTargetRes(nullptr)
	, m_arrComUI{}
	, m_arrResUI{}
{
	// ================
	// ComponentUI ����   
	// ================
	ComponentUI* pComUI = nullptr;

	pComUI = new TransformUI;
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM] = pComUI;

	pComUI = new MeshRenderUI;	
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER] = pComUI;

	pComUI = new CameraUI;
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::CAMERA] = pComUI;

	pComUI = new TileMapUI;
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::TILEMAP] = pComUI;

	pComUI = new Collider2DUI;
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::COLLIDER2D] = pComUI;

	pComUI = new Animator2DUI;
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D] = pComUI;
	
	pComUI = new Light2DUI;
	AddChild(pComUI);
	m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT2D] = pComUI;

	// ==============
	// ResInfoUI ����
	// ==============
	ResInfoUI* pResInfoUI = nullptr;

	// MaterialUI
	pResInfoUI = new MaterialUI;
	AddChild(pResInfoUI);
	m_arrResUI[(UINT)RES_TYPE::MATERIAL] = pResInfoUI;

	// TextureUI
	pResInfoUI = new TextureUI;
	AddChild(pResInfoUI);
	m_arrResUI[(UINT)RES_TYPE::TEXTURE] = pResInfoUI;

	pResInfoUI = new PrefabUI;
	AddChild(pResInfoUI);
	m_arrResUI[(UINT)RES_TYPE::PREFAB] = pResInfoUI;

	m_pCreate2DAnim = (Create2DAnimationUI*)CImGuiMgr::GetInst()->FindUI("Create 2D Animation UI");

}

InspectorUI::~InspectorUI()
{
}


void InspectorUI::update()
{
	for (int i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if(nullptr != m_arrComUI[i])
			m_arrComUI[i]->update();
	}

	for (int i = 0; i < (UINT)RES_TYPE::END; i++)
	{
		if (nullptr != m_arrResUI[i])
			m_arrResUI[i]->update();
	}

	//if (KEY_TAP(KEY::I))
	//{
	//	if (IsActive())
	//		Deactivate();
	//	else
	//		Activate();
	//}
}

void InspectorUI::render_update()
{
	if (nullptr != m_pTargetObject)
	{
		if (ImGui::BeginTable("##ObjInfoTable", 2, ImGuiTableFlags_SizingStretchProp))
		{
			// OBJ �̸� ǥ��
			ImGui::TableNextColumn();
			ImGui::Text("Name");
			ImGui::SameLine();

			char buffer[256] = {};
			string strName = ToString(m_pTargetObject->GetName());
			strcpy_s(buffer, strName.c_str());

			SceneOutliner* pList = (SceneOutliner*)CImGuiMgr::GetInst()->FindUI("SceneOutliner");

			if (ImGui::InputText("##ObjName", buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiTableFlags_NoPadInnerX ))
			{
				// �Է¹��� �̸����� ����
				string strInput(buffer);
				m_pTargetObject->SetName(ToWString(strInput));

				// SceneOutliner�� ����� �̸� �ݿ�
				CGameObject* tmp = m_pTargetObject;
				pList->Reset();
				m_pTargetObject = tmp;
			}

			// OBJ Layer ǥ��
			ImGui::Text("Layer");
			ImGui::SameLine();

			int curIdx = m_pTargetObject->GetLayerIndex();
			CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
			string strLayerName = std::to_string(curIdx) + " : " + ToString(pCurScene->GetLayer(curIdx)->GetName());

			if (ImGui::BeginCombo("##Layers", strLayerName.c_str()))
			{
				for (int i = 0; i < MAX_LAYER; i++)
				{
					strLayerName = std::to_string(i) + " : " + ToString(pCurScene->GetLayer(i)->GetName());

					if (ImGui::Selectable(strLayerName.c_str()))
					{
						m_pTargetObject->ChangeLayer(i);
					}
				}

				ImGui::EndCombo();
			}

			// ������ ���� ��ư
			ImGui::TableNextColumn();
			if (ImGui::Button("Prefab", ImVec2(50, 50)))
			{
				pList->MakePrefab(m_pTargetObject);
			}

			ImGui::EndTable();
		}

		ImGui::Separator();
	}
}



void InspectorUI::SetTargetObject(CGameObject* _pTarget)
{
	m_pTargetObject = _pTarget;
	m_pTargetRes = nullptr;

	for (int i = 0; i < (int)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrComUI[i])
		{
			// Object �� nullptr �� ���
			if (nullptr == m_pTargetObject)
			{
				m_arrComUI[i]->SetTargetObject(nullptr);
				m_arrComUI[i]->Deactivate();
			}
			else if (m_pTargetObject->GetComponent((COMPONENT_TYPE)i))
			{
				m_arrComUI[i]->SetTargetObject(m_pTargetObject);

				if (COMPONENT_TYPE::ANIMATOR2D == (COMPONENT_TYPE)i)
					m_pCreate2DAnim->SetTargetObj(m_pTargetObject);

				m_arrComUI[i]->Activate();
			}			
			else
			{
				m_arrComUI[i]->SetTargetObject(nullptr);
				m_arrComUI[i]->Deactivate();
			}
		}		
	}

	// ������Ʈ�� null �̸�
	if (nullptr == m_pTargetObject)
	{
		// ��� ��ũ��ƮUI ��Ȱ��ȭ
		for (int i = 0; i < m_vecScriptUI.size(); ++i)
		{
			m_vecScriptUI[i]->Deactivate();
		}
	}
	else
	{
		const vector<CScript*>& vecScripts = m_pTargetObject->GetScripts();
		ScriptUI* pScriptUI = nullptr;

		for (size_t i = 0; i < vecScripts.size(); ++i)
		{
			if (m_vecScriptUI.size() <= i)
				pScriptUI = AddScriptUI();
			else
				pScriptUI = m_vecScriptUI[i];

			pScriptUI->SetTargetObject(m_pTargetObject);
			pScriptUI->SetTargetScript(vecScripts[i]);
			pScriptUI->Activate();
		}

		// ScriptUI �� �� ���� ������
		if (vecScripts.size() < m_vecScriptUI.size())
		{
			// �����ϴ� UI �� ������ ������ ScriptUI ���� ��Ȱ��ȭ �Ѵ�.��
			for (size_t i = vecScripts.size(); i < m_vecScriptUI.size(); ++i)
			{
				m_vecScriptUI[i]->Deactivate();
			}
		}
	}

	
	// ResInfoUI ��Ȱ��ȭ
	for (int i = 0; i < (int)RES_TYPE::END; ++i)
	{
		if (nullptr != m_arrResUI[i] && m_arrResUI[i]->IsActive())
		{
			m_arrResUI[i]->SetTargetRes(nullptr);
			m_arrResUI[i]->Deactivate();
		}
	}
}

void InspectorUI::SetTargetResource(CRes* _pTargetRes)
{
	m_pTargetRes = _pTargetRes;
	m_pTargetObject = nullptr;

	// ComponentUI ���� ��Ȱ��ȭ
	for (int i = 0; i < (int)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrComUI[i] && m_arrComUI[i]->IsActive())
		{
			m_arrComUI[i]->SetTargetObject(nullptr);
			m_arrComUI[i]->Deactivate();
		}
	}

	// ScriptUI ���� ��Ȱ��ȭ
	for (size_t i = 0; i < m_vecScriptUI.size(); ++i)
	{
		m_vecScriptUI[i]->Deactivate();
	}

	// Ȱ��ȭ ��ų RES_TYPE �� �˾Ƴ�
	RES_TYPE type = _pTargetRes->GetResType();

	// �ش� ���ҽ� ResInfoUI Ȱ��ȭ
	for (int i = 0; i < (int)RES_TYPE::END; ++i)
	{
		if (nullptr != m_arrResUI[(int)i])
		{
			if (i == (int)type)
			{
				m_arrResUI[i]->Activate();
				m_arrResUI[i]->SetTargetRes(_pTargetRes);
			}			
			else
				m_arrResUI[i]->Deactivate();
		}
	}
	
}

ScriptUI* InspectorUI::AddScriptUI()
{
	ScriptUI* pScriptUI = new ScriptUI;
	pScriptUI->Deactivate();
	AddChild(pScriptUI);

	m_vecScriptUI.push_back(pScriptUI);

	return pScriptUI;
}

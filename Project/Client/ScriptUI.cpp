#include "pch.h"
#include "ScriptUI.h"

#include <Engine/CScript.h>
#include <Script/CScriptMgr.h>

#include "ParamUI.h"

ScriptUI::ScriptUI()
	: UI("Script")
	, m_pTargetObject(nullptr)
	, m_pTargetScript(nullptr)
	, m_eComType(COMPONENT_TYPE::SCRIPT)
	, m_bActive(false)
{	
}

ScriptUI::~ScriptUI()
{
}

void ScriptUI::update()
{

}

void ScriptUI::render_update()
{
	// ???? Script ?̸?
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.8f));

	if (nullptr != CScriptMgr::GetScriptName(m_pTargetScript))
	{
		wstring wstrScriptName = CScriptMgr::GetScriptName(m_pTargetScript);
		string strScriptName(wstrScriptName.begin(), wstrScriptName.end());
		ImGui::Button(strScriptName.c_str());
	}

	ImGui::PopStyleColor(3);
	ImGui::PopID();

	const vector<tScriptParamInfo>& vecParam = m_pTargetScript->GetScriptParam();

	for (size_t i = 0; i < vecParam.size(); ++i)
	{
		 switch (vecParam[i].eType)
		{
		case SCRIPTPARAM_TYPE::INT:
		{
			int data = ParamUI::Param_Int(vecParam[i].strParamName, (int*)vecParam[i].pParam);
			m_pTargetScript->SetParam<int>(vecParam[i].strParamName, data);
		}
			break;
		case SCRIPTPARAM_TYPE::FLOAT:
		{
			float data = ParamUI::Param_Float(vecParam[i].strParamName, (float*)vecParam[i].pParam);
			m_pTargetScript->SetParam<float>(vecParam[i].strParamName, data);
		}
			break;
		case SCRIPTPARAM_TYPE::VEC2:
		{
			Vec2 data = ParamUI::Param_Vec2(vecParam[i].strParamName, (Vec2*)vecParam[i].pParam);
			m_pTargetScript->SetParam<Vec2>(vecParam[i].strParamName, data);
		}
			break;
		case SCRIPTPARAM_TYPE::VEC4:
		{
			Vec4 data = ParamUI::Param_Vec4(vecParam[i].strParamName, (Vec4*)vecParam[i].pParam);
			m_pTargetScript->SetParam<Vec4>(vecParam[i].strParamName, data);
		}
			break;
		case SCRIPTPARAM_TYPE::ENUM:
		{
			UINT data = ParamUI::Param_ENUM(vecParam[i].strParamName, (UINT*)vecParam[i].pParam, m_pTargetScript->GetEnumItemNames());
			if(data != *(UINT*)vecParam[i].pParam)
				m_pTargetScript->SetParam<UINT>(vecParam[i].strParamName, data);
		}
			break;

		case SCRIPTPARAM_TYPE::TEX:
			break;
		case SCRIPTPARAM_TYPE::PREFAB:
			break;
		}

	}
}

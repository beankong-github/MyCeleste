#include "pch.h"
#include "PrefabUI.h"

#include "CImGuiMgr.h"

#include <Engine/CResMgr.h>
#include <Engine/CEventMgr.h>
#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>

PrefabUI::PrefabUI()
	: ResInfoUI("Prefab", RES_TYPE::PREFAB)

{
}

PrefabUI::~PrefabUI()
{
}

void PrefabUI::update()
{
	ResInfoUI::update();
}

void PrefabUI::render_update()
{
	ResInfoUI::render_update();

	ShowOBJInfo();

}

void PrefabUI::ShowOBJInfo()
{
	CPrefab* pTargetPref = (CPrefab*)GetTargetRes();
	CGameObject* pProto = pTargetPref->GetProto();

	if (ImGui::BeginTable("##ObjInfoTable", 2, ImGuiTableFlags_SizingStretchProp))
	{
		// OBJ 이름
		ImGui::TableNextColumn();
		ImGui::Text("Name");
		ImGui::SameLine();

		char buffer[256] = {};
		string strName = ToString(pProto->GetName());
		strcpy_s(buffer, strName.c_str());

		ImGui::InputText("##ObjName", buffer, 256, ImGuiInputTextFlags_ReadOnly);

		// OBJ Layer 표시
		ImGui::Text("Layer");
		ImGui::SameLine();

		int curIdx = pProto->GetLayerIndex();


		char buffer2[256] = {};
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
		string strLayer = std::to_string(pProto->GetLayerIndex()) + " : " + ToString(pCurScene->GetLayer(curIdx)->GetName());
		strcpy_s(buffer2, strLayer.c_str());

		ImGui::InputText("##LayerName", buffer2, 256, ImGuiInputTextFlags_ReadOnly);

		// 프리팹 Instatiate 버튼
		ImGui::TableNextColumn();
		if (ImGui::Button("Inst", ImVec2(50, 50)))
		{
			tEventInfo info = {};

			info.eType = EVENT_TYPE::CREATE_OBJ;
			info.lParam = (DWORD_PTR)pTargetPref->Instantiate();
			info.wParam = (DWORD_PTR)1;

			CEventMgr::GetInst()->AddEvent(info);
		}
		ImGui::EndTable();
	}
	ImGui::Separator();

}
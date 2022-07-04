#include "pch.h"
#include "TileMapUI.h"

#include <Engine\CTileMap.h>

#include "CImGuiMgr.h"
#include "LandScapeEditor.h"

TileMapUI::TileMapUI()
	: ComponentUI("TileMap", COMPONENT_TYPE::TILEMAP)
{
	SetSize(Vec2(0.f, 100.f));
}

TileMapUI::~TileMapUI()
{
}


void TileMapUI::update()
{
	ComponentUI::update();
}

void TileMapUI::render_update()
{
	ComponentUI::render_update();


	CGameObject* pTargetObject = GetTargetObject();
	CTileMap* pTileMap = pTargetObject->TileMap();
	CTexture* pTexture = pTileMap->GetAtlasTex();
	string strAtlasName = string(pTexture->GetKey().begin(), pTexture->GetKey().end());

	ImGui::Text("TileMap Atals Key");
	ImGui::SameLine(140);
	ImGui::SetNextItemWidth(180);
	ImGui::InputText("##TileMapAtalsName_Inspector", (char*)strAtlasName.c_str(), strAtlasName.capacity(), ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Tile Count");
	ImGui::SameLine(140);
	float fSize[2] = { pTileMap->GetTileMapSize().x, pTileMap->GetTileMapSize().y };
	ImGui::SetNextItemWidth(180);
	ImGui::InputFloat2("##TileMapSize_Inspector", fSize, "%.3f", ImGuiInputTextFlags_ReadOnly);

	ImGui::Spacing();
	ImGui::Spacing();

	// LandScapeEditor¸¦ ¶ç¿öÁÖ´Â Button
	if (ImGui::Button("Edit TileMap", ImVec2(320, 0)))
	{
		LandScapeEditor* editor = (LandScapeEditor*)CImGuiMgr::GetInst()->FindUI("LandScapeEditor");
		editor->SetType(LS_EDIT_TYPE::TILE_MAP);
		editor->SetTargetObject(GetTargetObject());
		editor->Activate();
	}
}

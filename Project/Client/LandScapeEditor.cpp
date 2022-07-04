#include "pch.h"
#include "LandScapeEditor.h"

#include <Engine\CTileMap.h>
#include <Engine\CResMgr.h>
#include <Engine\CCore.h>
#include <Engine\CCamera.h>
#include <Engine\CSceneMgr.h>
#include <Engine\CTransform.h>

#include "CImGuiMgr.h"
#include "TileMapEditor.h"
#include <ListUI.h>
#include <cmath>

LandScapeEditor::LandScapeEditor()
	: UI("LandScapeEditor")
	, m_eType(LS_EDIT_TYPE::NONE)
	, m_pTargetGameObject(nullptr)
	, m_pTileMapEditor(nullptr)
	, m_iCurTileIdx(-1)
{
	SetSize(Vec2{ 300, 0});

}

LandScapeEditor::~LandScapeEditor()
{
}


void LandScapeEditor::SetTargetObject(CGameObject* _pObj)
{
	// TargetGameObject must has TileMap component 
	assert(_pObj->TileMap());
	m_pTargetGameObject = _pObj;
}

void LandScapeEditor::update()
{
	if (LS_EDIT_TYPE::TILE_MAP == m_eType)
	{
		if (IsActive())
		{
			if (nullptr == m_pTileMapEditor)
			{
				m_pTileMapEditor = (TileMapEditor*)CImGuiMgr::GetInst()->FindUI("TileMapEditor");

				m_pTileMapEditor->SetTargetObject(m_pTargetGameObject);
				m_pTileMapEditor->Activate();
			}
		}
		else
		{
			if (nullptr != m_pTileMapEditor)
			{ 
				m_pTileMapEditor->Clear();
				m_pTileMapEditor->Deactivate();
			}
		}
	}

}

void LandScapeEditor::render_update()
{
	// EditMode Combo
	ImGui::Text("Edit Mode");
	ImGui::SameLine(120); ImGui::SetNextItemWidth(180);
	if (ImGui::BeginCombo("##LandScapeEditMode", strLSEditType[(int)m_eType]))
	{
		for (int i = 0; i < (int)LS_EDIT_TYPE::END; i++)
		{
			if (ImGui::Selectable(strLSEditType[i]))
			{
				m_eType = (LS_EDIT_TYPE)i;
			}
		}
		ImGui::EndCombo();
	}

	// TargetObject
	ImGui::Text("Target Object");
	ImGui::SameLine(120);
	string strName;
	if (nullptr != m_pTargetGameObject)
		strName = string(m_pTargetGameObject->GetName().begin(), m_pTargetGameObject->GetName().end());

	ImGui::SetNextItemWidth(180);
	ImGui::InputText("##TargetObject_LandScapeEditor", (char*)strName.c_str(), strName.capacity(), ImGuiInputTextFlags_ReadOnly);

	switch (m_eType)
	{
	case LS_EDIT_TYPE::TILE_MAP:
		TileMapMode();
		break;
	
	case LS_EDIT_TYPE::COLLIDER:
		ColliderMode();
		break;
	
	case LS_EDIT_TYPE::PREFAB:
		PrefabMode();
		break;
	}
}

void LandScapeEditor::TileMapMode()
{
	
	CTileMap* pTilemap = m_pTargetGameObject->TileMap();
	CTexture* pAtlasTexture = pTilemap->GetAtlasTex();

	assert(pTilemap);

	ImGui::Text("TileMap Size");
	ImGui::SameLine(150);
	ImGui::SetNextItemWidth(100);
	int TileMapSize[2] = { pTilemap->GetTileMapSize().x, pTilemap->GetTileMapSize().y };
	ImGui::InputInt2("##TileMapSize", TileMapSize, ImGuiInputTextFlags_ReadOnly);

	// ==============================
	//  Atlas Name + Texture List UI
	// ==============================
	ImGui::Text("Atlas");
	ImGui::SameLine(150);
	ImGui::SetNextItemWidth(100);
	string strName(pTilemap->GetAtlasTex()->GetKey().begin(), pTilemap->GetAtlasTex()->GetKey().end());
	ImGui::InputText("##TileAtlasTexture_LandScapeEditor", (char*)strName.c_str(), strName.capacity(), ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	if (ImGui::Button("##FindTileAtlas", ImVec2(15, 15 )))
	{
		// Get Textures
		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList(RES_TYPE::TEXTURE);
		
		// Init ListUI
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->Clear();
		pListUI->SetTitle("Texture List");

		// Add Items
		for (const auto& pair : mapRes)
		{ 
			pListUI->AddList(string(pair.first.begin(), pair.first.end()));
		}

		// Activate List UI
		pListUI->Activate();
		// Add Double Click Event
		pListUI->SetDBCEvent(this, (DBCLKED)&LandScapeEditor::TextureSelect);
	}
	// ============
	//  Inform UI 
	// ============

	ImGui::Text("Atlas Tile Count");
	ImGui::SameLine(150);
	ImGui::SetNextItemWidth(100);
	int TileCount[2] = { (int)pTilemap->GetAtlasTileCount().x, (int)pTilemap->GetAtlasTileCount().y };
	ImGui::InputInt2("##TileCountinAtlas", TileCount, ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("TileSize");
	ImGui::SameLine(150);
	ImGui::SetNextItemWidth(100);
	int TileSize = (int)pTilemap->GetTileSize().x;
	ImGui::InputInt("##TileSiez_LandscapeEditor", &TileSize, 0, 0);
	pTilemap->SetTileSize(Vec2{ TileSize , TileSize });

	// ====================
	//  Show Selected Tile
	// ====================	
	// selected tile's index number
	ImGui::Text("Selected Tile");
	ImGui::SameLine(150);
	ImGui::SetNextItemWidth(100);
	ImGui::InputInt("##CurrentlySelectedTileIndex", &m_iCurTileIdx, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AlwaysInsertMode);
	// show selected tile
	if (m_iCurTileIdx != -1)
	{
		int iCurTileCol = m_iCurTileIdx % (int)(pTilemap->GetAtlasTileCount().x);
		int iCurTileRow = m_iCurTileIdx / (int)(pTilemap->GetAtlasTileCount().x);
		ImVec2 vTileStartUV = ImVec2
		{	
			iCurTileCol * pTilemap->GetTileSize().x / pTilemap->GetAtlasTex()->Width(),
			iCurTileRow * pTilemap->GetTileSize().y / pTilemap->GetAtlasTex()->Height() 
		};
		ImVec2 vTileEndUV = ImVec2
		{
			vTileStartUV.x + pTilemap->GetTileSize().x / pTilemap->GetAtlasTex()->Width(),
			vTileStartUV.y + pTilemap->GetTileSize().y / pTilemap->GetAtlasTex()->Height()
		};
		ImGui::Image(pAtlasTexture->GetSRV().Get(), ImVec2{ 80, 80 }, vTileStartUV, vTileEndUV);
	}

	// ==============
	//  Tile Palette
	// ==============
	static ImVec2 offset = { 0.f, 0.f };
	static float zoom = 1;

	// Get the ImGui cursor in screen space
	ImVec2 display_p0 = ImGui::GetCursorScreenPos();
	// Get size of the tileset display
	ImVec2 display_sz = ImGui::GetContentRegionAvail();
	display_sz.x = (display_sz.x < 50.0f ? 50.0f : display_sz.x);
	display_sz.y = (display_sz.y < 50.0f ? 50.0f : (display_sz.y > 320.0f ? 320.0f : display_sz.y));
	ImVec2 display_p1 = ImVec2(display_p0.x + display_sz.x, display_p0.y + display_sz.y);
	ImVec2 border_p0 = ImVec2(display_p0.x - 1.f, display_p0.y - 1.f);
	ImVec2 border_p1 = ImVec2(display_p1.x + 1.f, display_p1.y + 1.f);

	// Draw border and background color
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRectFilled(display_p0, display_p1, IM_COL32(50, 50, 50, 255));
	draw_list->AddRect(border_p0, border_p1, IM_COL32(255, 255, 255, 255));

	// Create an invisible button which will catch inputs
	auto display_rect_flags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight |
		ImGuiButtonFlags_MouseButtonMiddle;
	ImGui::InvisibleButton("display", display_sz, display_rect_flags);
	const bool is_hovered = ImGui::IsItemHovered();
	const bool is_active = ImGui::IsItemActive();

	// Calculate origin, then use it to calculate where the mouse cursor is
	ImGuiIO& io = ImGui::GetIO();
	const ImVec2 origin(display_p0.x + offset.x, display_p0.y + offset.y);
	const ImVec2 mouse_pos_in_display(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

	// Pan the tileset display
	if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)) {
		offset.x += io.MouseDelta.x;
		offset.y += io.MouseDelta.y;
	}
	// Zoom into tileset display
	if (is_hovered) {
		// Minimum zoom = x0.1 (near camera limit)
		// Maximum zoom = x5.0 (it can fit an image with 1600px width)
		zoom = zoom - (io.MouseWheel * 0.3f);
		if(zoom < 0.1f)
			zoom = 0.1f;
		else if(zoom > 5.0f)
			zoom = 5.0f;
	}

	// Clip what we're drawing next
	draw_list->PushClipRect(display_p0, display_p1, true);

	// Draw the tileset
	ImVec2 tileset_p0 = ImVec2(origin.x, origin.y);
	ImVec2 tileset_p1 = ImVec2(origin.x + (pAtlasTexture->Width() / zoom), origin.y + (pAtlasTexture->Height() / zoom));
	draw_list->AddImage(pAtlasTexture->GetSRV().Get(), tileset_p0, tileset_p1, ImVec2(0, 0), ImVec2(1, 1));

	// Get the currently hovered + active tile
	// And set the current tile on click
	ImVec2 zoomed_tileSize = ImVec2{ pTilemap->GetTileSize().x / zoom, pTilemap->GetTileSize().y / zoom };
	if (is_hovered) {
		// calculate mouse pos on atlas
		float mx = (mouse_pos_in_display.x);
		float my = (mouse_pos_in_display.y);

		// check if mouse is intersecting atlas
		float atlasW = pAtlasTexture->Width() / zoom;
		float atlasH = pAtlasTexture->Height() / zoom;
		if (mx > 0 && mx < atlasW && my > 0 && my < atlasH) {
			int tile_col = (int)(mx / zoomed_tileSize.x);
			int tile_row = (int)(my / zoomed_tileSize.y);

			// draw tile highlight
			ImVec2 hover_p0 = ImVec2(tileset_p0.x + (zoomed_tileSize.x * tile_col), tileset_p0.y + (zoomed_tileSize.y * (tile_row)));
			ImVec2 hover_p1 = ImVec2(hover_p0.x + zoomed_tileSize.x, hover_p0.y + zoomed_tileSize.y);
			draw_list->AddRectFilled(hover_p0, hover_p1, IM_COL32(120, 120, 120, 120));


			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				// calculate tile ID
				float tilesPerRow = pTilemap->GetAtlasTileCount().x;
				float tileSetId = pTilemap->GetAtlasTileCount().x * pTilemap->GetAtlasTileCount().y;

				// X and Y are reversed because of UV flip when drawing tileset atlas (see above
				// @AddImage)
				float tile_id = tile_col + (tile_row * tilesPerRow);

				// set current tile
				m_iCurTileIdx = tile_id;
			}
		}
		
	}
}

void LandScapeEditor::ColliderMode()
{
}

void LandScapeEditor::PrefabMode()
{
}

void LandScapeEditor::TextureSelect(DWORD_PTR _texture)
{
	string strSelectedName = (char*)_texture;
	wstring strTexturelKey = wstring(strSelectedName.begin(), strSelectedName.end());

	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(strTexturelKey);
	assert(pTex.Get());
	m_pTargetGameObject->TileMap()->SetAtlasTex(pTex);
}

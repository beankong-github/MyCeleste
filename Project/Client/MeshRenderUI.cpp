#include "pch.h"
#include "MeshRenderUI.h"

#include "CImGuiMgr.h"
#include "ListUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CMeshRender.h>
#include <Engine/CMesh.h>
#include <Engine/CMaterial.h>

MeshRenderUI::MeshRenderUI()
	: ComponentUI("MeshRender", COMPONENT_TYPE::MESHRENDER)
{
	SetSize(Vec2(0.f, 150.f));
}

MeshRenderUI::~MeshRenderUI()
{

}

void MeshRenderUI::update()
{
	ComponentUI::update();

}

void MeshRenderUI::render_update()
{
	ComponentUI::render_update();

	ImGui::PushItemWidth(200);

	CMeshRender* pMeshRender = GetTargetObject()->MeshRender();
	Ptr<CMesh> pMesh = pMeshRender->GetMesh();
	Ptr<CMaterial> pMtrl = pMeshRender->GetMaterial();

	string strMeshName, strMtrlName;
	if(nullptr != pMesh)
		strMeshName = string(pMesh->GetKey().begin(), pMesh->GetKey().end());
	if(nullptr != pMtrl)
		strMtrlName = string(pMtrl->GetKey().begin(), pMtrl->GetKey().end());
	
	ImGui::Text("Mesh");	
	ImGui::SameLine(86.f); 

	ImGui::InputText("##MeshName", (char*)strMeshName.c_str(), strMeshName.capacity(), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget())
	{
		DWORD_PTR dwData = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ResourceUI"))
		{
			memcpy(&dwData, payload->Data, sizeof(DWORD_PTR));
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("##MeshListBtn", Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList(RES_TYPE::MESH);
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->Clear();
		pListUI->SetTitle("Mesh List");

		for (const auto& pair : mapRes)
		{			
			pListUI->AddList(string(pair.first.begin(), pair.first.end()));
		}
		
		pListUI->Activate();
		pListUI->SetDBCEvent(this, (DBCLKED)&MeshRenderUI::MeshSelect);
	}


	ImGui::Text("Material"); 
	ImGui::SameLine(86.f); 
	ImGui::InputText("##MaterialName", (char*)strMtrlName.c_str(), strMtrlName.capacity(), ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine(); 
	if (ImGui::Button("##MaterialListBtn", Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList(RES_TYPE::MATERIAL);
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->Clear();
		pListUI->SetTitle("Material List");

		for (const auto& pair : mapRes)
		{
			pListUI->AddList(string(pair.first.begin(), pair.first.end()));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(this, (DBCLKED)&MeshRenderUI::MtrlSelect);
	}

	ImGui::Text("Atlas Texture Key");
	ImGui::SameLine(86.f);
	ImGui::InputText("##TexKey", (char*)ToString(pMeshRender->GetAtlasKey()).c_str(), 256, ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	if (ImGui::Button("##TexKeyListBtn", Vec2(15, 15)))
	{
		// Texture Map을 가져온다
		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList(RES_TYPE::TEXTURE);
		// ListUI 활성화한다.
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->Clear();
		pListUI->SetTitle("Texture List");
		// List Item 추가
		for (const auto& pair : mapRes)
		{
			pListUI->AddList(string(pair.first.begin(), pair.first.end()));
		}
		// List 활성화
		pListUI->Activate();
		// 더블클릭 이벤트 등록
		pListUI->SetDBCEvent(this, (DBCLKED)&MeshRenderUI::TextureSelect);
	}
	
	ImGui::Text("Sprite Key");
	ImGui::SameLine(86.f);
	ImGui::InputText("##SpriteKey", (char*)ToString(pMeshRender->GetSpriteKey()).c_str(), 256, ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	if (ImGui::Button("##SpriteKeyListBtn", Vec2(15, 15)))
	{
		// XML Map을 가져온다
		const map<wstring, CRes*> mapRes = CResMgr::GetInst()->GetResList(RES_TYPE::XMLData);
		
		// 현재 Atlas에 해당하는 XML 파일이 있는지 확인한다. 
		wstring spriteKey = pMeshRender->GetAtlasKey();
		size_t pos = spriteKey.find_last_of(L".");
		assert(pos != spriteKey.npos);
		spriteKey = spriteKey.substr(0, pos);
		spriteKey += L".xml";

		// XML이 있다면 해당 XML이 가진 Sprite ID들을 띄워준다
		map<wstring, CRes*>::const_iterator iter = mapRes.find(spriteKey);
		if (iter != mapRes.end())
		{
			// ListUI 활성화한다.
			ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
			pListUI->Clear();
			pListUI->SetTitle("Sprite List");

			CXMLData* data = (CXMLData*)iter->second;
			const map<wstring, tSprite> sprites = data->GetSpriteDatas();

			for (const auto& pair : sprites)
			{
				pListUI->AddList(string(pair.first.begin(), pair.first.end()));
			}
			// List 활성화
			pListUI->Activate();
			// 더블클릭 이벤트 등록
			pListUI->SetDBCEvent(this, (DBCLKED)&MeshRenderUI::SpriteKeySelect);
		}

	}
}

void MeshRenderUI::MeshSelect(DWORD_PTR _param)
{
	string strSelectedName = (char*)_param;
	wstring strMeshKey = wstring(strSelectedName.begin(), strSelectedName.end());

	Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(strMeshKey);
	assert(pMesh.Get());

	CMeshRender* pMeshRender = GetTargetObject()->MeshRender();
	pMeshRender->SetMesh(pMesh);
}

void MeshRenderUI::MtrlSelect(DWORD_PTR _param)
{
	string strSelectedName = (char*)_param;
	wstring strMtrlKey = wstring(strSelectedName.begin(), strSelectedName.end());

	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(strMtrlKey);
	assert(pMtrl.Get());

	CMeshRender* pMeshRender = GetTargetObject()->MeshRender();
	pMeshRender->SetSharedMaterial(pMtrl);
}

void MeshRenderUI::TextureSelect(DWORD_PTR _param)
{
	string strSelectedName = (char*)_param;
	wstring strTexKey = wstring(strSelectedName.begin(), strSelectedName.end());

	CMeshRender* pMeshRender = GetTargetObject()->MeshRender();
	pMeshRender->SetAtlasKey(strTexKey);
}

void MeshRenderUI::SpriteKeySelect(DWORD_PTR _param)
{
	string strSelectedName = (char*)_param;
	wstring strSpriteKey = wstring(strSelectedName.begin(), strSelectedName.end());

	CMeshRender* pMeshRender = GetTargetObject()->MeshRender();
	pMeshRender->SetSpritekey(strSpriteKey);
}



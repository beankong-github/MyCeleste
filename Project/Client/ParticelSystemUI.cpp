#include "pch.h"
#include "ParticelSystemUI.h"

#include "CImGuiMgr.h"
#include "ListUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CParticleSystem.h>
#include <Engine/CParticleUpdateShader.h>

ParticelSystemUI::ParticelSystemUI()
	: ComponentUI("MeshRender", COMPONENT_TYPE::PARTICLESYSTEM)
	, m_iSelectedIdx(-1)
{
	SetSize(Vec2(0.f, 350.f));
}

ParticelSystemUI::~ParticelSystemUI()
{
}

void ParticelSystemUI::update()
{
	ComponentUI::update();
}

void ParticelSystemUI::render_update()
{
	ComponentUI::render_update();
	
	CParticleSystem* pParticelSystem = GetTargetObject()->ParticleSystem();
	const Ptr<CParticleUpdateShader> pCS = GetTargetObject()->ParticleSystem()->GetParticelUpdateShader();

	
	// 파티클 업데이트 쉐이더
	string strCS;
	if (nullptr != pCS)
		strCS = ToString(pCS.Get()->GetKey());

	ImGui::Text("Particle Update Shader");
	ImGui::InputText("##ParticleUpdateShader", (char*)strCS.c_str(), strCS.capacity(), ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	if (ImGui::Button("##ParticleUpdateShaderBtn", Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList(RES_TYPE::COMPUTE_SHADER);
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->Clear();
		pListUI->SetTitle("Mesh List");

		for (const auto& pair : mapRes)
		{
			pListUI->AddList(string(pair.first.begin(), pair.first.end()));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(this, (DBCLKED)&ParticelSystemUI::CSSelect);
	}

	// 최대 파티클 수
	int iMaxParticle = pParticelSystem->GetMaxParticleCount();

	ImGui::Text("Max Particle Count");
	ImGui::SameLine(110);
	if (ImGui::InputInt("##MaxParticle", &iMaxParticle))
	{
		pParticelSystem->SetMaxParticleCount(iMaxParticle);
	}

	// 한 프레임 파티클 수 
	int iAliveCount = pParticelSystem->GetAliveParticleCountPerFrame();

	ImGui::Text(u8"프레임 당 파티클 수");
	ImGui::SameLine(110);
	if (ImGui::InputInt("##AliveParticleCount", &iAliveCount))
	{
		pParticelSystem->SetAliveParticleCountPerFrame(iAliveCount);
	}

	// 오브젝트 좌표에 영향
	bool bFollow = (bool)pParticelSystem->GetPosInherit();
	ImGui::Text("Follow Object Pos");
	ImGui::SameLine(110);
	if (ImGui::Checkbox("##Follow", &bFollow))
	{
		pParticelSystem->SetPosInherit(bFollow);
	}

	// 파티클 최소 지속 시간
	float iMinLife = pParticelSystem->GetMinLifeTime();
	ImGui::Text("Min Life Time");
	ImGui::SameLine(110);
	if (ImGui::InputFloat("##MinLife", &iMinLife))
	{
		pParticelSystem->SetMinLifeTime(iMinLife);
	}

	// 파티클 최대 지속 시간
	float iMaxLife = pParticelSystem->GetMaxLifeTime();
	ImGui::Text("Max Life Time");
	ImGui::SameLine(110);
	if (ImGui::InputFloat("##MaxLife", &iMaxLife))
	{
		pParticelSystem->SetMaxLifeTime(iMaxLife);
	}

	// 파티클 최소 속도
	float iMinSpeed = pParticelSystem->GetStartSpeed();
	ImGui::Text("Min Speed");
	ImGui::SameLine(110);
	if (ImGui::InputFloat("##MinSpeed", &iMinSpeed))
	{
		pParticelSystem->SetStartSpeed(iMinSpeed);
	}

	// 파티클 최대 속도
	float iMaxSpeed = pParticelSystem->GetEndSpeed();
	ImGui::Text("Max Speed");
	ImGui::SameLine(110);
	if (ImGui::InputFloat("##MaxSpeed", &iMaxSpeed))
	{
		pParticelSystem->SetEndSpeed(iMaxSpeed);
	}

	// 파티클 생성 범위
	float fParticleCreateDistance = pParticelSystem->GetParticleCreateDiastance();
	ImGui::Text(u8"파티클 생성 범위");
	ImGui::SameLine(110);
	if (ImGui::InputFloat("##ParticleCreateDistance", &fParticleCreateDistance))
	{
		pParticelSystem->SetParticleCreateDiastance(fParticleCreateDistance);
	}

	// 파티클 생성 간격
	float fParticleCreateTerm = pParticelSystem->GetParticleCreateTerm();
	ImGui::Text(u8"파티클 생성 간격");
	ImGui::SameLine(110);
	if (ImGui::InputFloat("##ParticleCreateTerm", &fParticleCreateTerm))
	{
		pParticelSystem->SetParticleCreateTerm(fParticleCreateTerm);
	}

	// 파티클 시작 크기
	float vStartScale[3] = { pParticelSystem->GetStartScale().x, pParticelSystem->GetStartScale().y, pParticelSystem->GetStartScale().z };
	ImGui::Text("Start Scale");
	ImGui::SameLine(110);
	if (ImGui::InputFloat3("##StartScale", vStartScale))
	{
		Vec3 scale(vStartScale[0], vStartScale[1], vStartScale[2]);
		pParticelSystem->SetStartScale(scale);
	}

	// 파티클 최종 크기
	float vEndScale[3] = { pParticelSystem->GetEndScale().x, pParticelSystem->GetEndScale().y, pParticelSystem->GetEndScale().z };
	ImGui::Text("End Scale");
	ImGui::SameLine(110);
	if (ImGui::InputFloat3("##EndScale", vEndScale))
	{
		Vec3 scale(vEndScale[0], vEndScale[1], vEndScale[2]);
		pParticelSystem->SetEndScale(scale);
	}

	// 파티클 시작 색상
	float vStartColor[4] = { pParticelSystem->GetStartColor().x, pParticelSystem->GetStartColor().y, pParticelSystem->GetStartColor().z,pParticelSystem->GetStartColor().w };
	ImGui::Text("Start Color");
	ImGui::SameLine(110);
	if (ImGui::InputFloat4("##StartColor", vStartColor))
	{
		Vec4 color(vStartColor[0], vStartColor[1], vStartColor[2], vStartColor[3]);
		pParticelSystem->SetStartColor(color);
	}

	// 파티클 종료 색상
	float vEndColor[4] = { pParticelSystem->GetEndColor().x, pParticelSystem->GetEndColor().y, pParticelSystem->GetEndColor().z,pParticelSystem->GetEndColor().w };
	ImGui::Text("End Color");
	ImGui::SameLine(110);
	if (ImGui::InputFloat4("##EndColor", vEndColor))
	{
		Vec4 color(vEndColor[0], vEndColor[1], vEndColor[2], vEndColor[3]);
		pParticelSystem->SetEndColor(color);
	}


	// 이미지
	const map<wstring, CRes*>& mapTexRes = CResMgr::GetInst()->GetResList(RES_TYPE::TEXTURE);
	for (int i = 0; i < 4; i++)
	{
		Ptr<CTexture> tex = pParticelSystem->GetParticleTex(i);
		string texKey = "";

		if(nullptr != tex)
			texKey = ToString(tex.Get()->GetKey());

		string texNum = "Tex" + std::to_string(i);
		ImGui::Text(texNum.c_str());
		
		string uikey = "##TextureID" + std::to_string(i);
		ImGui::InputText(uikey.c_str(), (char*)texKey.c_str(), strCS.capacity(), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		uikey += "btn";
		if (ImGui::Button(uikey.c_str(), Vec2(15, 15)))
		{
			// ListUI 활성화한다.
			ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
			pListUI->Clear();
			pListUI->SetTitle("Texture List");

			for (const auto& pair : mapTexRes)
			{
				pListUI->AddList(string(pair.first.begin(), pair.first.end()));
			}

			pListUI->Activate();
			m_iSelectedIdx = i;
			pListUI->SetDBCEvent(this, (DBCLKED)&ParticelSystemUI::TexSelect);
		}
	}
}

void ParticelSystemUI::CSSelect(DWORD_PTR _param)
{
	string strSelectedName = (char*)_param;
	wstring strCSKey = wstring(strSelectedName.begin(), strSelectedName.end());

	CComputeShader* pCS = CResMgr::GetInst()->FindRes<CComputeShader>(strCSKey).Get();
	assert(pCS);

	Ptr<CParticleUpdateShader> pParticleUpdateShader = (CParticleUpdateShader*)pCS;
	CParticleSystem* pParticleSys = GetTargetObject()->ParticleSystem();
	pParticleSys->SetParticleUpdateShader(pParticleUpdateShader);
}

void ParticelSystemUI::TexSelect(DWORD_PTR _param)
{
	assert(m_iSelectedIdx != -1);

	string strSelectedName = (char*)_param;
	wstring strTexKey = wstring(strSelectedName.begin(), strSelectedName.end());

	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(strTexKey).Get();
	assert(pTex.Get());

	CParticleSystem* pParticleSys = GetTargetObject()->ParticleSystem();
	
	pParticleSys->SetParticleTexture(pTex, m_iSelectedIdx);

	m_iSelectedIdx = -1;
}


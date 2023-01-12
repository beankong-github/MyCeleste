#include "pch.h"
#include "CPlayerAfterImgScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>
#include <Engine/CScene.h>

#include "CPlayerScript.h"

CPlayerAfterImgScript::CPlayerAfterImgScript()
	: CScript((int)SCRIPT_TYPE::PLAYERAFTERIMGSCRIPT)
	, m_pPlayer(nullptr)
	, m_vAfterImgs{}
	, m_fTerm(0.01f)
	, m_fLifeTime(0.2f)
	, m_iCurIdx(0)
	, m_bPlay(false)
	, m_vColor(68.f, 183.f, 255.f, 256.f)
{
	SetName(L"CPlayerAfterImgScript");

	AddScriptParam("Term", SCRIPTPARAM_TYPE::FLOAT, &m_fTerm);
	AddScriptParam("Life", SCRIPTPARAM_TYPE::FLOAT, &m_fLifeTime);
}

CPlayerAfterImgScript::~CPlayerAfterImgScript()
{
}

void CPlayerAfterImgScript::start()
{
	// 플레이어
	CGameObject* pPlayer = CSceneMgr::GetInst()->FindObjectByName(L"Player");
	assert(pPlayer);
	m_pPlayer = pPlayer->GetScript<CPlayerScript>();
	assert(m_pPlayer);

	// 잔상들 초기화
	m_vAfterImgs = GetOwner()->GetChild();
	assert(!m_vAfterImgs.empty());
	for (int i = 0; i < m_vAfterImgs.size(); ++i)
	{
		m_vAfterImgs[i]->Deactivate();
	}

	// 타이머 초기화
	m_vTimers.resize(m_vAfterImgs.size());
	for (int i = 0; i < m_vTimers.size(); ++i)
	{
		m_vTimers[i] = m_fLifeTime;
	}
}

void CPlayerAfterImgScript::update()
{
	if (m_pPlayer->GetPlayerState() == PLAYER_STATE::DASH && !m_bPlay)
		m_bPlay = true;
	else if (m_pPlayer->GetPlayerState() != PLAYER_STATE::DASH)
	{
		m_iCurIdx = 0;
		m_bPlay = false;
	}

	if (m_bPlay)
	{
		if (m_fTerm < 0.f)
		{
			// 텀 초기화
			m_fTerm = 0.01f;

			// 활성화
			m_vAfterImgs[m_iCurIdx]->Activate();

			// MeshRender 설정
			int curFrame = m_pPlayer->Animator2D()->GetCurAnim()->GetCurFrmIdx();
			wstring AtlasKey = m_pPlayer->Animator2D()->GetCurAnim()->GetAtlasTex()->GetKey();
			wstring AnimName = m_pPlayer->Animator2D()->GetCurAnim()->GetName();
			wstring wstrFrame = curFrame < 10 ? L"0" + to_wstring(curFrame) : to_wstring(curFrame);
			wstring SpriteKey = AnimName + wstrFrame + L".png";
			m_vAfterImgs[m_iCurIdx]->MeshRender()->SetAtlasKey(AtlasKey);
			m_vAfterImgs[m_iCurIdx]->MeshRender()->SetSpritekey(SpriteKey);
		
			// 위치 설정 
			m_vAfterImgs[m_iCurIdx]->Transform()->SetRelativePos(m_pPlayer->Transform()->GetRelativePos());
			m_vAfterImgs[m_iCurIdx]->Transform()->SetRelativeScale(m_pPlayer->Transform()->GetRelativeScale());
			m_vAfterImgs[m_iCurIdx]->Transform()->SetRelativeRotation(m_pPlayer->Transform()->GetRelativeRotation());

				

			++m_iCurIdx;
			if (m_vAfterImgs.size() == m_iCurIdx)
			{
				m_iCurIdx = 0;
				m_bPlay = false;
			}
		}
		else
		{
			m_fTerm -= DT;
		}
	}

}

void CPlayerAfterImgScript::lateupdate()
{
	// 생존 시간에 따라 색상 달리 전달
	for (size_t i = 0; i < m_vAfterImgs.size(); i++)
	{
		// 잔상이 활성화되지 않았다면 생존 시간 계산을 진행하지 않는다.
		if (!m_vAfterImgs[i]->IsActive())
			continue;

		// 타이머 감속
		m_vTimers[i] -= DT;

		// 색상 전달
		m_vAfterImgs[i]->MeshRender()->SetColor(m_vColor / 256.f);
		// 타이머가 종료되면
		if (m_vTimers[i] < 0)
		{
			m_vAfterImgs[i]->Deactivate();
			m_vTimers[i] = m_fLifeTime;
		}
	}
}

void CPlayerAfterImgScript::SaveToScene(FILE* _pFile)
{
}

void CPlayerAfterImgScript::LoadFromScene(FILE* _pFile)
{
}

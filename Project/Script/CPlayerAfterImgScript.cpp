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
	// �÷��̾�
	CGameObject* pPlayer = CSceneMgr::GetInst()->FindObjectByName(L"Player");
	assert(pPlayer);
	m_pPlayer = pPlayer->GetScript<CPlayerScript>();
	assert(m_pPlayer);

	// �ܻ�� �ʱ�ȭ
	m_vAfterImgs = GetOwner()->GetChild();
	assert(!m_vAfterImgs.empty());
	for (int i = 0; i < m_vAfterImgs.size(); ++i)
	{
		m_vAfterImgs[i]->Deactivate();
	}

	// Ÿ�̸� �ʱ�ȭ
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
			// �� �ʱ�ȭ
			m_fTerm = 0.01f;

			// Ȱ��ȭ
			m_vAfterImgs[m_iCurIdx]->Activate();

			// MeshRender ����
			int curFrame = m_pPlayer->Animator2D()->GetCurAnim()->GetCurFrmIdx();
			wstring AtlasKey = m_pPlayer->Animator2D()->GetCurAnim()->GetAtlasTex()->GetKey();
			wstring AnimName = m_pPlayer->Animator2D()->GetCurAnim()->GetName();
			wstring wstrFrame = curFrame < 10 ? L"0" + to_wstring(curFrame) : to_wstring(curFrame);
			wstring SpriteKey = AnimName + wstrFrame + L".png";
			m_vAfterImgs[m_iCurIdx]->MeshRender()->SetAtlasKey(AtlasKey);
			m_vAfterImgs[m_iCurIdx]->MeshRender()->SetSpritekey(SpriteKey);
		
			// ��ġ ���� 
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
	// ���� �ð��� ���� ���� �޸� ����
	for (size_t i = 0; i < m_vAfterImgs.size(); i++)
	{
		// �ܻ��� Ȱ��ȭ���� �ʾҴٸ� ���� �ð� ����� �������� �ʴ´�.
		if (!m_vAfterImgs[i]->IsActive())
			continue;

		// Ÿ�̸� ����
		m_vTimers[i] -= DT;

		// ���� ����
		m_vAfterImgs[i]->MeshRender()->SetColor(m_vColor / 256.f);
		// Ÿ�̸Ӱ� ����Ǹ�
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

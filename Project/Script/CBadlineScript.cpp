#include "pch.h"
#include "CBadlineScript.h"

#include <Engine\CAnimator2D.h>
#include <Engine\CAnimation2D.h>

#include <Engine\CSceneMgr.h>

CBadlineScript::CBadlineScript()
	: CScript((int)SCRIPT_TYPE::BADLINESCRIPT)
	, m_ePrevState(BADLINE_STATE::NONE)
	, m_eState(BADLINE_STATE::NONE)
	, m_lTargetPos{}
	, m_fSpeed(100.f)
	, m_fStartDelay(3.f)
	, m_fRecordDelay(0.5f)
{
	SetName(L"CBadlineScript");
}

CBadlineScript::~CBadlineScript()
{
}

void CBadlineScript::start()
{
	m_eState = BADLINE_STATE::SPAWN;
}

void CBadlineScript::update()
{
	BADLINE_STATE curState = m_eState;

	PlayAnim();

	if (m_eState == BADLINE_STATE::FOLLOW)
	{
		RecordPlayer();

		m_fStartDelay -= DT;

		if (m_fStartDelay <= 0)
		{
			if (!m_lTargetPos.empty())
			{
				Vec2 targetPos = m_lTargetPos.front();
				Vec3 Pos = Transform()->GetRelativePos();

				if (abs(Pos.x - targetPos.x) > 0.0001f && abs(Pos.y - targetPos.y) > 0.0001f)
				{
					Pos = Pos.Lerp(Pos, Vec3(targetPos.x, targetPos.y, Pos.z), 0.004f);
					Transform()->SetRelativePos(Pos);
				}
				else
				{
					m_lTargetPos.pop_front();
				}
			}
		}
	}

	// 이번 프레임에 상태가 바뀌었다면
	if (curState != m_eState)
		m_ePrevState = curState;
}

void CBadlineScript::lateupdate()
{
}

void CBadlineScript::PlayAnim()
{
	CAnimator2D* animator = Animator2D();

	switch (m_eState)
	{
	case BADLINE_STATE::SPAWN:
	{
		if (m_ePrevState != BADLINE_STATE::SPAWN)
			animator->Play(L"spawn");
		
		if (animator->GetCurAnim()->IsFinish())
			m_eState = BADLINE_STATE::FOLLOW;
	}
	break;
	case BADLINE_STATE::FOLLOW:
		animator->Play(L"idle");
		break;
	case BADLINE_STATE::PLAYER_DEAD:
		animator->Play(L"booste");
		break;
	}

}

void CBadlineScript::RecordPlayer()
{
	if (m_pPlayer == nullptr)
	{
		m_pPlayer = CSceneMgr::GetInst()->FindObjectByName(ToWString("Player"));
		assert(m_pPlayer);
	}

	m_fRecordDelay -= DT;
	if (m_fRecordDelay <= 0)
	{
		m_lTargetPos.push_back(Vec2(m_pPlayer->Transform()->GetRelativePos().x, m_pPlayer->Transform()->GetRelativePos().y));
		m_fRecordDelay = 0.5f;
	}
}


void CBadlineScript::SaveToScene(FILE* _pFile)
{
}

void CBadlineScript::LoadFromScene(FILE* _pFile)
{
}

void CBadlineScript::OnChildCollisionEnter(CGameObject* _OtherObject, wstring _childName)
{
}

void CBadlineScript::OnChildCollision(CGameObject* _OtherObject, wstring _childName)
{
}

void CBadlineScript::OnChildCollisionExit(CGameObject* _OtherObject, wstring _childName)
{
}

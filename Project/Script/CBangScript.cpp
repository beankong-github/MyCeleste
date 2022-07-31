#include "pch.h"
#include "CBangScript.h"

#include "CPlayerScript.h"
#include <Engine\CAnimation2D.h>
#include <Engine\CAnimator2D.h>

CBangScript::CBangScript()
	: CScript((int)SCRIPT_TYPE::BANGSCRIPT)
	, m_pPlayerScript(nullptr)
	, m_fWaveTime(0.441f)
{
	SetName(L"CBangScript");
}

CBangScript::~CBangScript()
{
}


void CBangScript::start()
{
	m_pPlayerScript = (CPlayerScript*)GetOwner()->GetParent()->GetScriptByName(L"CPlayerScript");

	assert(m_pPlayerScript);

	m_vDefaultHairColor = m_pPlayerScript->GetNormalHairColor();
	m_vUseDashHairColor = m_pPlayerScript->GetDashHairColor();

	m_vColor = m_vDefaultHairColor;
}

void CBangScript::update()
{
	//static int iSwith = 1;
	//if (m_fWaveTime < 0)
	//{
	//	iSwith *= -1;
	//	m_fWaveTime = 0.441f;
	//}
	//else
	//{
	//	m_fWaveTime -= DT;
	//}

	CAnimator2D* pParentAnimator = GetOwner()->GetParent()->Animator2D();

	PLAYER_STATE state = m_pPlayerScript->GetPlayerState();
	PLAYER_STATE prevState = m_pPlayerScript->GetPrevPlayerState();

	Vec3 Pos = Transform()->GetRelativePos();


	switch (state)
	{
	case PLAYER_STATE::IDLE:
	{
		if (3 == pParentAnimator->GetCurAnim()->GetCurFrmIdx())
		{
			Pos.y = -0.17f;
		}
		else if (0 == pParentAnimator->GetCurAnim()->GetCurFrmIdx())
		{
			Pos.y = -0.15f;
		}
	}
	break;
	case PLAYER_STATE::RUN:
		break;
	case PLAYER_STATE::JUMP:
		break;
	case PLAYER_STATE::FALL:
		break;
	case PLAYER_STATE::DASH:
		break;
	case PLAYER_STATE::DREAM_DASH:
		GetOwner()->MeshRender()->Deactivate();
		break;
	case PLAYER_STATE::CLIME:
		break;
	case PLAYER_STATE::DANGLING:
		break;
	case PLAYER_STATE::DEAD:
		break;
	case PLAYER_STATE::END:
		break;
	}

	if(L"flip" == pParentAnimator->GetCurAnim()->GetName())
	{
		int i = pParentAnimator->GetCurAnim()->GetCurFrmIdx();
		if (2 < i && 8 > i )
		{
			Pos.x = 0.05f;
			Pos.y = -0.20f;
			GetOwner()->MeshRender()->SetSpritekey(L"bangs01.png");
		}
		else
		{
			Pos.x = 0.0f;
			Pos.y = -0.19f;
			GetOwner()->MeshRender()->SetSpritekey(L"bangs00.png");
		}
	}

	if (L"dreamDash" == pParentAnimator->GetCurAnim()->GetName())
	{
		GetOwner()->MeshRender()->Deactivate();
	}
	else
	{
		GetOwner()->MeshRender()->Activate();
	}

	Transform()->SetRelativePos(Pos);
}

void CBangScript::lateupdate()
{
	if(nullptr != MeshRender())
		MeshRender()->SetColor(m_vColor / 256.f);
}

void CBangScript::SaveToScene(FILE* _pFile)
{
}

void CBangScript::LoadFromScene(FILE* _pFile)
{
}

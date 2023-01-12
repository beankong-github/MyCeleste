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
			Pos.x = 0.f;
			Pos.y = -0.17f;
		}
		else if (0 == pParentAnimator->GetCurAnim()->GetCurFrmIdx())
		{
			Pos.x = 0.f;
			Pos.y = -0.15f;
		}
	}
	break;
	case PLAYER_STATE::FLIP:
	{
		if (0 == pParentAnimator->GetCurAnim()->GetCurFrmIdx())
		{
			Pos.x = -0.06f;
			Pos.y = -0.17f;
		}
		else if (3 >= pParentAnimator->GetCurAnim()->GetCurFrmIdx())
		{
			Pos.x = 0.0f;
			Pos.y = -0.17f;
		}
		else if (3 == pParentAnimator->GetCurAnim()->GetCurFrmIdx())
		{
			Pos.x = 0.04f;
			Pos.y = -0.17f;
		}
		else
		{
			Pos.x = 0.08f;
			Pos.y = -0.2f;
		}
	}
	break;
	case PLAYER_STATE::WALK:
	{
		Pos.x = 0.0f;
		Pos.y = -0.17f;
	}
	break;
	case PLAYER_STATE::DASH:
	{
		Pos.x = 0.0f;	
		Pos.y = -0.17f;
	}
		break;
	case PLAYER_STATE::DREAM_DASH:
		GetOwner()->MeshRender()->Deactivate();
		break;
	case PLAYER_STATE::CLIME:
		break;
	case PLAYER_STATE::JUMP:
	{
		Pos.x = 0.0f;
		Pos.y = -0.17f;
	}
		break;
	case PLAYER_STATE::DEAD:
		GetOwner()->MeshRender()->Deactivate();
		break;
	}

	wstring animName = pParentAnimator->GetCurAnim()->GetName();
	if (L"dreamDash" == animName || L"death_h" == animName)
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

	if (m_pPlayerScript->UseDash())
		m_vColor = m_vUseDashHairColor;
	else
		m_vColor = m_vDefaultHairColor;

	if(nullptr != MeshRender())
		MeshRender()->SetColor(m_vColor / 256.f);
}

void CBangScript::SaveToScene(FILE* _pFile)
{
}

void CBangScript::LoadFromScene(FILE* _pFile)
{
}

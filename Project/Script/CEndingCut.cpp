#include "pch.h"
#include "CEndingCut.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>
#include "CTransition.h"

CEndingCut::CEndingCut()
	: CScript((int)SCRIPT_TYPE::ENDINGCUT)
	, m_pTransition(nullptr)
	, m_bEventPlay(false)
{
	SetName(L"CEndingCut");
}

CEndingCut::~CEndingCut()
{
}


void CEndingCut::start()
{
	// Transition 추가
	m_pTransition = CSceneMgr::GetInst()->FindObjectByName(L"transition");
	assert(m_pTransition);

	assert(GetOwner()->Animator2D());
}

void CEndingCut::update()
{
	if (m_bEventPlay)
	{
		if (GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
		{
			CTransition* pTransition = m_pTransition->GetScript<CTransition>();

			if (nullptr != pTransition)
				pTransition->SceneEnd();

			m_bEventPlay = false;
		}
	}
}

void CEndingCut::lateupdate()
{
}

void CEndingCut::SaveToScene(FILE* _pFile)
{
}

void CEndingCut::LoadFromScene(FILE* _pFile)
{
}

void CEndingCut::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (!m_bEventPlay && L"03_col_body" == _OtherObject->GetName())
	{
		// Player Deactivate
		_OtherObject->GetParent()->Deactivate();
		
		// Animation CutScene 재생
		GetOwner()->Animator2D()->Play(L"phone");

		m_bEventPlay = true;

	}
}

void CEndingCut::OnCollision(CGameObject* _OtherObject)
{
}

void CEndingCut::OnCollisionExit(CGameObject* _OtherObject)
{
}

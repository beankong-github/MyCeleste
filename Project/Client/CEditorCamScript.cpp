#include "pch.h"
#include "CEditorCamScript.h"

#include <Engine\CKeyMgr.h>
#include <Engine\CTimeMgr.h>

#include <Engine\CTransform.h>
#include <Engine\CCamera.h>

CEditorCamScript::CEditorCamScript()
	: CScript(-1)
	, m_fSpeed(200.f)
{
}

CEditorCamScript::~CEditorCamScript()
{
}

void CEditorCamScript::update()
{
	// 카메라 위치
	Vec3 vPos = Transform()->GetRelativePos();
	// 카메라 속도
	float fSpeed = m_fSpeed;
	
	// 카메라 속도 올림
	if (KEY_PRESSED(KEY::LSHFT))
		fSpeed *= 3.f;


	// 카메라 이동
	if (KEY_PRESSED(KEY::W))
	{
		vPos.y += fSpeed * DT;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vPos.y -= fSpeed * DT;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vPos.x -= fSpeed * DT;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vPos.x += fSpeed * DT;
	}

	Transform()->SetRelativePos(vPos);


	// 카메라 확대 & 축소
	if (KEY_PRESSED(KEY::_1))
	{
		Camera()->SetWidth(Camera()->GetWidth() - DT * 100.f);
	}

	if (KEY_PRESSED(KEY::_2))
	{
		Camera()->SetWidth(Camera()->GetWidth() + DT * 100.f);
	}

}
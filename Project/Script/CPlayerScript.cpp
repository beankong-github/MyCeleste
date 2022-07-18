#include "pch.h"
#include "CPlayerScript.h"

#include <Engine\CAnimator2D.h>
#include <Engine\CAnimation2D.h>

#include <Engine\CCollider2D.h>
#include "CCollider2DScript.h"

CPlayerScript::CPlayerScript()
	: CScript((int)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_eState(PLAYER_STATE::IDLE)
	, m_fSpeed(400.f)
	, m_vVelocity(0.f, 0.f)
	, m_vForce(0.f, 0.f)
	, m_vAccel(0.f, 0.f)
	, m_fMass(1.f)
	, m_fMaxSpeed(60.f, 200.f)
	, m_fFricCoeff(230.f)
	, m_bClimb(false)
	, m_bGround(false)
	, m_bUseDash(false)
	, m_fDanglingSec(5.f)
	, m_fGravity(800.f)
	, m_vAccelA{}
	, m_pCollisionObj(nullptr)
{
	AddScriptParam("Speed", SCRIPTPARAM_TYPE::FLOAT, &m_fSpeed);
	AddScriptParam("MaxSpeed", SCRIPTPARAM_TYPE::VEC2, &m_fMaxSpeed);
	AddScriptParam("Friction Coefficient", SCRIPTPARAM_TYPE::FLOAT, &m_fFricCoeff);
	AddScriptParam("Mass", SCRIPTPARAM_TYPE::FLOAT, &m_fMass);
	AddScriptParam("m_fGravity", SCRIPTPARAM_TYPE::FLOAT, &m_fGravity);

}

CPlayerScript::~CPlayerScript()
{

}

void CPlayerScript::start()
{
	m_pColBot = GetOwner()->FindChild(L"col_bot");
	m_pColFront = GetOwner()->FindChild(L"col_front");
	m_pColBody = GetOwner()->FindChild(L"col_body");

	assert(m_pColBot);
	assert(m_pColFront);
	assert(m_pColBody);
}

void CPlayerScript::update()
{
	KeyCheck();
	PlayAnim();

	//if (KEY_TAP(KEY::B))
	//{
	//	m_bBurn = true;
	//	Ptr<CMaterial> pMtrl = MeshRender()->GetMaterial();
	//	Vec4 vColor(1.f, 0.75f, 0.5f, 0.f);
	//	pMtrl->SetScalarParam(SCALAR_PARAM::VEC4_0, &vColor);
	//}

	//Burnning();
}

void CPlayerScript::lateupdate()
{
	CalPhysics();

	if (!m_bGround)
		CalGravity();
}

void CPlayerScript::SaveToScene(FILE* _pFile)
{
	fwrite(&m_fSpeed, sizeof(float), 1, _pFile);
	fwrite(&m_fMaxSpeed, sizeof(float), 1, _pFile);
	fwrite(&m_fFricCoeff, sizeof(float), 1, _pFile);
	fwrite(&m_fMass, sizeof(float), 1, _pFile);
}

void CPlayerScript::LoadFromScene(FILE* _pFile)
{
	fread(&m_fSpeed, sizeof(float), 1, _pFile);
	fread(&m_fMaxSpeed, sizeof(float), 1, _pFile);
	fread(&m_fFricCoeff, sizeof(float), 1, _pFile);
	fread(&m_fMass, sizeof(float), 1, _pFile);
}

void CPlayerScript::KeyCheck()
{
	// 이동: 방향기
	// 점프: c
	// 대쉬: x
	// 매달리기: z 

	Vec3 vRot = Transform()->GetRelativeRotation();
	vRot.ToDegree();
	// 회전
	if (KEY_TAP(KEY::LEFT))
	{
		// 회전
		if (vRot.y != 180.f)
		{
			m_eState = PLAYER_STATE::FLIP;
			vRot.y = 180.f;
		}
	
	}
	if (KEY_TAP(KEY::RIGHT))
	{
		if (vRot.y != 0.f)
		{
			m_eState = PLAYER_STATE::FLIP;
			vRot.y = 0.f;
		}
	}

	// 이동 ( 걷기 )
	Vec2 right = Vec2(Transform()->GetRelativeRightDir().x, Transform()->GetRelativeRightDir().y);
	if (KEY_PRESSED(KEY::LEFT) || KEY_PRESSED(KEY::RIGHT))
	{	
		m_eState = PLAYER_STATE::WALK;
		//vPos.x -= DT * m_fSpeed
		AddForce(Vec2(right.x * m_fSpeed, right.y));
	}
	if (KEY_AWAY(KEY::LEFT) || KEY_AWAY (KEY::RIGHT))
	{
		m_eState = PLAYER_STATE::IDLE;
	}

	//if (KEY_PRESSED(KEY::UP))
	//	vPos.y += DT * m_fSpeed;

	//if (KEY_PRESSED(KEY::DOWN))
	//	vPos.y -= DT * m_fSpeed;

	// 점프
	if (KEY_PRESSED(KEY::Z))
	{
		// 일반 점프

		// 벽 점프
	}

	// 대쉬
	if (KEY_PRESSED(KEY::Z))
	{
	}

	// 매달리기
	if (KEY_PRESSED(KEY::Z))
	{
	}

	//// 회전
	//Vec3 vRot = Transform()->GetRelativeRotation();
	//vRot.z += DT * XM_2PI;
	//Transform()->SetRelativeRotation(vRot)

	vRot.ToRadian();
	Transform()->SetRelativeRotation(vRot);
}

void CPlayerScript::PlayAnim()
{
	CAnimator2D* animator = Animator2D();
	
	// 현재 재생중인 애니메이션이 반복 애니메이션이 아니고 끝나지 않았다면 다른 애니메이션을 재생하지 않는다.
	if (!animator->GetCurAnim()->IsRepeat() && !animator->GetCurAnim()->IsFinish())
		return;

	switch (m_eState)
	{
	case PLAYER_STATE::IDLE:
		animator->Play(L"idle");
		break;
	case PLAYER_STATE::LOOKUP:
		break;
	case PLAYER_STATE::CROUCH:
		break;
	case PLAYER_STATE::FLIP:
		animator->Play(L"flip");
		break;
	case PLAYER_STATE::WALK:
		animator->Play(L"walk");
		break;
	case PLAYER_STATE::RUN:
		break;
	case PLAYER_STATE::JUMP:
		break;
	case PLAYER_STATE::DASH:
		break;
	case PLAYER_STATE::CLIME:
		break;
	case PLAYER_STATE::DANGLING:
		break;
	case PLAYER_STATE::CUTSCENE:
		break;
	case PLAYER_STATE::DEAD:
		break;
	}
}

void CPlayerScript::CalPhysics()
{
	// 가속도
	m_vAccel = (m_vForce / m_fMass) * DT; // 초당 가속도 = 힘의 방향 * 가속도의 크기 * DeltaTime
	m_vAccel += m_vAccelA * DT;
	// 속도
	m_vVelocity += m_vAccel;
	
	// 속도 제한 검사
	if (m_fMaxSpeed.x <= abs(m_vVelocity.x))
	{
		float y = m_vVelocity.y;
		m_vVelocity.Normalize();
		m_vVelocity.x *= m_fMaxSpeed.x;
		m_vVelocity.y = y;
	}
	if (m_fMaxSpeed.y <= abs(m_vVelocity.y))
	{
		float x = m_vVelocity.x;
		m_vVelocity.Normalize();
		m_vVelocity.x = x;
		m_vVelocity.y *= m_fMaxSpeed.y;
	}

	// 마찰력
	Vec2 vFric = m_vVelocity;
	vFric.Normalize();
	vFric *= -1;
	vFric = vFric * m_fFricCoeff * DT;


	if (vFric.Length() >= m_vVelocity.Length())
		m_vVelocity = Vec2(0.f, 0.f);
	else m_vVelocity += vFric;

	// 이동
	Move();

	m_vForce = Vec2(0.f, 0.f);
	m_vAccel = Vec2(0.f, 0.f);
	m_vAccelA = Vec2(0.f, 0.f);
}

void CPlayerScript::CalGravity()
{
	SetAccelAlpha(Vec2( 0.f, -m_fGravity ));
}

void CPlayerScript::Move()
{
	Vec3 vPos = Transform()->GetRelativePos();
	vPos.x += m_vVelocity.x * DT;
	vPos.y += m_vVelocity.y * DT;
	Transform()->SetRelativePos(vPos);
}

void CPlayerScript::OnChildCollisionEnter(CGameObject* _OtherObject, wstring _childName)
{
	CGameObject* pColObj = GetOwner()->FindChild(_childName);
	CCollider2DScript* pOtherColScript = (CCollider2DScript*)_OtherObject->GetScriptByName(L"CCollider2DScript");
	
	if (nullptr == pOtherColScript)
		return;
	
	// Ground Check
	if (COLLIDER2D_PROPERTY::GROUND == pOtherColScript->GetColliderProterty() && _childName == L"col_bot")
	{
		m_bGround = true;
		m_pCollisionObj = _OtherObject;

		CCollider2D* pThisCol = m_pColBot->Collider2D();
		CCollider2D* pOtherCol = m_pCollisionObj->Collider2D();

		// 밀린 만큼 위로 올려주기
		Vec2 thisColPos = Vec2{ Transform()->GetWorldPos().x + pThisCol->GetOffsetPos().x,
		Transform()->GetWorldPos().y + pThisCol->GetOffsetPos().y };
		Vec2 otherColPos = Vec2{ m_pCollisionObj->Transform()->GetWorldPos().x + pOtherCol->GetOffsetPos().x, m_pCollisionObj->Transform()->GetWorldPos().y + pOtherCol->GetOffsetPos().y };

		float fLen = abs(thisColPos.y - otherColPos.y);

		fLen = (pThisCol->GetOffsetScale().y / 2.f + pOtherCol->GetOffsetScale().y / 2.f) - fLen;

		Vec3 Pos = Transform()->GetRelativePos();
		Pos.y += fLen;
		Transform()->SetRelativePos(Pos);

		m_vVelocity = Vec2(0.f, 0.f);
	}
}

void CPlayerScript::OnChildCollision(CGameObject* _OtherObject, wstring _childName)
{
	CGameObject* pCol = GetOwner()->FindChild(_childName);
	CCollider2DScript* pOtherColScript = (CCollider2DScript*)_OtherObject->GetScriptByName(L"CCollider2DScript");

	if (nullptr == pOtherColScript)
		return;

	if (COLLIDER2D_PROPERTY::GROUND == pOtherColScript->GetColliderProterty() && _childName == L"col_front")
	{
		m_pCollisionObj = _OtherObject;

		CCollider2D* pThisCol = m_pColBot->Collider2D();
		CCollider2D* pOtherCol = m_pCollisionObj->Collider2D();

		// 밀린 만큼 위로 올려주기
		Vec2 thisColPos = Vec2{ Transform()->GetWorldPos().x + pThisCol->GetOffsetPos().x,
		Transform()->GetWorldPos().y + pThisCol->GetOffsetPos().y };
		Vec2 otherColPos = Vec2{ m_pCollisionObj->Transform()->GetWorldPos().x + pOtherCol->GetOffsetPos().x, m_pCollisionObj->Transform()->GetWorldPos().y + pOtherCol->GetOffsetPos().y };

		float fLen = abs(thisColPos.x - otherColPos.x);

		fLen = (pThisCol->GetOffsetScale().x / 2.f + pOtherCol->GetOffsetScale().x / 2.f) - fLen;

		Vec3 Pos = Transform()->GetRelativePos();
		Pos.x += fLen;
		Transform()->SetRelativePos(Pos);


		m_vVelocity = Vec2(0.f, m_vVelocity.y);
	}

}

void CPlayerScript::OnChildCollisionExit(CGameObject* _OtherObject, wstring _childName)
{
	CGameObject* pColObj = GetOwner()->FindChild(_childName);
	CCollider2DScript* pOtherCol = (CCollider2DScript*)_OtherObject->GetScriptByName(L"CCollider2DScript");

	if (nullptr == pOtherCol)
		return;


	// Ground Check
	if (COLLIDER2D_PROPERTY::GROUND == pOtherCol->GetColliderProterty() && _childName == L"col_bot")
	{
		m_bGround = false;
		m_pCollisionObj = nullptr;
	}
}



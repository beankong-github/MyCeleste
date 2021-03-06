#include "pch.h"
#include "CPlayerScript.h"

#include <Engine\CAnimator2D.h>
#include <Engine\CAnimation2D.h>
#include <Engine\CCollider2D.h>

#include "CCollider2DScript.h"
#include "CPhysics.h"

CPlayerScript::CPlayerScript()
	: CScript((int)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_eState(PLAYER_STATE::IDLE)
	, m_ePrevState(PLAYER_STATE::IDLE)
	, m_vNormalHairColor(172.f, 50.f, 50.f, 256.f)
	, m_vDashHairColor(68.f, 183.f, 255.f, 256.f)
	, m_vMaxSpeed(80.f, 160.f)
	, m_fSpeed(70.0f)
	, m_fDashTime(0.23f)
	, m_fDanglingSec(5.f)
	, m_bClimb(false)
	, m_bGround(false)
	, m_bDash(false)
	, m_bWall(false)
	, m_bUseDash(false)
	, m_bUseJump(false)
	, m_bCollision(false)
	, m_bClimbMove(false)
	, m_bFlip(false)
	, m_pPhysics(nullptr)
	, m_pCollisionObj(nullptr)
	, m_pColBot(nullptr)
	, m_pColFront(nullptr)
	, m_pColBody(nullptr)
{
	SetName(L"CPlayerScript");

	AddScriptParam("Speed", SCRIPTPARAM_TYPE::FLOAT, &m_fSpeed);
	AddScriptParam("MaxSpeed", SCRIPTPARAM_TYPE::VEC2, &m_vMaxSpeed);

}

CPlayerScript::~CPlayerScript()
{

}

void CPlayerScript::start()
{
	// 자식 Collider 찾기 
	m_pColBot = GetOwner()->FindChild(L"01_col_bot");
	m_pColFront = GetOwner()->FindChild(L"02_col_front");
	m_pColBody = GetOwner()->FindChild(L"03_col_body");

	assert(m_pColBot);
	assert(m_pColFront);
	assert(m_pColBody);

	// Gravity Script 추가
	m_pPhysics = (CPhysics*)GetOwner()->GetScript<CPhysics>();
	assert(m_pPhysics);
	// 최대 속력 추가
	m_pPhysics->SetMaxXSpeed(m_vMaxSpeed.x);
	m_pPhysics->SetMaxYSpeed(m_vMaxSpeed.y);
}

void CPlayerScript::update()
{
	PLAYER_STATE curState = m_eState;

	Vec4 color = m_vNormalHairColor / 256.f;

	KeyCheck();
	PlayAnim();

	// 이번 프레임에 상태가 바뀌었다면
	if (curState != m_eState)
		m_ePrevState = m_eState;
}

void CPlayerScript::lateupdate()
{
}

void CPlayerScript::SaveToScene(FILE* _pFile)
{
}

void CPlayerScript::LoadFromScene(FILE* _pFile)
{
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
		if (vRot.y != 180.f && !m_bClimb)
		{
			m_eState = PLAYER_STATE::FLIP;
			m_bFlip = true;
			vRot.y = 180.f;
		}
	}
	if (KEY_TAP(KEY::RIGHT))
	{
		if (vRot.y != 0.f && !m_bClimb)
		{
			m_eState = PLAYER_STATE::FLIP;
			m_bFlip = true;
			vRot.y = 0.f;
		}
	}

	vRot.ToRadian();
	Transform()->SetRelativeRotation(vRot);


	Vec3 pos = Transform()->GetRelativePos();
	Vec2 front = Vec2(Transform()->GetRelativeRightDir().x, Transform()->GetRelativeRightDir().y);
	Vec2 up = Vec2(Transform()->GetRelativeUpDir().x, Transform()->GetRelativeUpDir().y);

	if (m_eState == PLAYER_STATE::DREAM_DASH)
	{
		pos.x += m_fDreamDashDir.x * 400.f * DT;
		pos.y += m_fDreamDashDir.y * 400.f * DT;
		if (nullptr != m_pPhysics)
			m_pPhysics->OnCollision();

		if (m_bWall)
		{
			// 대쉬 종료
			m_eState == PLAYER_STATE::IDLE;
			pos.x -= m_fDreamDashDir.x * 400.f * DT;
			m_bDash = false;
			m_fDashTime = -1.f;

			// 플레이어 사망
			Dead();
		}
		if (!m_bGround && m_bCollision)
		{
			m_eState == PLAYER_STATE::IDLE;
			pos.y -= m_fDreamDashDir.y * 400.f * DT;
			m_bDash = false;
			m_fDashTime = -1.f;

			// 플레이어 사망
			Dead();
		}

		Transform()->SetRelativePos(pos);
		return;
	}
	else
	{
		m_bUseDash = false;
		m_fDashTime = 0.23f;
		m_fDreamDashDir = Vec2(0.f, 0.f);
	}

	// 이동 ( 걷기 )
	if (KEY_PRESSED(KEY::LEFT) || KEY_PRESSED(KEY::RIGHT))
	{
		if (m_bGround && !m_bDash && !m_bFlip && !m_bClimb)
			m_eState = PLAYER_STATE::WALK;

		if (!m_bWall)
			pos.x += front.x * m_fSpeed * DT;
	}

	// 점프
	if (KEY_TAP(KEY::C))
	{
		// 일반 점프
		if (m_bGround)
		{
			m_eState = PLAYER_STATE::JUMP;
			m_bUseJump = true;
			m_pPhysics->GetVelocity() += Vec2(0.f, 400.f);
		}
	}

	// 대쉬
	if (KEY_PRESSED(KEY::X))
	{
		if (!m_bUseDash && !m_bWall)
		{
			if (KEY_PRESSED(KEY::LEFT) || KEY_PRESSED(KEY::RIGHT))
				m_fDashDir += front;
			if (KEY_PRESSED(KEY::UP))
				m_fDashDir += up;
			if (KEY_PRESSED(KEY::DOWN))
				m_fDashDir += -up;

			m_fDashDir.Normalize();

			m_bUseDash = true;
			m_bDash = true;
			m_eState = PLAYER_STATE::DASH;
		}
	}
	if (m_bUseDash)
	{
		// 대쉬 지속 시간이 끝났다면
		if (m_fDashTime < 0)
		{
			// Dash 방향 초기화
			if (m_bDash)
			{
				m_eState = PLAYER_STATE::IDLE;
				// 대쉬 끝
				m_bDash = false;
			}

			// 착지하면 대쉬를 재사용할 수 있게 UseDash 초기화
			if (m_bGround)
			{
				m_bUseDash = false;
				m_fDashTime = 0.23f;
				m_fDashDir = Vec2(0.f, 0.f);
			}
		}
		else
		{
			// 현재 상태가 대쉬일 때
			if (m_eState == PLAYER_STATE::DASH)
			{
				m_fDashTime -= DT;
				m_bDash = true;

				pos.x += m_fDashDir.x * 130.f * DT;
				pos.y += m_fDashDir.y * 130.f * DT;

				// 벽에 박으면
				if (m_bWall)
				{
					// 대쉬 종료
					pos.x -= m_fDashDir.x * 130.f * DT;
					m_bDash = false;
					m_fDashTime = -1.f;
				}

				// 머리 박으면
				if (!m_bGround && m_bCollision)
				{
					pos.y -= m_fDashDir.y * 130.f * DT;
					m_bDash = false;
					m_fDashTime = -1.f;
				}
			}

		}
	}

	Transform()->SetRelativePos(pos);

	// Idle
	if (Vec2(0.f, 0.f) == m_pPhysics->GetVelocity() && m_bGround && !m_bFlip)
	{
		m_eState = PLAYER_STATE::IDLE;
	}

	// 매달리기
	if (KEY_PRESSED(KEY::Z))
	{
		if (m_bWall)
		{
			if (!m_bClimb)
			{
				m_eState = PLAYER_STATE::CLIME;
				m_bClimb = true;
				if (nullptr != m_pPhysics)
					m_pPhysics->StartClimb();
			}

			if (m_bClimb)
			{
				m_bClimbMove = false;

				if (KEY_PRESSED(KEY::UP))
				{
					if (m_bClimb && !m_bCollision)
					{
						m_eState = PLAYER_STATE::CLIME;
						pos.y += m_fSpeed * DT;
						m_bClimbMove = true;
					}
				}
				if (KEY_PRESSED(KEY::DOWN))
				{
					if (m_bClimb && !m_bGround)
					{
						m_eState = PLAYER_STATE::CLIME;
						pos.y -= m_fSpeed * DT;
						m_bClimbMove = true;
					}
				}

			}
		}
	}

	if (KEY_AWAY(KEY::Z))
	{
		// 바닥으로 추락
		if (m_bClimb)
		{
			m_eState = PLAYER_STATE::IDLE;
			m_bClimb = false;
			if (nullptr != m_pPhysics)
				m_pPhysics->EndClimb();
		}
	}


	Transform()->SetRelativePos(pos);

		// Jump
		//if (KEY_TAP(KEY::C))
		//{
		//	m_eState = PLAYER_STATE::JUMP;
		//	m_bUseJump = true;
		//	m_bClimb = false;

		//	Vec2 jDir;
		//	// 회전
		//	if (vRot.y != 180.f)
		//	{
		//		vRot.y = 180.f;
		//		jDir = Vec2(-1.f, 1.f);
		//	}
		//	else
		//	{
		//		vRot.y = 0.f;
		//		jDir = Vec2(1.f, 1.f);
		//	}


	// 떨어지기
	//if (m_vVelocity.y < 0)
	//	m_eState = PLAYER_STATE::FALL;

}

void CPlayerScript::PlayAnim()
{
	CAnimator2D* animator = Animator2D();

	// 현재 재생중인 애니메이션이 반복 애니메이션이 아니고 끝나지 않았다면 다른 애니메이션을 재생하지 않는다.

	switch (m_eState)
	{
	case PLAYER_STATE::IDLE:
		animator->Play(L"idle");
		break;
	case PLAYER_STATE::LOOKUP:
	{
		m_eState = PLAYER_STATE::IDLE;
		animator->Play(L"lookUp");
	}
	break;
	case PLAYER_STATE::CROUCH:
		break;
	case PLAYER_STATE::FLIP:
		if (m_ePrevState != PLAYER_STATE::FLIP)
			animator->Play(L"flip");
		else
			if (animator->GetCurAnim()->IsFinish())
				m_bFlip = false;
		break;
	case PLAYER_STATE::WALK:
		animator->Play(L"walk");
		break;
	case PLAYER_STATE::RUN:
		break;
	case PLAYER_STATE::JUMP:
	{
		if (m_ePrevState != PLAYER_STATE::JUMP)
		{
			if (m_pPhysics->GetVelocity().y < 300.f)
				animator->Play(L"jumpSlow");
			else
				animator->Play(L"jumpFast");
		}

		if (animator->GetCurAnim()->IsFinish() && m_bGround)
		{
			m_eState = PLAYER_STATE::IDLE;
		}
	}
	break;
	case PLAYER_STATE::FALL:
	{
		if (m_ePrevState != PLAYER_STATE::FALL)
			animator->Play(L"fallPose");
		else
			if (animator->GetCurAnim()->IsFinish())
				m_eState = PLAYER_STATE::IDLE;
	}
	break;
	case PLAYER_STATE::DASH:
		if (m_ePrevState != PLAYER_STATE::DASH)
			animator->Play(L"dash");
		break;
	case PLAYER_STATE::DREAM_DASH:
	{
		if (m_ePrevState != PLAYER_STATE::DREAM_DASH)
			animator->Play(L"dreamDash");
	}
	break;
	case PLAYER_STATE::CLIME:
	{
		if (m_bClimbMove)
			animator->Play(L"climb");
		else
			animator->Play(L"climb_idle");
		break;
	}
	case PLAYER_STATE::DANGLING:
		animator->Play(L"dangling");
		break;
	case PLAYER_STATE::CUTSCENE:
		break;
	case PLAYER_STATE::DEAD:
		break;
	}
}

void CPlayerScript::Dead()
{
}

void CPlayerScript::OnChildCollisionEnter(CGameObject* _OtherObject, wstring _childName)
{
	CGameObject* pColObj = GetOwner()->FindChild(_childName);
	CCollider2DScript* pOtherColScript = (CCollider2DScript*)_OtherObject->GetScriptByName(L"CCollider2DScript");

	if (nullptr == pOtherColScript)
		return;

	// Wall Check	
	if (COLLIDER2D_PROPERTY::WALL == pOtherColScript->GetColliderProterty() && _childName == L"02_col_front")
	{
		m_bWall = true;
		if (nullptr != m_pPhysics)
			m_pPhysics->OnWall();
		m_lCurWall.push_back(_OtherObject);
	}

	// Ground Check
	if (COLLIDER2D_PROPERTY::GROUND == pOtherColScript->GetColliderProterty() && _childName == L"01_col_bot")
	{
		m_bGround = true;
		if (nullptr != m_pPhysics)
			m_pPhysics->OnGround();
		m_lCurGround.push_back(_OtherObject);
	}

	if (COLLIDER2D_PROPERTY::WALL == pOtherColScript->GetColliderProterty() && _childName == L"01_col_bot")
	{
		if (!m_bWall)
		{
			m_bGround = true;
			if (nullptr != m_pPhysics)
				m_pPhysics->OnCollision();

			m_lCurGround.push_back(_OtherObject);
		}
	}

	// etc collision
	if (_childName == L"03_col_body")
	{
		if (COLLIDER2D_PROPERTY::WALL == pOtherColScript->GetColliderProterty() || COLLIDER2D_PROPERTY::GROUND == pOtherColScript->GetColliderProterty())
			if (!m_bGround)
			{
				m_bCollision = true;
				if (nullptr != m_pPhysics)
					m_pPhysics->OnCollision();
			}
	}

	// DreamBlock Check
	if (COLLIDER2D_PROPERTY::DREAM_BLOCK == pOtherColScript->GetColliderProterty() && _childName == L"03_col_body")
	{
		if (m_bDash && PLAYER_STATE::DREAM_DASH != m_eState)
		{
			m_eState = PLAYER_STATE::DREAM_DASH;
		
			Vec2 right = Vec2(Transform()->GetRelativeRightDir().x, Transform()->GetRelativeRightDir().y);
			Vec2 up = Vec2(Transform()->GetRelativeUpDir().x, Transform()->GetRelativeUpDir().y);
			
			m_fDreamDashDir = Vec2(0.f, 0.f);

			if (KEY_PRESSED(KEY::LEFT) || KEY_PRESSED(KEY::RIGHT))
				m_fDreamDashDir += right;
			if (KEY_PRESSED(KEY::UP))
				m_fDreamDashDir += up;
			if (KEY_PRESSED(KEY::DOWN))
				m_fDreamDashDir += -up;

			m_fDreamDashDir.Normalize();
		}
	}
}

void CPlayerScript::OnChildCollision(CGameObject* _OtherObject, wstring _childName)
{


}

void CPlayerScript::OnChildCollisionExit(CGameObject* _OtherObject, wstring _childName)
{
	CGameObject* pColObj = GetOwner()->FindChild(_childName);
	CCollider2DScript* pOtherCol = (CCollider2DScript*)_OtherObject->GetScriptByName(L"CCollider2DScript");

	if (nullptr == pOtherCol)
		return;


	// Ground Check
	if (_childName == L"01_col_bot")
	{
		m_lCurGround.erase(std::remove(m_lCurGround.begin(), m_lCurGround.end(), _OtherObject), m_lCurGround.end());
		if (m_lCurGround.empty())
		{
			m_bGround = false;
			if (nullptr != m_pPhysics)
				m_pPhysics->OnAir();
		}
	}

	// Wall Check
	if (COLLIDER2D_PROPERTY::WALL == pOtherCol->GetColliderProterty() && _childName == L"02_col_front")
	{
		m_lCurWall.erase(std::remove(m_lCurWall.begin(), m_lCurWall.end(), _OtherObject), m_lCurWall.end());

		if (m_lCurWall.empty())
			m_bWall = false;
	}

	// else
	if (_childName == L"03_col_body")
	{
		if (COLLIDER2D_PROPERTY::WALL == pOtherCol->GetColliderProterty() || COLLIDER2D_PROPERTY::GROUND == pOtherCol->GetColliderProterty())
			if (m_bCollision)
			{
				m_bCollision = false;
			}
	}

	// DreamBlock Check
	if (COLLIDER2D_PROPERTY::DREAM_BLOCK == pOtherCol->GetColliderProterty() && _childName == L"03_col_body")
	{
		m_eState = PLAYER_STATE::IDLE;
		m_bDash = false;
		m_bUseDash = false;
		m_fDashTime = 0.23f;
		m_fDreamDashDir = Vec2(0.f, 0.f);
		m_fDashDir = Vec2(0.f, 0.f);
	}
}



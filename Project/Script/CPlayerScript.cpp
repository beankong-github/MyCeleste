#include "pch.h"
#include "CPlayerScript.h"
#include <random>

#include <Engine\CAnimator2D.h>
#include <Engine\CAnimation2D.h>
#include <Engine\CCollider2D.h>

#include "CCollider2DScript.h"
#include "CPhysics.h"
#include "CTransition.h"
#include "CCamTrigger.h"

CPlayerScript::CPlayerScript()
	: CScript((int)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_eState(PLAYER_STATE::IDLE)
	, m_ePrevState(PLAYER_STATE::IDLE)
	, m_eStage(STAGE::S00)
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
	, m_bWalk(false)
	, m_pPhysics(nullptr)
	, m_pCollisionObj(nullptr)
	, m_pColBot(nullptr)
	, m_pColFront(nullptr)
	, m_pColBody(nullptr)
	, m_pTransition(nullptr)
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
	// �ڽ� Collider ã�� 
	m_pColBot = GetOwner()->FindChild(L"01_col_bot");
	m_pColFront = GetOwner()->FindChild(L"02_col_front");
	m_pColBody = GetOwner()->FindChild(L"03_col_body");

	assert(m_pColBot);
	assert(m_pColFront);
	assert(m_pColBody);

	// Gravity Script �߰�
	m_pPhysics = (CPhysics*)GetOwner()->GetScript<CPhysics>();
	assert(m_pPhysics);

	// �ִ� �ӷ� �߰�
	m_pPhysics->SetMaxXSpeed(m_vMaxSpeed.x);
	m_pPhysics->SetMaxYSpeed(m_vMaxSpeed.y);

	// Transition �߰�
	m_pTransition = CSceneMgr::GetInst()->FindObjectByName(L"transition");
	assert(m_pTransition);

	// bg ���� ���
	Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\bg.mp3", L"sound\\bg.mp3");
	pSound->Play(0, 0.5f);

	pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\env_amb_02_dream.ogg", L"env_amb_02_dream.ogg");
	pSound->Play(0, 0.3f);

}

void CPlayerScript::update()
{
	PLAYER_STATE curState = m_eState;

	Vec4 color = m_vNormalHairColor / 256.f;

	if(curState != PLAYER_STATE::DEAD)
		KeyCheck();

	PlayAnim();

	// �̹� �����ӿ� ���°� �ٲ���ٸ�
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
	// �̵�: �����
	// ����: c
	// �뽬: x
	// �Ŵ޸���: z 

	Vec3 vRot = Transform()->GetRelativeRotation();
	vRot.ToDegree();

	// ȸ��
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
		// �뽬
		pos.x += m_fDreamDashDir.x * 400.f * DT;
		pos.y += m_fDreamDashDir.y * 400.f * DT;
		if (nullptr != m_pPhysics)
			m_pPhysics->OnCollision();
		
		// ���� ���
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\player\\char_mad_dreamblock_travel_loop.ogg", L"sound\\player\\char_mad_dreamblock_travel_loop.ogg");
		pSound->Play(1, 0.5f);

		if (m_bWall)
		{
			// �뽬 ����
			m_eState = PLAYER_STATE::IDLE;
			pos.x -= m_fDreamDashDir.x * 400.f * DT;
			m_bDash = false;
			m_fDashTime = -1.f;

			pSound->Stop();

			// �÷��̾� ���
			Dead();
		}
		if (!m_bGround && m_bCollision)
		{
			m_eState = PLAYER_STATE::IDLE;
			pos.y -= m_fDreamDashDir.y * 400.f * DT;
			m_bDash = false;
			m_fDashTime = -1.f;

			pSound->Stop();

			// �÷��̾� ���
			Dead();
		}

		Transform()->SetRelativePos(pos);
		return;
	}
	else
	{
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\player\\char_mad_dreamblock_travel_loop.ogg", L"sound\\player\\char_mad_dreamblock_travel_loop.ogg");
		pSound->Stop();
	}

	// �̵� ( �ȱ� )
	float term = 0.35f;
	static float termCounter = 0.f;
	if (KEY_PRESSED(KEY::LEFT) || KEY_PRESSED(KEY::RIGHT))
	{
		if (m_bGround && !m_bDash && !m_bFlip && !m_bClimb)
		{
			m_eState = PLAYER_STATE::WALK;
			m_bWalk = true;

			if (termCounter <= 0.f)
			{
				// ���� ���
				// �õ尪�� ��� ���� random_device ����.
				std::random_device rd;
				// random_device �� ���� ���� ���� ������ �ʱ�ȭ �Ѵ�.
				std::mt19937 gen(rd());
				// 0 ���� 99 ���� �յ��ϰ� ��Ÿ���� �������� �����ϱ� ���� �յ� ���� ����.
				std::uniform_int_distribution<int> dis(1, 6);
				int rndNum = dis(gen);
				wstring skey = L"sound\\brick\\game_gen_debris_stone_soft_0" + to_wstring(rndNum) + L".ogg";
				Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(skey, skey);
				pSound->Play(1, 0.5f, true);

				termCounter = term;
			}
			else
			{
				termCounter -= DT;
			}
		}
		if (!m_bWall)
			pos.x += front.x * m_fSpeed * DT;
	}
	if (KEY_AWAY(KEY::LEFT) || KEY_AWAY(KEY::RIGHT))
	{
		m_bWalk = false;
		termCounter = 0.f;
	}

	// ����
	if (KEY_TAP(KEY::C))
	{
		// �Ϲ� ����
		if (m_bGround)
		{
			m_eState = PLAYER_STATE::JUMP;
			m_bUseJump = true;
			m_pPhysics->GetVelocity() += Vec2(0.f, 400.f);
			// ���� ���
			Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\player\\char_mad_jump.ogg", L"sound\\player\\char_mad_jump.ogg");
			pSound->Play(1, 0.5f, true);
		}
	}

	// �뽬
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
		// �뽬 ���� �ð��� �����ٸ�
		if (m_fDashTime < 0)
		{
			// Dash ���� �ʱ�ȭ
			if (m_bDash)
			{
				m_eState = PLAYER_STATE::IDLE;
				// �뽬 ��
				m_bDash = false;
			}

			// �����ϸ� �뽬�� ������ �� �ְ� UseDash �ʱ�ȭ
			if (m_bGround)
			{
				m_bUseDash = false;
				m_fDashTime = 0.23f;
				m_fDashDir = Vec2(0.f, 0.f);
			}
		}
		else
		{
			m_fDashTime -= DT;
			m_bDash = true;
			
			pos.x += m_fDashDir.x * 130.f * DT;
			pos.y += m_fDashDir.y * 130.f * DT;

			// ���� ������
			if (m_bWall)
			{
				// �뽬 ����
				pos.x -= m_fDashDir.x * 130.f * DT;
				m_bDash = false;
				m_fDashTime = -1.f;
			}

			// �Ӹ� ������
			if (!m_bGround && m_bCollision)
			{
				pos.y -= m_fDashDir.y * 130.f * DT;
				m_bDash = false;
				m_fDashTime = -1.f;

				if (PLAYER_STATE::DREAM_DASH == m_eState)
					Dead();
			}

		}
	}

	if (KEY_PRESSED(KEY::UP))
	{
		if (!m_bGround && !m_bCollision)
		{
			pos.y += m_fSpeed / 4.f * DT;
		}
	}
	if (KEY_PRESSED(KEY::DOWN))
	{
		if (!m_bGround)
		{
			pos.y -= m_fSpeed / 4.f * DT;
		}
	}


	Transform()->SetRelativePos(pos);

	// Idle
	if (Vec2(0.f, 0.f) == m_pPhysics->GetVelocity() && m_bGround && !m_bFlip && !m_bWalk)
	{
		m_eState = PLAYER_STATE::IDLE;
	}

	// �Ŵ޸���
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
		// �ٴ����� �߶�
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
		//	// ȸ��
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


	// ��������
	//if (m_vVelocity.y < 0)
	//	m_eState = PLAYER_STATE::FALL;

}

void CPlayerScript::PlayAnim()
{
	CAnimator2D* animator = Animator2D();

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
		{
			animator->Play(L"flip");
			
			// ���� ���
			// �õ尪�� ��� ���� random_device ����.
			std::random_device rd;
			// random_device �� ���� ���� ���� ������ �ʱ�ȭ �Ѵ�.
			std::mt19937 gen(rd());
			// 0 ���� 99 ���� �յ��ϰ� ��Ÿ���� �������� �����ϱ� ���� �յ� ���� ����.
			std::uniform_int_distribution<int> dis(1, 6);
			int rndNum = dis(gen);
			wstring skey = L"sound\\brick\\game_gen_debris_stone_soft_0" + to_wstring(rndNum) + L".ogg";
			Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(skey, skey);
			pSound->Play(1, 0.5f, true);
		}
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
		{
			animator->Play(L"dash");
			// ���� ���
			Ptr<CSound> pSound;
			if (KEY_PRESSED(KEY::LEFT))
				pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\player\\char_mad_dash_pink_left.ogg", L"sound\\player\\char_mad_dash_pink_left.ogg");
			else if (KEY_PRESSED(KEY::RIGHT))
				pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\player\\char_mad_dash_pink_right.ogg", L"sound\\player\\char_mad_dash_pink_right.ogg");
			pSound->Stop();
			pSound->Play(1, 0.5f);
		}
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
	{
		if (animator->GetCurAnim()->GetName() != L"death_h")
		{
			animator->Play(L"death_h");
		}
		else
		{
			static CTransition* pTransition = m_pTransition->GetScript<CTransition>();

			if(animator->GetCurAnim()->GetCurFrmIdx() == 7)
				pTransition->Play();

			if (animator->GetCurAnim()->IsFinish())
			{
				Respwan();
			}
		}

	}
		break;
	}
}

void CPlayerScript::Dead()
{
	m_eState = PLAYER_STATE::DEAD;
	m_pPhysics->Deactivate();
	
	// ���� ���
	Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\player\\char_mad_death.ogg", L"sound\\player\\char_mad_death.ogg");
	pSound->Play(1, 0.8f, true);
}

void CPlayerScript::Respwan()
{

	m_eState = PLAYER_STATE::IDLE;
	m_pPhysics->Activate();

	switch (m_eStage)
	{
	case STAGE::S00:
		Transform()->SetRelativePos(Vec3(-120.f, 121.f, Transform()->GetRelativePos().z));
		break;
	case STAGE::S01:		
		Transform()->SetRelativePos(Vec3(83.f, 57.f, Transform()->GetRelativePos().z));
		break;
	case STAGE::S02:
		Transform()->SetRelativePos(Vec3(110.f, -111.f, Transform()->GetRelativePos().z));
		break;
	case STAGE::S03:		
		Transform()->SetRelativePos(Vec3(-134.f, -246.f, Transform()->GetRelativePos().z));
		break;
	case STAGE::S04:
		break;
	case STAGE::S05:
		break;
	}

	// ���� ���
	Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\player\\char_mad_revive.ogg", L"sound\\player\\char_mad_revive.ogg");
	pSound->Play(1, 0.8f);
}

void CPlayerScript::OnChildCollisionEnter(CGameObject* _OtherObject, wstring _childName)
{
	CGameObject* pColObj = GetOwner()->FindChild(_childName);
	CCollider2DScript* pOtherColScript = (CCollider2DScript*)_OtherObject->GetScriptByName(L"CCollider2DScript");

	if (nullptr == pOtherColScript)
		return;

	// Dead Check
	if (COLLIDER2D_PROPERTY::SPIKE == pOtherColScript->GetColliderProterty() && _childName == L"03_col_body")
	{
		Dead();
		return;
	}

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
	if (COLLIDER2D_PROPERTY::DREAM_BLOCK == pOtherColScript->GetColliderProterty())
	{
		if (_childName == L"03_col_body")
		{
			if (m_bDash && PLAYER_STATE::DREAM_DASH != m_eState)
			{
				// ���� ����
				m_eState = PLAYER_STATE::DREAM_DASH;

				// ����, ���� ���� ���� ��������
				Vec2 front = Vec2(Transform()->GetRelativeRightDir().x, Transform()->GetRelativeRightDir().y);
				Vec2 up = Vec2(Transform()->GetRelativeUpDir().x, Transform()->GetRelativeUpDir().y);

				// �帲 �뽬 ���� ���ϱ�
				m_fDreamDashDir = Vec2(0.f, 0.f);

				if (KEY_PRESSED(KEY::LEFT) || KEY_PRESSED(KEY::RIGHT))
					m_fDreamDashDir += front;
				if (KEY_PRESSED(KEY::UP))
					m_fDreamDashDir += up;
				if (KEY_PRESSED(KEY::DOWN))
					m_fDreamDashDir += -up;

				m_fDreamDashDir.Normalize();
				
				// ���� ���

				wstring soundKey = L"sound\\player\\char_mad_dreamblock_enter_01.ogg";
				Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(soundKey, soundKey);
				pSound->Play(1, 0.8f, true);
			}
		}

		else if (_childName == L"01_col_bot")
		{
			if (!m_bDash && PLAYER_STATE::DREAM_DASH != m_eState && !m_bWall)
			{
				m_bGround = true;
				if (nullptr != m_pPhysics)
					m_pPhysics->OnGround();
				m_lCurGround.push_back(_OtherObject);
			}
		}

		else if (_childName == L"02_col_front")
		{
			if (!m_bDash && PLAYER_STATE::DREAM_DASH != m_eState)
			{
				m_bWall = true;
				if (nullptr != m_pPhysics)
					m_pPhysics->OnWall();
				m_lCurWall.push_back(_OtherObject);
			}
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
	if (COLLIDER2D_PROPERTY::DREAM_BLOCK == pOtherCol->GetColliderProterty())
	{
		if (_childName == L"03_col_body")
		{
			m_eState = PLAYER_STATE::IDLE;
			m_bDash = false;
			m_bUseDash = false;
			m_fDashTime = 0.23f;
			m_fDreamDashDir = Vec2(0.f, 0.f);
			m_fDashDir = Vec2(0.f, 0.f);

			// ���� ���
			wstring soundKey = L"sound\\player\\char_mad_dreamblock_exit_01.ogg";
			Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(soundKey, soundKey);
			pSound->Play(1, 0.8f, true);
		}

		else if (_childName == L"01_col_bot")
		{
			m_lCurGround.erase(std::remove(m_lCurGround.begin(), m_lCurGround.end(), _OtherObject), m_lCurGround.end());
			if (m_lCurGround.empty())
			{
				m_bGround = false;
				if (nullptr != m_pPhysics)
					m_pPhysics->OnAir();
			}
		}

		else if (_childName == L"02_col_front")
		{
			m_lCurWall.erase(std::remove(m_lCurWall.begin(), m_lCurWall.end(), _OtherObject), m_lCurWall.end());

			if (m_lCurWall.empty())
				m_bWall = false;

		}
	}
}



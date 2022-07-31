#include "pch.h"
#include "CCamTrigger.h"

#include <Engine\CRenderMgr.h>
#include <Engine\CResMgr.h>
#include <Engine\CEventMgr.h>
#include <Engine\CPrefab.h>

CCamTrigger::CCamTrigger()
	: CScript((int)SCRIPT_TYPE::CAMTRIGGER)
	, m_pMainCam(nullptr)
	, m_bCamMove(false)
	, m_eNext(STAGE::NONE)
	, m_vTargetPos()
{
	SetName(L"CCamTrigger");

	AddEnumItemName(L"NONE");
	AddEnumItemName(L"S00");
	AddEnumItemName(L"S01");
	AddEnumItemName(L"S02");
	AddEnumItemName(L"S03");
	AddEnumItemName(L"S04");
	AddEnumItemName(L"S05");

	AddScriptParam("NextStage", SCRIPTPARAM_TYPE::ENUM, &m_eNext);
}

CCamTrigger::~CCamTrigger()
{
}


void CCamTrigger::start()
{
	m_pMainCam = CRenderMgr::GetInst()->GetMainCam();
	assert(m_pMainCam);
}

void CCamTrigger::update()
{
	if (m_bCamMove)
	{
		Vec3 Pos = m_pMainCam->Transform()->GetRelativePos();
		if (abs(Pos.x - m_vTargetPos.x) <= 0.0001f && abs(Pos.y - m_vTargetPos.y) <= 0.0001f)
		{
			m_bCamMove = false;
			m_vTargetPos = Vec2{ 0.f, 0.f };
			return;
		} 
 		Pos = Pos.Lerp(Pos, Vec3(m_vTargetPos.x, m_vTargetPos.y, Pos.z), 0.1f);
		m_pMainCam->Transform()->SetRelativePos(Pos);
	}
}

void CCamTrigger::lateupdate()
{
}

void CCamTrigger::SaveToScene(FILE* _pFile)
{
	fwrite(&m_eNext, sizeof(int), 1, _pFile);
}

void CCamTrigger::LoadFromScene(FILE* _pFile)
{
	int idx = 0;
	fread(&idx, sizeof(int), 1, _pFile);    

	m_eNext = (STAGE)idx;
}

void CCamTrigger::CreateNextStage()
{
	map<wstring, CRes*> pPrefabMap = CResMgr::GetInst()->GetResList(RES_TYPE::PREFAB);

	wstring BGTilePref;
	wstring TilePref;
	CGameObject* pPrevTile = nullptr;
	CGameObject* pPrevTileBG = nullptr;

	switch (m_eNext)
	{
	case STAGE::S00:
	{
		m_vTargetPos = Vec2(0.f, 184.f);
	}
	break;
	case STAGE::S01:
	{
		m_vTargetPos = Vec2(0.f, 0.f);

		m_bCamMove = true;

		TilePref = L"prefab\\Map_1.pref";
		BGTilePref = L"prefab\\Map_1_BG.pref";
	}
	break;
	case STAGE::S02:
	{
		m_vTargetPos = Vec2(0.f, -172.f);

		m_bCamMove = true;

		pPrevTile = CSceneMgr::GetInst()->FindObjectByName(L"Map_0");
		pPrevTileBG = CSceneMgr::GetInst()->FindObjectByName(L"Map_0_BG");


		TilePref = L"prefab\\Map_2.pref";
		BGTilePref = L"prefab\\Map_2_BG.pref";
	}
	break;
	case STAGE::S03:
	{
		m_vTargetPos = Vec2(0.f, -332.f);

		m_bCamMove = true;

		pPrevTile = CSceneMgr::GetInst()->FindObjectByName(L"Map_1");
		pPrevTileBG = CSceneMgr::GetInst()->FindObjectByName(L"Map_1_BG");

		TilePref = L"prefab\\Map_3.pref";
		BGTilePref = L"prefab\\Map_3_BG.pref";
	}
	break;
	case STAGE::S04:
	{
		m_vTargetPos = Vec2(248.f, -510.f);

		m_bCamMove = true;

		pPrevTile = CSceneMgr::GetInst()->FindObjectByName(L"Map_2");
		pPrevTileBG = CSceneMgr::GetInst()->FindObjectByName(L"Map_2_BG");

		TilePref = L"prefab\\Map_4.pref";
		BGTilePref = L"prefab\\Map_4_BG.pref";
	}
	break;
	case STAGE::S05:
	{
		m_vTargetPos = Vec2(568.f, -510.f);

		m_bCamMove = true;

		pPrevTile = CSceneMgr::GetInst()->FindObjectByName(L"Map_3");
		pPrevTileBG = CSceneMgr::GetInst()->FindObjectByName(L"Map_3_BG");

		TilePref = L"prefab\\Map_5.pref";
		BGTilePref = L"prefab\\Map_5_BG.pref";
	}
		break;
	}

	if (nullptr != pPrevTile)
	{
		tEventInfo info = {};
		info.eType = EVENT_TYPE::DELETE_OBJ;
		info.lParam = (DWORD_PTR)pPrevTile;
		CEventMgr::GetInst()->AddEvent(info);
	}

	if (nullptr != pPrevTileBG)
	{
		tEventInfo info = {};
		info.eType = EVENT_TYPE::DELETE_OBJ;
		info.lParam = (DWORD_PTR)pPrevTileBG;
		CEventMgr::GetInst()->AddEvent(info);
	}

	CPrefab* pBGPref = (CPrefab*)CResMgr::GetInst()->FindRes<CPrefab>(BGTilePref).Get();
	CPrefab* pTilePref = (CPrefab*)CResMgr::GetInst()->FindRes<CPrefab>(TilePref).Get();

	if (nullptr != pBGPref)
	{
		tEventInfo info = {};
		info.eType = EVENT_TYPE::CREATE_OBJ;
		info.lParam = (DWORD_PTR)pBGPref->Instantiate();
		info.wParam = (DWORD_PTR)2;
		CEventMgr::GetInst()->AddEvent(info);
	}

	if (nullptr != pTilePref)
	{
		tEventInfo info = {};
		info.eType = EVENT_TYPE::CREATE_OBJ;
		info.lParam = (DWORD_PTR)pTilePref->Instantiate();
		info.wParam = (DWORD_PTR)3;
		CEventMgr::GetInst()->AddEvent(info);
	}
}

void CCamTrigger::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (L"03_col_body" == _OtherObject->GetName())
	{
		CreateNextStage();
	}
}

void CCamTrigger::OnCollision(CGameObject* _OtherObject)
{
}

void CCamTrigger::OnCollisionExit(CGameObject* _OtherObject)
{
}


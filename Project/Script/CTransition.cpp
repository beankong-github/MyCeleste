#include "pch.h"
#include "CTransition.h"

#include <Engine/CResMgr.h>
#include <Engine/CSceneMgr.h>

CTransition::CTransition()
	: CScript((int)SCRIPT_TYPE::TRANSITION)
	, m_fSpeed(0.08f)
	, m_vColor(0.f, 255.f, 1.f, 256.f)
	, m_iCurIdx(0)
	, m_vTex{}
	, m_bPlay(false)
	, m_bSceneEnd(false)
	, m_iStartFrm(0)
	, m_iEndFrm(0)
{
	SetName(L"CTransition");

	AddScriptParam("Transition Speed", SCRIPTPARAM_TYPE::FLOAT, &m_fSpeed);
	AddScriptParam("Sub Color", SCRIPTPARAM_TYPE::VEC4, &m_vColor);
	AddScriptParam("Transition Index", SCRIPTPARAM_TYPE::INT, &m_iCurIdx);


	map<wstring, CRes*> mTex = CResMgr::GetInst()->GetResList(RES_TYPE::TEXTURE);
	map<wstring, CRes*>::iterator iter = mTex.begin();

	for (; iter != mTex.end(); iter++)
	{
		if (nullptr != strstr(ToString(iter->first).c_str(), "transition"))
		{
			m_vTex.push_back((CTexture*)iter->second);
		}
	}

}

CTransition::~CTransition()
{
}

void CTransition::SceneStart()
{
	m_bPlay = true;
	m_iStartFrm = 12;
	m_iEndFrm = m_vTex.size();

	m_iCurIdx = m_iStartFrm;
}

void CTransition::SceneEnd()
{
	m_bPlay = true;
	m_iStartFrm = 0;
	m_iEndFrm = 19;
	m_bSceneEnd = true;

	m_iCurIdx = m_iStartFrm;
}

void CTransition::Play()
{
	m_bPlay = true;
	m_iStartFrm = 0;
	m_iEndFrm = m_vTex.size();

	m_iCurIdx = m_iStartFrm;
}

void CTransition::Stop()
{
	m_bPlay = false;
	m_fSpeed = 0.1f;
	m_iCurIdx = 0;
	m_iStartFrm = 0;
	m_iEndFrm = 0;

	if (m_bSceneEnd)
	{
		m_iCurIdx = 19;
	}
}

void CTransition::start()
{
	assert(GetOwner()->MeshRender());

	if(!m_bPlay)
		SceneStart();
}

void CTransition::update()
{
	if (m_bPlay)
	{
		m_fSpeed -= DT;
		if (m_fSpeed <= 0.f)
		{
			++m_iCurIdx;

			// Repeat
			if (m_iCurIdx >= m_iEndFrm)
			{
				Stop();
			}

			m_fSpeed = 0.08f;
		}
	}
 }

void CTransition::lateupdate()
{
	if (nullptr != MeshRender())
	{
		MeshRender()->SetColor(m_vColor / 256.f);
		MeshRender()->SetAtlasKey(m_vTex[m_iCurIdx]->GetKey());
	}
}

void CTransition::SaveToScene(FILE* _pFile)
{
}

void CTransition::LoadFromScene(FILE* _pFile)
{
}


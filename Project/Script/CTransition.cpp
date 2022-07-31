#include "pch.h"
#include "CTransition.h"

#include <Engine/CResMgr.h>

CTransition::CTransition()
	: CScript((int)SCRIPT_TYPE::TRANSITION)
	, m_fSpeed(0.1f)
	, m_vColor(0.f, 255.f, 1.f, 256.f)
	, m_iCurIdx(0)
	, m_vTex{}
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

void CTransition::start()
{
	assert(GetOwner()->MeshRender());
}

void CTransition::update()
{
	//m_fSpeed -= DT;
	//if (m_fSpeed <= 0.f)
	//{
	//	++m_iCurIdx;

	//	// Repeat
	//	if (m_iCurIdx >= m_vTex.size())
	//	{
	//		m_iCurIdx = 0;
	//	}
	//	m_fSpeed = 0.1f;
	//}
}

void CTransition::lateupdate()
{
	if (nullptr != MeshRender())
		MeshRender()->SetColor(m_vColor / 256.f);

	m_vTex[m_iCurIdx]->UpdateData((UINT)PIPELINE_STAGE::ALL, 0);
}

void CTransition::SaveToScene(FILE* _pFile)
{
}

void CTransition::LoadFromScene(FILE* _pFile)
{
}


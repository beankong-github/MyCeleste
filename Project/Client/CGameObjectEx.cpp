#include "pch.h"
#include "CGameObjectEx.h"

#include <Engine\CComponent.h>

CGameObjectEx::CGameObjectEx()
{
}

CGameObjectEx::~CGameObjectEx()
{
}

void CGameObjectEx::finalupdate()
{
	// 기존 GO와 차이점: Layer 등록 없이 final update

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		CComponent* pCom = GetComponent((COMPONENT_TYPE)i);
		if (nullptr != pCom)
			pCom->finalupdate();
	}

	const vector<CGameObject*>& vecChild = GetChild();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		((CGameObjectEx*)vecChild[i])->finalupdate();
	}
}

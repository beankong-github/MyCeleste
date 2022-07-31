#include "pch.h"
#include "CScriptMgr.h"

#include "CBadlineScript.h"
#include "CBangScript.h"
#include "CCameraMoveScript.h"
#include "CCamTrigger.h"
#include "CCollider2DScript.h"
#include "CDreamBlock.h"
#include "CPhysics.h"
#include "CPhysics.h"
#include "CPlayerScript.h"
#include "CTransition.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CBadlineScript");
	_vec.push_back(L"CBangScript");
	_vec.push_back(L"CCameraMoveScript");
	_vec.push_back(L"CCamTrigger");
	_vec.push_back(L"CCollider2DScript");
	_vec.push_back(L"CDreamBlock");
	_vec.push_back(L"CPhysics");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CTransition");
}

CScript * CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CBadlineScript" == _strScriptName)
		return new CBadlineScript;
	if (L"CBangScript" == _strScriptName)
		return new CBangScript;
	if (L"CCameraMoveScript" == _strScriptName)
		return new CCameraMoveScript;
	if (L"CCamTrigger" == _strScriptName)
		return new CCamTrigger;
	if (L"CCollider2DScript" == _strScriptName)
		return new CCollider2DScript;
	if (L"CDreamBlock" == _strScriptName)
		return new CDreamBlock;
	if (L"CPhysics" == _strScriptName)
		return new CPhysics;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CTransition" == _strScriptName)
		return new CTransition;
	return nullptr;
}

CScript * CScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::BADLINESCRIPT:
		return new CBadlineScript;
		break;
	case (UINT)SCRIPT_TYPE::BANGSCRIPT:
		return new CBangScript;
		break;
	case (UINT)SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return new CCameraMoveScript;
		break;
	case (UINT)SCRIPT_TYPE::CAMTRIGGER:
		return new CCamTrigger;
		break;
	case (UINT)SCRIPT_TYPE::COLLIDER2DSCRIPT:
		return new CCollider2DScript;
		break;
	case (UINT)SCRIPT_TYPE::DREAMBLOCK:
		return new CDreamBlock;
		break;
	case (UINT)SCRIPT_TYPE::PHYSICS:
		return new CPhysics;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::TRANSITION:
		return new CTransition;
		break;
	}
	return nullptr;
}

const wchar_t * CScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::BADLINESCRIPT:
		return L"CBadlineScript";
		break;

	case SCRIPT_TYPE::BANGSCRIPT:
		return L"CBangScript";
		break;

	case SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return L"CCameraMoveScript";
		break;

	case SCRIPT_TYPE::CAMTRIGGER:
		return L"CCamTrigger";
		break;

	case SCRIPT_TYPE::COLLIDER2DSCRIPT:
		return L"CCollider2DScript";
		break;

	case SCRIPT_TYPE::DREAMBLOCK:
		return L"CDreamBlock";
		break;

	case SCRIPT_TYPE::PHYSICS:
		return L"CPhysics";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
		break;

	case SCRIPT_TYPE::TRANSITION:
		return L"CTransition";
		break;

	}
	return nullptr;
}
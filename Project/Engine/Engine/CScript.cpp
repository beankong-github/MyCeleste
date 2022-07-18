#include "pch.h"
#include "CScript.h"


CScript::CScript(int _iScriptType)
	: CComponent(COMPONENT_TYPE::SCRIPT)
	, m_iScriptID(_iScriptType)
	, m_vecParamInfo{}
{
}

CScript::~CScript()
{
}

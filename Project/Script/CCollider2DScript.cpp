#include "pch.h"
#include "CCollider2DScript.h"


CCollider2DScript::CCollider2DScript()
	: CScript((int)SCRIPT_TYPE::COLLIDER2DSCRIPT)
	, m_pCollider(nullptr)
	, m_eProperty(COLLIDER2D_PROPERTY::NONE)
{
	SetName(L"CCollider2DScript");

	AddEnumItemName(L"NONE");
	AddEnumItemName(L"PLAYER");
	AddEnumItemName(L"GROUND");
	AddEnumItemName(L"WALL");
	AddEnumItemName(L"SPIKE");
	AddEnumItemName(L"DREAM_BLOCK");
	AddEnumItemName(L"CAM_AREA");
	AddEnumItemName(L"TELEPORT");
	AddEnumItemName(L"OBJ");

	AddScriptParam("Collider Property", SCRIPTPARAM_TYPE::ENUM, &m_eProperty);
}

CCollider2DScript::~CCollider2DScript()
{
}


void CCollider2DScript::start()
{
	m_pCollider =  GetOwner()->Collider2D();
	assert(m_pCollider);
}

void CCollider2DScript::update()
{
}

void CCollider2DScript::lateupdate()
{
}

void CCollider2DScript::SaveToScene(FILE* _pFile)
{
	fwrite(&m_eProperty, sizeof(UINT), 1, _pFile);
}

void CCollider2DScript::LoadFromScene(FILE* _pFile)
{
	fread(&m_eProperty, sizeof(UINT), 1, _pFile);
}

void CCollider2DScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (nullptr != GetOwner()->GetParent())
	{
		const vector<CScript*>& vecScript = GetOwner()->GetParent()->GetScripts();
		for (size_t i = 0; i < vecScript.size(); ++i)
		{
			vecScript[i]->OnChildCollisionEnter(_OtherObject, GetOwner()->GetName());
		}
	}
}

void CCollider2DScript::OnCollision(CGameObject* _OtherObject)
{
	if (nullptr != GetOwner()->GetParent())
	{
		const vector<CScript*>& vecScript = GetOwner()->GetParent()->GetScripts();
		for (size_t i = 0; i < vecScript.size(); ++i)
		{
			vecScript[i]->OnChildCollision(_OtherObject, GetOwner()->GetName());
		}
	}
}

void CCollider2DScript::OnCollisionExit(CGameObject* _OtherObject)
{
	if (nullptr != GetOwner()->GetParent())
	{
		const vector<CScript*>& vecScript = GetOwner()->GetParent()->GetScripts();
		for (size_t i = 0; i < vecScript.size(); ++i)
		{
			vecScript[i]->OnChildCollisionExit(_OtherObject, GetOwner()->GetName());
		}
	}
}

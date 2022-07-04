#include "pch.h"
#include "CToolObjMgr.h"

#include "CGameObjectEx.h"
#include "CCameraEx.h"
#include "CEditorCamScript.h"

#include <Engine\CRenderMgr.h>
#include <Engine\CTransform.h>


CToolObjMgr::CToolObjMgr()
{

}

CToolObjMgr::~CToolObjMgr()
{
	Safe_Del_Vec(m_vecObj);
}

void CToolObjMgr::init()
{
	// Editor Camera 생성 & 등록
	CGameObjectEx* pToolObj = new CGameObjectEx;
	pToolObj->SetName(L"EditorCamera");
	
	pToolObj->AddComponent(new CTransform);
	pToolObj->AddComponent(new CCameraEx);
	pToolObj->AddComponent(new CEditorCamScript);

	pToolObj->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pToolObj->Camera()->SetFar(10000);
	pToolObj->Camera()->CheckLayerMaskAll();

	m_vecObj.push_back(pToolObj);

	CRenderMgr::GetInst()->RegisterEditorCamera(pToolObj->Camera());
}

void CToolObjMgr::progress()
{
	// 현재 씬 상태가 Play라면 ToolObject들은 작동하지 않는다.
	if (CSceneMgr::GetInst()->GetCurScene()->GetSceneState() == SCENE_STATE::PLAY)
		return;

	for (size_t i = 0; i < m_vecObj.size(); ++i)\
	{
		m_vecObj[i]->update();
	}

	for (size_t i = 0; i < m_vecObj.size(); ++i)\
	{
		m_vecObj[i]->lateupdate();
	}

	for (size_t i = 0; i < m_vecObj.size(); ++i)\
	{
		m_vecObj[i]->finalupdate();
	}

	// render할 필요가 없는 Object들은 render()하지 않는다.
	for (size_t i = 0; i < m_vecObj.size(); ++i)\
	{
		if (nullptr == m_vecObj[i]->GetRenderComponent()
			|| nullptr == m_vecObj[i]->GetRenderComponent()->GetMesh()
			|| nullptr == m_vecObj[i]->GetRenderComponent()->GetMaterial()
			|| nullptr == m_vecObj[i]->GetRenderComponent()->GetMaterial()->GetShader())
			continue;

		m_vecObj[i]->render();
	}
}

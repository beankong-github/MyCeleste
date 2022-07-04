#include "pch.h"
#include "CLight2D.h"

#include "CTransform.h"
#include "CRenderMgr.h"

CLight2D::CLight2D()
	: CComponent(COMPONENT_TYPE::LIGHT2D)
	, m_iLightIdx(-1)
	, m_LightInfo{}
{
}

CLight2D::~CLight2D() 
{
}

void CLight2D::update()
{
}

void CLight2D::finalupdate()
{
	// 광원의 World Pisition을 구해서 구조체에 전달한다
	m_LightInfo.vWorldPos = Transform()->GetWorldPos();
	m_LightInfo.vLightDir = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	

	// 광원을 등록한다
	m_iLightIdx = CRenderMgr::GetInst()->RegisterLight2D(this);
}

void CLight2D::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);

	fwrite(&m_LightInfo, sizeof(tLightInfo), 1, _pFile);

}

void CLight2D::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);

	fread(&m_LightInfo, sizeof(tLightInfo), 1, _pFile);
}

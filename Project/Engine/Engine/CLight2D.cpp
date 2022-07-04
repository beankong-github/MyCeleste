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
	// ������ World Pisition�� ���ؼ� ����ü�� �����Ѵ�
	m_LightInfo.vWorldPos = Transform()->GetWorldPos();
	m_LightInfo.vLightDir = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	

	// ������ ����Ѵ�
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

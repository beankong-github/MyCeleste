#include "pch.h"
#include "CMeshRender.h"

#include "CMesh.h"
#include "CMaterial.h"

#include "CTransform.h"
#include "CAnimator2D.h"

#include "CDevice.h"
#include "CConstBuffer.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER)
	, m_tTexInfo(nullptr)
{
}

CMeshRender::~CMeshRender()
{

}


void CMeshRender::finalupdate()
{
}

void CMeshRender::render()
{
	if (nullptr == GetMesh() || nullptr == GetMaterial())
		return;
	
	static CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::TEXTUREINFO);
	tSprite tInfo;

	if (Animator2D())
	{
		tInfo.vPos = Vec2{ 0.f, 0.f };
		tInfo.vSize = Vec2{ 1.f, 1.f };

		Animator2D()->UpdateData();
		Transform()->UpdateData();
		
		pCB->SetData(&tInfo, sizeof(tSprite));
		pCB->UpdateData();
		GetMaterial()->UpdateData();
		GetMesh()->render();

		CAnimator2D::Clear();
	}


	if (!Animator2D())
	{
		CTexture* pAltas = (CTexture*)CResMgr::GetInst()->FindRes<CTexture>(m_strAtlasKey).Get();
		tSprite* _info = CResMgr::GetInst()->FindSpriteOffsetInfo(m_strAtlasKey, m_strSpriteKey);

		if (nullptr != pCB && nullptr != pAltas)
		{

			if (nullptr == _info)
			{
				tInfo.vPos = Vec2{ 0.f, 0.f };
				tInfo.vSize = Vec2{ 1.f, 1.f };

				m_tTexInfo = &tInfo;
			}
			else
			{
				m_tTexInfo = _info;
				tInfo = *_info;

				// LT
				tInfo.vPos.x /= pAltas->Width();
				tInfo.vPos.y /= pAltas->Height();

				// Size
				tInfo.vSize.x /= pAltas->Width();
				tInfo.vSize.y /= pAltas->Height();
			}

			Transform()->UpdateData();
			
			pCB->SetData(&tInfo, sizeof(tSprite));
			pCB->UpdateData();
			GetMaterial()->SetTexParam(TEX_PARAM::TEX_0, pAltas);
			GetMaterial()->UpdateData();
			GetMesh()->render();
		}
	}

}

void CMeshRender::SaveToScene(FILE* _pFile)
{
	// Component 이름, Mesh, Material 저장
	CRenderComponent::SaveToScene(_pFile);
	
	// Atlas Key 저장
	SaveWStringToFile(m_strAtlasKey, _pFile);
	// Sprite Key 저장
	SaveWStringToFile(m_strSpriteKey, _pFile);
}

void CMeshRender::LoadFromScene(FILE* _pFile)
{
	// Component 이름, Mesh, Material 로드
	CRenderComponent::LoadFromScene(_pFile);

	// Atlas Key 로드
	LoadWStringFromFile(m_strAtlasKey, _pFile);
	// Sprite Key	로드
	LoadWStringFromFile(m_strSpriteKey, _pFile);
}



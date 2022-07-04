#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"

#include "CCamera.h"
#include "CResMgr.h"
#include "CLight2D.h"

#include "CStructuredBuffer.h"

CRenderMgr::CRenderMgr()
	: m_pEditorCam(nullptr)
	, m_pLight2DBuffer(nullptr)
	, m_vecCam{}
	, m_vecLight2D{}
{
	// Light2D 전달용 구조화 버퍼 초기화
	m_pLight2DBuffer = new CStructuredBuffer;
	m_pLight2DBuffer->Create(sizeof(tLightInfo), 2, SB_TYPE::READ_ONLY, true, nullptr);
}

CRenderMgr::~CRenderMgr()
{
	SAFE_DELETE(m_pLight2DBuffer);
}

void CRenderMgr::update()
{

}

void CRenderMgr::render()
{
	// Render Target Clear
	CDevice::GetInst()->SetRenderTarget();
	CDevice::GetInst()->ClearTarget();

	// Light Update
	UpdateLight2D();

	// Global 상수 업데이트
	static CConstBuffer* pGlobalCB = CDevice::GetInst()->GetCB(CB_TYPE::GLOBAL);
	pGlobalCB->SetData(&g_global, sizeof(tGlobal));
	pGlobalCB->UpdateData();
	pGlobalCB->UpdateData_CS();

	// 카메라가 없으면 렌더하지 않는다.
	if (m_vecCam.empty())
		return;

	// 메인 카메라 시점으로 렌더링
	CCamera* pMainCam = m_vecCam[0];

	// Camera 가 찍는 Layer 의 오브젝트들을 Shader Domain 에 따라 분류해둠
	pMainCam->SortGameObject();

	g_transform.matView = pMainCam->GetViewMat();
	g_transform.matProj = pMainCam->GetProjMat();

	// Foward 물체 렌더링
	pMainCam->render_forward();

	// Masked 물체 렌더링
	pMainCam->render_masked();

	// Alpha 물체 렌더링
	pMainCam->render_translucent();

	// PostProcess 물체 렌더링
	pMainCam->render_postprocess();


	//// Sub 카메라 시점으로 렌더링
	//for (int i = 1; i < m_vecCam.size(); ++i)
	//{
	//	if (nullptr == m_vecCam[i])
	//		continue;

	//	m_vecCam[i]->SortGameObject();

	//	g_transform.matView = m_vecCam[i]->GetViewMat();
	//	g_transform.matProj = m_vecCam[i]->GetProjMat();

	//	// Foward 물체 렌더링
	//	m_vecCam[i]->render_forward();

	//	// Masked 물체 렌더링
	//	m_vecCam[i]->render_masked();

	//	// Alpha 물체 렌더링
	//	m_vecCam[i]->render_translucent();
	//}	
}

void CRenderMgr::RegisterCamera(CCamera* _pCam)
{
	// 카메라가 RenderMgr에 최초 등록 되는 경우
	if (-1 == _pCam->m_iCamIdx)
	{
		m_vecCam.push_back(_pCam);
		int iIdx = (int)m_vecCam.size() - 1;
		_pCam->m_iCamIdx = iIdx;
	}	
	else
	{
		if (m_vecCam.size() <= _pCam->m_iCamIdx)
		{
			m_vecCam.resize((size_t)_pCam->m_iCamIdx + 1);
		}

		m_vecCam[_pCam->m_iCamIdx] = _pCam;
	}
}

void CRenderMgr::SwapCameraIndex(CCamera* _pCam, int _iChangeIdx)
{
	for (size_t i = 0; i < m_vecCam.size(); ++i)
	{
		if (_pCam == m_vecCam[i])
		{
			if (nullptr != m_vecCam[_iChangeIdx])
			{
				m_vecCam[_iChangeIdx]->m_iCamIdx = (int)i;
				_pCam->m_iCamIdx = _iChangeIdx;

				return;
			}
		}
	}

	assert(nullptr);
}

void CRenderMgr::CopyTargetToPostProcess()
{
	Ptr<CTexture> pRenderTarget = CResMgr::GetInst()->FindRes<CTexture>(L"RenderTargetTex");
	Ptr<CTexture> pPostProcess = CResMgr::GetInst()->FindRes<CTexture>(L"PostProcessTex");

	CONTEXT->CopyResource(pPostProcess->GetTex2D().Get(), pRenderTarget->GetTex2D().Get());
}

void CRenderMgr::UpdateLight2D()
{
	// 버퍼의 크기보다 라이트가 많다면 버퍼 크기를 재조정하여 생성한다.
	if (m_pLight2DBuffer->GetElementCount() < m_vecLight2D.size())
	{
		m_pLight2DBuffer->Create(sizeof(tLightInfo), m_vecLight2D.size(), SB_TYPE::READ_ONLY, true, nullptr);
	}

	// 라이트2D 포인터 벡터로부터 Light2DInfo를 받아올 벡터를 준비한다.
	static vector<tLightInfo> vecLight2DInfo;
	vecLight2DInfo.clear();

	// Light2DInfo를 얻어온다
	for (size_t i = 0; i < m_vecLight2D.size(); ++i)
	{
		vecLight2DInfo.push_back(m_vecLight2D[i]->GetLightInfo());
	}
	
	// 구조화 버퍼에 라이트 정보를 실어 쉐이더에 전달해준다
	m_pLight2DBuffer->SetData(vecLight2DInfo.data(), (UINT)vecLight2DInfo.size());
	m_pLight2DBuffer->UpdateData(PIPELINE_STAGE::PS, 60);

	// 글로벌 상수 버퍼에 라이트의 개수를 전달해준다
	g_global.Light2DCount = m_vecLight2D.size();

	// Light2DInfo버퍼 클리어
	m_vecLight2D.clear();
}

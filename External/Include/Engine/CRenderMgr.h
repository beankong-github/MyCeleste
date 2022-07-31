#pragma once
#include "singleton.h"
#include "CCamera.h"

class CLight2D;

class CRenderMgr :
    public CSingleton<CRenderMgr>
{
    SINGLE(CRenderMgr);

private:
    vector<CCamera*>    m_vecCam;       // Scene 에 있는 Camera 들
    CCamera*            m_pEditorCam;   // Editor 시점 카메라

    // 쉐이더가 빛 계산을 할 수 있도록 모든 라이트를 구조화 버퍼에 담아 전달해준다
    vector<CLight2D*>   m_vecLight2D;
    CStructuredBuffer*  m_pLight2DBuffer;   // 2D 광원 버퍼

public:
    void init();
    void update();
    void render();

private:
    void render_play();         // Main Camera 시점으로 Render
    void render_editor();       // Editor Camera 시점으로 Render

public:
    CGameObject* GetMainCam() { return m_vecCam[0]->GetOwner(); }
    void RegisterCamera(CCamera* _pCam);
    void RegisterEditorCamera(CCamera* _pCam) { m_pEditorCam = _pCam; }
    void ClearCamera() { m_vecCam.clear(); }
    void CopyTargetToPostProcess();

    int RegisterLight2D(CLight2D* _pLight)
    {
        m_vecLight2D.push_back(_pLight);
        return (int)m_vecLight2D.size() - 1;
    }

private:
    void SwapCameraIndex(CCamera* _pCam, int _iChangeIdx);
    void UpdateLight2D();


    friend class CEventMgr;
};


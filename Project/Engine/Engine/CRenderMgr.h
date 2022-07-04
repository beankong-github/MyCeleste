#pragma once
#include "singleton.h"

class CCamera;
class CLight2D;

class CRenderMgr :
    public CSingleton<CRenderMgr>
{
    SINGLE(CRenderMgr);

private:
    vector<CCamera*>    m_vecCam;       // Scene �� �ִ� Camera ��
    CCamera*            m_pEditorCam;   // Editor ���� ī�޶�

    // ���̴��� �� ����� �� �� �ֵ��� ��� ����Ʈ�� ����ȭ ���ۿ� ��� �������ش�
    vector<CLight2D*>   m_vecLight2D;
    CStructuredBuffer*  m_pLight2DBuffer;

public:
    void init();
    void update();
    void render();

public:
    void RegisterCamera(CCamera* _pCam);
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

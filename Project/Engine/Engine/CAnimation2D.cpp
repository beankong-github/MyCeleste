#include "pch.h"
#include "CAnimation2D.h"

#include "CAnimator2D.h"
#include "CTimeMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"


CAnimation2D::CAnimation2D() :
	m_iCurFrmIdx(0),
	m_iIsDummy(0),
	m_fAccTime(0.f),
	m_pOwner(nullptr),
	m_bFinish(false),
	m_bRepeat(false),
	m_bReset(false),
	m_pAtlasTex(nullptr)
{
}


CAnimation2D::~CAnimation2D()
{
}

void CAnimation2D::finalupdate()
{
	if (m_iIsDummy) return;

	m_fAccTime += DT;

	if (m_vecFrm[m_iCurFrmIdx].fDuration < m_fAccTime)
	{
		m_fAccTime -= m_vecFrm[m_iCurFrmIdx].fDuration;

		if (m_vecFrm.size() - 1 <= m_iCurFrmIdx)
		{
			if (!m_bRepeat)
			{
				m_bFinish = true;
				m_bReset = true;
			}
			else
			{
				m_bReset = true;
			}
		}
		else if (m_vecFrm.size() - 1 > m_iCurFrmIdx)
		{
			++m_iCurFrmIdx;
		}
	}
}

void CAnimation2D::UpdateData()
{
	static CConstBuffer* pBuffer = CDevice::GetInst()->GetCB(CB_TYPE::ANIM2D);

	tAnim2D info = {};
	info.useAnim2D = 1;
	info.useDummyAnim = m_iIsDummy;

	info.vBackgroundSize = m_vBackgroundSize;
	info.vLT = m_vecFrm[m_iCurFrmIdx].vLT;
	info.vSlice = m_vecFrm[m_iCurFrmIdx].vSlice;
	info.vOffset = m_vecFrm[m_iCurFrmIdx].vOffset;

	pBuffer->SetData(&info, sizeof(tAnim2D));
	pBuffer->UpdateData();

	//���� ����� Animation�� ����� �Ǵ� Texture ������
	//register(t10) �� ���ε��� �س��´�.
	//�׷��� �ٸ� �ؽ����� �̹����� �����ص� �� ��� �ƴ���.
	if (!m_iIsDummy)
	{
		info.Atlas_Width = m_pAtlasTex->Width();
		info.Atlas_Height = m_pAtlasTex->Height();
		m_pAtlasTex->UpdateData((UINT)PIPELINE_STAGE::PS, 10);
	}
}

void CAnimation2D::Create(Ptr<CTexture> _Atlas, Vec2 _vBackgroundSizePixel, Vec2 _vLT, Vec2 _vSlice, Vec2 _vStep, float _fDuration, int _iFrameCount, bool _bRepeat)
{
	assert(_Atlas.Get());

	m_pAtlasTex = _Atlas;

	float fWidth = m_pAtlasTex->Width();
	float fHeight = m_pAtlasTex->Height();

	// �ȼ� ��ǥ�� 0 ~ 1 UV �� ��ȯ
	Vec2 vLT = _vLT / Vec2(fWidth, fHeight);       // 0, 260    UV(0, 0)
	Vec2 vStep = _vStep / Vec2(fWidth, fHeight);   // 60, 0
	Vec2 vSlice = _vSlice / Vec2(fWidth, fHeight); //(60, 325) , (120, 325) , ...  UV(1, 1)
	//Vec2 vBackground = _vBackgroundSizePixel / Vec2(fWidth, fHeight);

	m_vBackgroundSize = _vBackgroundSizePixel / Vec2(fWidth, fHeight);
	m_bRepeat = _bRepeat;

	for (int i = 0; i < _iFrameCount; ++i)
	{
		tAnim2DFrame frm = {};

		//vOffset ��� ����
		//if (i == 5)
		//{
		//	frm.vOffset = Vec2(10.f, 0.f) / Vec2(fWidth, fHeight);
		//}

		//UV ��ȯ
		frm.vLT = vLT + (vStep * (float)i); //�ڸ����� �̹����� UV 0, 0
		frm.vSlice = vSlice;		        //�ڸ����� �̹����� UV 1, 1
		frm.fDuration = _fDuration;

		m_vecFrm.push_back(frm);
	}
}

void CAnimation2D::CreateDummy(int _isDummy)
{
	m_iIsDummy = _isDummy;

	tAnim2DFrame frm = {};
	m_vecFrm.push_back(frm);
}


void CAnimation2D::SaveToScene(FILE* _pFile)
{
	CEntity::SaveToScene(_pFile);

	size_t i = m_vecFrm.size();
	fwrite(&i, sizeof(size_t), 1, _pFile);
	fwrite(m_vecFrm.data(), sizeof(tAnim2DFrame), i, _pFile);
	fwrite(&m_vBackgroundSize, sizeof(Vec2), 1, _pFile);
	fwrite(&m_bRepeat, sizeof(bool), 1, _pFile);

	//���� � ���ҽ��� �����ϰ� �־����� ��θ� �������ֱ�
	SaveResPtr(m_pAtlasTex, _pFile);
}

void CAnimation2D::LoadFromScene(FILE* _pFile)
{
	CEntity::LoadFromScene(_pFile);

	size_t i = 0;
	fread(&i, sizeof(size_t), 1, _pFile);                    //�ִϸ��̼� �� ������ ���ϰ�
	m_vecFrm.resize(i);                                      //���͸� �� ������ŭ �ø���
	fread(m_vecFrm.data(), sizeof(tAnim2DFrame), i, _pFile); //��� �ִϸ��̼��� ��������
	fread(&m_vBackgroundSize, sizeof(Vec2), 1, _pFile);
	fread(&m_bRepeat, sizeof(bool), 1, _pFile);

	LoadResPtr(m_pAtlasTex, _pFile);
}

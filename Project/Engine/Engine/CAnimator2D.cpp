#include "pch.h"
#include "CAnimator2D.h"

#include "CAnimation2D.h"
#include "CDevice.h"
#include "CConstBuffer.h"

//Animator2D�� ����� Animation2D ��ü����
//map�� �־�ΰ�
//���� ����� CAnimation2D�� �����Ͽ� ��������ִ� ����.

//�ݺ������ ���� ������ ���⼭ �����ϴµ�
//�ִϸ��̼� ��ü���� �ݺ� ������ ������ �ֵ��� �ϴ°� ���� ������
//�ݺ��� �ǵ� �ȵǵ� Reset() �ϵ��� �Ѵ�

CAnimator2D::CAnimator2D() :
	CComponent(COMPONENT_TYPE::ANIMATOR2D),
	m_pCurAnim(nullptr),
	m_playAnimKey{}
	//m_bRepeat(false)
{
	CreateDummyAnim(L"Dummy", 1);
}

CAnimator2D::CAnimator2D(const CAnimator2D& _origin) :
	CComponent(_origin),
	m_pCurAnim(nullptr)
	//m_bRepeat(_origin.m_bRepeat)
{
	for (auto& pair : _origin.m_mapAnim)
	{
		CAnimation2D* pCloneAnim = pair.second->Clone();
		m_mapAnim.insert(make_pair(pair.first, pCloneAnim));
		pCloneAnim->m_pOwner = this;
	}

	if (nullptr != _origin.m_pCurAnim)
	{
		m_pCurAnim = FindAnim(_origin.m_pCurAnim->GetName());
	}
}

CAnimator2D::~CAnimator2D()
{
	Safe_Del_Map(m_mapAnim);
}

void CAnimator2D::finalupdate()
{
	//IsFinish() Ȱ��
	//�׷��� IsReset()�� �־ ���� �ʳ�?
	//������ �����ӿ� �����ߴٴ°� �˷��ִ� �����̶��
	//����� IsReset()�� true�� �Ǵ� ������ ������ ������ �����ߴٴ� ��.
	//IsRepeat()�� false���� �� m_pCurAnim ��ü�ϴºκ� �����ؾ� �Ұ� ����

	if (nullptr == m_pCurAnim) return;

	m_pCurAnim->finalupdate();

	if (m_pCurAnim->IsReset() && m_pCurAnim->IsRepeat())
	{
		m_pCurAnim->Reset();
	}
	else if (m_pCurAnim->IsReset() && !m_pCurAnim->IsRepeat())
	{
		m_pCurAnim->Reset();
		m_pCurAnim = nullptr;
	}
}

void CAnimator2D::UpdateData()
{
	if (nullptr == m_pCurAnim) return;

	m_pCurAnim->UpdateData();
}

void CAnimator2D::Clear()
{
	static CConstBuffer* pBuffer = CDevice::GetInst()->GetCB(CB_TYPE::ANIM2D);
	tAnim2D info = {};
	pBuffer->SetData(&info, sizeof(tAnim2D)); //MapUnMap
	pBuffer->UpdateData();                    //(Shader)SetConstantBuffers
}

CAnimation2D* CAnimator2D::FindAnim(const wstring& _strName)
{
	map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.find(_strName);

	if (iter == m_mapAnim.end())
	{
		return nullptr;
	}

	return iter->second;
}

void CAnimator2D::CreateAnim(const wstring& _strName, Ptr<CTexture> _pAtlas, Vec2 _vBackgroundSizePixel, Vec2 _vLeftTopPixel, Vec2 _vSlicePixel, Vec2 _vStepPixel,      float _fDuration, int _iFrameCount, bool _bRepeat)
{
	//if (nullptr != FindAnim(_strName)) return;
	//Animation �̸� �ߺ� ���� assert
    assert(!FindAnim(_strName));
    
    CAnimation2D* anim = new CAnimation2D;
    anim->SetName(_strName);
    anim->Create(_pAtlas, _vBackgroundSizePixel, _vLeftTopPixel, _vSlicePixel, _vStepPixel, _fDuration, _iFrameCount, _bRepeat);
    
    m_mapAnim.insert(make_pair(_strName, anim));
    anim->m_pOwner = this;
}

void CAnimator2D::CreateDummyAnim(const wstring& _strName, int _iIsDummy)
{
	assert(!FindAnim(_strName));

	CAnimation2D* anim = new CAnimation2D;
	anim->SetName(_strName);
	anim->CreateDummy(_iIsDummy);

	m_mapAnim.insert(make_pair(_strName, anim));
	anim->m_pOwner = this;
}

void CAnimator2D::AddAnim(const string& _strName, CAnimation2D* _pAnim)
{
	wstring _wStrName = wstring(_strName.begin(), _strName.end());

	assert(!FindAnim(_wStrName));

	m_mapAnim.insert(make_pair(_wStrName, _pAnim));

	_pAnim->m_pOwner = this;
}

void CAnimator2D::Play(const wstring& _strName)
{
	//���ڷ� ���� ���ڿ��� ����߽� �ִϸ��̼� �̸� ������ ����
	if (nullptr != m_pCurAnim && m_pCurAnim->GetName() == _strName) return;

	CAnimation2D* pAnim = FindAnim(_strName);

	assert(pAnim);

	if (nullptr == m_pCurAnim) 
	{ 
		m_pCurAnim = pAnim; 
		m_playAnimKey = _strName;
		return; 
	}
	//������ ������̾��� �ִϸ��̼� ����
	else if (nullptr != m_pCurAnim) 
		m_pCurAnim->Reset();

	//�ִϸ��̼� ��ü
	m_pCurAnim = pAnim;
	m_playAnimKey = _strName;
}

void CAnimator2D::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);

	size_t iAnimCount = m_mapAnim.size();
	fwrite(&iAnimCount, sizeof(size_t), 1, _pFile); //������ ��� �ִϸ��̼� ���� ����

	//m_bRepeat ������ �����Ǵ� �ִϸ��̼ǿ��� ������.
	map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.begin();
	for (; iter != m_mapAnim.end(); ++iter)
	{
		iter->second->SaveToScene(_pFile);
	}

	wstring strCurAnimName;
	if (nullptr != m_pCurAnim) strCurAnimName = m_pCurAnim->GetName();

	SaveWStringToFile(strCurAnimName, _pFile);

	//fwrite(&m_bRepeat, sizeof(bool), 1, _pFile);
}

void CAnimator2D::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);

	size_t iAnimCount = 0;
	fread(&iAnimCount, sizeof(size_t), 1, _pFile); //������ ��� �ִϸ��̼� ���� ����

	for (size_t i = 0; i < iAnimCount; ++i)
	{
		CAnimation2D* pAnim = new CAnimation2D;
		pAnim->LoadFromScene(_pFile);

		m_mapAnim.insert(make_pair(pAnim->GetName(), pAnim));
		pAnim->m_pOwner = this;
	}

	wstring strCurAnimName;
	LoadWStringFromFile(strCurAnimName, _pFile);

	m_pCurAnim = FindAnim(strCurAnimName);

	//fwrite(&m_bRepeat, sizeof(bool), 1, _pFile);
}
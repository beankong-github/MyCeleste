#include "pch.h"
#include "CAnimator2D.h"

#include "CAnimation2D.h"
#include "CDevice.h"
#include "CConstBuffer.h"

//Animator2D는 사용할 Animation2D 객체들을
//map에 넣어두고
//현재 실행될 CAnimation2D만 참조하여 실행시켜주는 구조.

//반복재생을 하지 말지를 여기서 결정하는데
//애니메이션 자체에서 반복 정보를 가지고 있도록 하는게 좋지 않을까
//반복이 되든 안되든 Reset() 하도록 한다

CAnimator2D::CAnimator2D() :
	CComponent(COMPONENT_TYPE::ANIMATOR2D),
	m_pCurAnim(nullptr),
	m_playAnimKey{},
	m_pDefaultAnim(nullptr)
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

	if (nullptr != _origin.m_pDefaultAnim)
	{
		m_pDefaultAnim = FindAnim(_origin.m_pDefaultAnim->GetName());
	}

}

CAnimator2D::~CAnimator2D()
{
	Safe_Del_Map(m_mapAnim);
}

void CAnimator2D::finalupdate()
{
	//IsFinish() 활용
	//그런데 IsReset()만 있어도 되지 않나?
	//마지막 프레임에 도달했다는걸 알려주는 목적이라면
	//현재는 IsReset()이 true가 되는 시점이 마지막 프레임 도달했다는 뜻.
	//IsRepeat()가 false였을 때 m_pCurAnim 교체하는부분 개선해야 할것 같다

	if (nullptr == m_pCurAnim) return;

	m_pCurAnim->finalupdate();

	if (m_pCurAnim->IsReset() && m_pCurAnim->IsRepeat())
	{
		m_pCurAnim->Reset();
	}
	else if (m_pCurAnim->IsReset() && !m_pCurAnim->IsRepeat())
	{
		m_pCurAnim->Reset();
		m_pCurAnim = m_pDefaultAnim;
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
	// 이름이 겹치는 애니메이션이 있으면 기존의 애니메이션을 삭제하고 새로 저장한다
	const map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.find(_strName);
	if (iter != m_mapAnim.end())
	{
		delete(iter->second);
		m_mapAnim.erase(iter);
	}
    
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

	auto result = m_mapAnim.insert(make_pair(_strName, anim));
	assert(result.first != m_mapAnim.end());
	anim->m_pOwner = this;
}

void CAnimator2D::AddAnim(const string& _strName, CAnimation2D* _pAnim)
{
	wstring _wStrName = wstring(_strName.begin(), _strName.end());
	
	// 이름이 겹치는 애니메이션이 있으면 기존의 애니메이션을 삭제하고 새로 저장한다
	const map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.find(ToWString(_strName));
	if (iter != m_mapAnim.end())
	{
		delete(iter->second);
		m_mapAnim.erase(iter);
	}

	m_mapAnim.insert(make_pair(_wStrName, _pAnim));

	_pAnim->m_pOwner = this;
}

void CAnimator2D::SetDefaultAnim(const wstring& _strName)
{
	CAnimation2D* pAnim = FindAnim(_strName);

	assert(pAnim);

	m_pDefaultAnim = pAnim;
}

void CAnimator2D::Play(const wstring& _strName)
{
	//인자로 들어온 문자열과 재생중신 애니메이션 이름 같으면 리턴
	if (m_pCurAnim != nullptr && m_pCurAnim->GetName() == _strName) return;

	CAnimation2D* pAnim = FindAnim(_strName);

	assert(pAnim);
	//기존에 재생중이었던 애니메이션 리셋
	if (nullptr != m_pCurAnim) 
		m_pCurAnim->Reset();

	//애니메이션 교체
	m_pCurAnim = pAnim;
	m_playAnimKey = _strName;
}

void CAnimator2D::PlayDefaultAnim()
{
	// 기본 애니메이션이 없으면 assert
	assert(m_pDefaultAnim == nullptr);

	//기존에 재생중이었던 애니메이션 리셋
	if (nullptr != m_pCurAnim)
		m_pCurAnim->Reset();

	//애니메이션 교체
	m_pCurAnim = m_pDefaultAnim;
	m_playAnimKey = m_pDefaultAnim->GetName();
}

void CAnimator2D::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);

	size_t iAnimCount = m_mapAnim.size();
	fwrite(&iAnimCount, sizeof(size_t), 1, _pFile); //보유한 모든 애니메이션 개수 저장

	//m_bRepeat 변수는 생성되는 애니메이션에서 결정됨.
	map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.begin();
	for (; iter != m_mapAnim.end(); ++iter)
	{
		iter->second->SaveToScene(_pFile);
	}

	wstring strCurAnimName;
	wstring strDefaultName;
	if (nullptr != m_pCurAnim) strCurAnimName = m_pCurAnim->GetName();
	if (nullptr != m_pDefaultAnim) strDefaultName = m_pDefaultAnim->GetName();

	SaveWStringToFile(strCurAnimName, _pFile);
	SaveWStringToFile(strDefaultName, _pFile);

	//fwrite(&m_bRepeat, sizeof(bool), 1, _pFile);
}

void CAnimator2D::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);

	size_t iAnimCount = 0;
	fread(&iAnimCount, sizeof(size_t), 1, _pFile); //보유한 모든 애니메이션 개수 저장

	for (size_t i = 0; i < iAnimCount; ++i)
	{
		CAnimation2D* pAnim = new CAnimation2D;
		pAnim->LoadFromScene(_pFile);

		
		auto iter = m_mapAnim.insert(make_pair(pAnim->GetName(), pAnim));
		if (false == iter.second)
		{
			delete pAnim;
			continue;
		}
		pAnim->m_pOwner = this;
	}

	wstring strCurAnimName;
	LoadWStringFromFile(strCurAnimName, _pFile);

	wstring strDefaultName;
	LoadWStringFromFile(strDefaultName, _pFile);

	m_pCurAnim = FindAnim(strCurAnimName);
	m_pDefaultAnim = FindAnim(strDefaultName);

	//fwrite(&m_bRepeat, sizeof(bool), 1, _pFile);
}
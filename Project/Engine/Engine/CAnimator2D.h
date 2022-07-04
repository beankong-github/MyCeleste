#pragma once
#include "CComponent.h"
#include "CTexture.h"

class CAnimation2D;

class CAnimator2D : public CComponent
{
private:
	map<wstring, CAnimation2D*> m_mapAnim;
	CAnimation2D*				m_pCurAnim;
	wstring				        m_playAnimKey; //현재 진행중인 애니메이션 키 저장
	//bool					    m_bRepeat;


public:
	void CreateAnim(const wstring& _strName, Ptr<CTexture> _pAtlas, Vec2 _vBackgroundSizePixel, Vec2 _vLeftTopPixel, Vec2 _vSlicePixel, Vec2 _vStepPixel,
		            float _fDuration, int _iFrameCount, bool _bRepeat);

	void CreateDummyAnim(const wstring& _strName, int _iIsDummy = 0);

	CAnimation2D* GetCurAnim() { if (nullptr != m_pCurAnim) { return m_pCurAnim; } }
	CAnimation2D* FindAnim(const wstring& _strName);

	void Play(const wstring& _strName);

	const map<wstring, CAnimation2D*>& GetAnimList() { return m_mapAnim; }

	const wstring& GetPlayAnimKey() { return m_playAnimKey; }


	//CreateAnimation2DTool에서 생성한 애니메이션 추가해주는 함수
	void AddAnim(const string& _strName, CAnimation2D* _pAnim);

public:
	virtual void finalupdate() override;
	virtual void UpdateData() override;
	static void Clear();

public:

	virtual void SaveToScene(FILE* _pFile) override;
	virtual void LoadFromScene(FILE* _pFile) override;

	CLONE(CAnimator2D)

public:
	CAnimator2D();
	CAnimator2D(const CAnimator2D& _origin);
	~CAnimator2D();

};


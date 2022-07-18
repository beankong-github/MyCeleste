#pragma once
#include "CComponent.h"
#include "CTexture.h"

class CAnimation2D;

class CAnimator2D : public CComponent
{
private:
	map<wstring, CAnimation2D*> m_mapAnim;
	CAnimation2D*				m_pCurAnim;
	CAnimation2D*				m_pDefaultAnim;
	wstring				        m_playAnimKey; //���� �������� �ִϸ��̼� Ű ����
	//bool					    m_bRepeat;


public:
	CAnimation2D* GetDefaultAnim() { return m_pDefaultAnim; }
	CAnimation2D* FindAnim(const wstring& _strName);
	CAnimation2D* GetCurAnim() { if (nullptr != m_pCurAnim) { return m_pCurAnim; } }
	const map<wstring, CAnimation2D*>& GetAnimList() { return m_mapAnim; }
	const wstring& GetPlayAnimKey() { return m_playAnimKey; }


	void SetDefaultAnim(const wstring& _strName);
	void CreateAnim(const wstring& _strName, Ptr<CTexture> _pAtlas, Vec2 _vBackgroundSizePixel, Vec2 _vLeftTopPixel, Vec2 _vSlicePixel, Vec2 _vStepPixel,float _fDuration, int _iFrameCount, bool _bRepeat);
	void CreateDummyAnim(const wstring& _strName, int _iIsDummy = 0);
	//CreateAnimation2DTool���� ������ �ִϸ��̼� �߰����ִ� �Լ�
	void AddAnim(const string& _strName, CAnimation2D* _pAnim);

	void Play(const wstring& _strName);
	void PlayDefaultAnim();

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


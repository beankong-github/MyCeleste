#pragma once
#include "CEntity.h"
#include "CTexture.h"

//< 04.20 1교시 16:00 ~ > 애니메이션 보강 목록

class CAnimator2D;

class CAnimation2D : public CEntity
{
private:
	vector<tAnim2DFrame> m_vecFrm;
	Ptr<CTexture>        m_pAtlasTex; //참조할 텍스쳐
	Vec2				 m_vBackgroundSize;

	CAnimator2D*		 m_pOwner;
	int					 m_iCurFrmIdx;
	int                  m_iIsDummy;
	float				 m_fAccTime;

	bool				 m_bFinish;
	bool                 m_bReset;
	bool                 m_bRepeat;


public:
	CTexture* GetAtlasTex() { return m_pAtlasTex.Get(); }
	tAnim2DFrame& GetFrame(int _iFrmIdx) { return m_vecFrm[_iFrmIdx]; }
	UINT GetMaxFrame() { return (UINT)m_vecFrm.size(); }

	void SetAnimFrame(Ptr<CTexture> _pAtlas, vector<tAnim2DFrame>& _vecFrm) 
	{ 
		m_pAtlasTex = _pAtlas;
		m_vecFrm = _vecFrm; 
	}

private:
	void finalupdate();

	virtual void UpdateData() override;

public:
	void Create(Ptr<CTexture> _Atlas, Vec2 _vBackgroundSizePixel, Vec2 _vLT, Vec2 _vSlice, Vec2 _vStep, float _fDuration, int _iFrameCount, bool _bRepeat);
	void CreateDummy(int _isDummy);
	void SetFinish(bool _bFinish) { m_bFinish = _bFinish; }
	void SetRepeat(bool _bRepeat) { m_bRepeat = _bRepeat; }
	bool IsFinish() { return m_bFinish; }
	bool IsRepeat() { return m_bRepeat; }
	bool IsReset() { return m_bReset; }
	int GetCurFrmIdx() { return m_iCurFrmIdx; }

	void Reset() 
	{ 
		m_fAccTime = 0.f; 
		m_iCurFrmIdx = 0; 
		m_bFinish = false;
		m_bReset = false;
	}

public:
	virtual void SaveToScene(FILE* _pFile) override;
	virtual void LoadFromScene(FILE* _pFile) override;

	CLONE(CAnimation2D)


public:
	CAnimation2D();
	~CAnimation2D();

	friend class CAnimator2D;
};
#pragma once
#include "UI.h"

typedef void (UI::* AnimListEvent)(DWORD_PTR _param);

class CGameObject;
class CTexture;

struct Preview_Info
{
	ImVec2 vSize;
	ImVec2 vOriginSize;
	ImVec2 vLT;
	ImVec2 vOriginLT;
	ImVec2 vRB;
	ImVec2 vOriginRB;
	ImVec2 vSlice;
	ImVec2 vCheckDummy;
	float  fSetRectPos[2];
	float  fSetRectSize[2];
};


class Create2DAnimationUI : public UI
{
private:
	CGameObject* m_pTargetObj; 
	CTexture*    m_pTex;
	Vec2         m_vTexResolution;

	string       m_animName;
	int          m_iRepeatSaveCount;    //반복저장 회수
	bool         m_bIsRepeat;			//반복 애니메이션 여부 설정


	Vec2         m_vScroll;
	Vec2         m_gridScroll;
	Vec2         m_SampleUV[3];
	Vec2         m_OriginUV[2];
	Vec2         m_ChangeRectSize;
	Vec2         m_vDrawRectInfo[2];
	float        m_fImgZoom;


	//CheckAnimation 관련 멤버
	int    m_iCheckIdx;
	int    m_iImgProduct;
	float  m_fStride;
	bool   m_bCheckWindow;
	bool   m_bIsCheckPlay;
	string m_strCheckButton;
	ImVec2 CheckCanvasSize;
	float  m_fDT;


	//DragFloat 인자
	Vec2 _vCalculValue;
	float _RectLT[2];
	float _RectRB[2];
	float _RectSize[2];
	float _RectDist[2];
	float _previewRectPos[2];
	float _previewRectSize[2];

	//AnimInfo 컨테이너
	vector<tAnim2DFrame> m_vecAnim2DInfo;
	vector<Preview_Info> m_vecPreviewInfo;
	float			     m_fDuration;

	//리스트목록 전달 딜리게이트
	UI*            m_Inst;
	AnimListEvent  m_AddAnimEvent;

public:
	void SetTargetObj(CGameObject* _obj);
	CGameObject* GetTargetobj() { return m_pTargetObj; }


	void SetAddListEvent(UI* _Inst, AnimListEvent _func)
	{
		m_Inst = _Inst;
		m_AddAnimEvent = _func;
	}

private:

	ImVec2 V2ToImV2(Vec2& _vec2);
	void F2ToV2(Vec2& _vec2, float* _f2) { _vec2.x = _f2[0]; _vec2.y = _f2[1]; }
	ImVec2 ImV2XInt(ImVec2 _imvec2, int _i) { _imvec2.x *= _i; _imvec2.y *= _i; return _imvec2; }
	void VecClearFunc();
	void FloatReset();
	void SetCheckDummy(Preview_Info& _preview);

public:
	virtual void update() override;
	virtual void render_update() override;

public:
	Create2DAnimationUI();
	~Create2DAnimationUI();
};
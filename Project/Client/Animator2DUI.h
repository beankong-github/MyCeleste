#pragma once
#include "ComponentUI.h"


class Create2DAnimationUI;

class Animator2DUI : public ComponentUI
{
private:
	Create2DAnimationUI* pCreate2DAnimUI;
	vector<string>	     m_vecAnimList;
	int                  m_iAnimIdx;


public:
	virtual void update() override;
	virtual void render_update() override;
	virtual void Activate() override;
	virtual void Deactivate() override;

public:
	void UpdateAnimList(DWORD_PTR _param = NULL);

public:


	Animator2DUI();
	~Animator2DUI();
};


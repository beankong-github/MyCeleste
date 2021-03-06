#pragma once

#include "UI.h"

typedef void (UI::* PARAM_1)(DWORD_PTR);

struct tUIDelegate
{
	UI*			pInst;
	PARAM_1		pFunc;
	DWORD_PTR	dwParam;
};


class CImGuiMgr
	: public CSingleton<CImGuiMgr>
{
	SINGLE(CImGuiMgr)
private:
	map<string, UI*>	m_mapUI;
	vector<tUIDelegate>	m_vecDelegate;

	HANDLE				m_hNotify;		// content 파일에 변경점 이벤트를 받는다.
private:
	void CreateUI();
	void ObserveContent();

public:
	UI*		FindUI(const string& _strKey);
	void	AddDelegate(tUIDelegate _del) { m_vecDelegate.push_back(_del); }

public:
	void init(HWND _hwnd);

	void progress();
	void render();
	void clear();
};


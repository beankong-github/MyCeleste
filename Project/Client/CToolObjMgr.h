#pragma once

class CGameObjectEx;

class CToolObjMgr :
    public CSingleton<CToolObjMgr>
{
    SINGLE(CToolObjMgr);

private:
    vector<CGameObjectEx*>	m_vecObj;       //  모든 Tool 전용 Object들

public:
    void init();
    void progress();
};


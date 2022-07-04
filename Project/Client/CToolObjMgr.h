#pragma once

class CGameObjectEx;

class CToolObjMgr :
    public CSingleton<CToolObjMgr>
{
    SINGLE(CToolObjMgr);

private:
    vector<CGameObjectEx*>	m_vecObj;       //  ��� Tool ���� Object��

public:
    void init();
    void progress();
};


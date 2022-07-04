#pragma once
#include "CRes.h"

class CGameObject;

class CPrefab :
    public CRes
{
private:
    CGameObject* m_pProtoObj;


// 프리팹 Sava/Load 함수 포인터
public:
    typedef void (*SaveFunc) (CPrefab*, const wstring&);
    typedef int (*LoadFunc) (CPrefab*, const wstring&);
    static SaveFunc        m_pSaveFunc;
    static LoadFunc        m_pLoadFunc;

public:
    CGameObject* Instantiate();
    CGameObject* GetProto() { return m_pProtoObj; }
    void SetProto(CGameObject* _pProto) { m_pProtoObj = _pProto; }

public:
    virtual int Save(const wstring& _strFilePath);
    virtual int Load(const wstring& _strFilePath);

public:
    CPrefab();
    CPrefab(CGameObject* _pProtoObj);
    ~CPrefab();
};


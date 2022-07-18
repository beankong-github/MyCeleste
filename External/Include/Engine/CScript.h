#pragma once

#include "global.h"
#include "CComponent.h"

#include "CTransform.h"
#include "CMeshRender.h"

#include "CResMgr.h"
#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"


enum class SCRIPTPARAM_TYPE
{
    INT,
    FLOAT,
    VEC2,
    VEC4,
    ENUM,

    TEX,
    PREFAB,
};


struct tScriptParamInfo
{
    string              strParamName;
    SCRIPTPARAM_TYPE    eType;
    void*               pParam;
};


class CScript :
    public CComponent
{
private:
    const int                   m_iScriptID;    // 스크립트 구별용도(스크립트 타입)
    vector<tScriptParamInfo>    m_vecParamInfo; // 에디터 노출 변수
    vector<wstring>             m_vecEnumItemNames;

public:
    int GetScriptType() { return m_iScriptID; }
    const vector<tScriptParamInfo>& GetScriptParam() { return m_vecParamInfo; }
    const vector<wstring>& GetEnumItemNames() { return m_vecEnumItemNames; }

protected:
    void AddScriptParam(string _strParamName, SCRIPTPARAM_TYPE _eType, void* _pData) { m_vecParamInfo.push_back(tScriptParamInfo{ _strParamName , _eType, _pData });}
    void AddScriptParam(tScriptParamInfo _info) { m_vecParamInfo.push_back(_info); }
    void AddEnumItemName(wstring _name) { m_vecEnumItemNames.push_back(_name); }

public:
    virtual void start() {}
    virtual void update() {}
    virtual void lateupdate() {}
    virtual void finalupdate() final {}

    //virtual void SaveToScene(FILE* _file) = 0;
    //virtual void LoadToScene(FILE* _file) = 0;

    virtual void OnCollisionEnter(CGameObject* _OtherObject) {};
    virtual void OnCollision(CGameObject* _OtherObject) {};
    virtual void OnCollisionExit(CGameObject* _OtherObject) {};

    virtual void OnChildCollisionEnter(CGameObject* _OtherObject, wstring _childName) {};
    virtual void OnChildCollision(CGameObject* _OtherObject, wstring _childName) {};
    virtual void OnChildCollisionExit(CGameObject* _OtherObject, wstring _childName) {};

    virtual CScript* Clone() = 0;
    
    //temp
    template<typename T>
    void SetParam(string _strParamName, T _pData)
    {
        auto iter = std::find_if(m_vecParamInfo.begin(), m_vecParamInfo.end(), [_strParamName](const tScriptParamInfo& info) {
            return _strParamName == info.strParamName;
            });

        if (iter != m_vecParamInfo.end())
        {
            T* pParam = (T*)(*iter).pParam;
            *pParam = _pData;
        }
    }
public:
    CScript(int _iScriptType);
    ~CScript();
};


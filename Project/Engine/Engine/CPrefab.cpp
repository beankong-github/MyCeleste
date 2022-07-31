#include "pch.h"
#include "CPrefab.h"

#include "CGameObject.h"
#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"

CPrefab::SaveFunc CPrefab::m_pSaveFunc = nullptr;
CPrefab::LoadFunc CPrefab::m_pLoadFunc = nullptr;

CPrefab::CPrefab()
    : CRes(RES_TYPE::PREFAB)
    , m_pProtoObj(nullptr)
    , m_iLayer(1)
{
}

CPrefab::CPrefab(CGameObject* _pProtoObj)
    : CRes(RES_TYPE::PREFAB)
    , m_pProtoObj(_pProtoObj)
    , m_iLayer(_pProtoObj->GetLayerIndex())
{
}

CPrefab::~CPrefab()
{
    SAFE_DELETE(m_pProtoObj);
}

CGameObject* CPrefab::Instantiate()
{
    return m_pProtoObj->Clone();
}

int CPrefab::Save(const wstring& _strFilePath)
{
    m_pSaveFunc(this, _strFilePath);

    CRes::Save(_strFilePath);

    return S_OK;
}

int CPrefab::Load(const wstring& _strFilePath)
{
    return m_pLoadFunc(this, _strFilePath);
}
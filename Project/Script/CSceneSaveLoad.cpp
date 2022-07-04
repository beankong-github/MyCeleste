#include "pch.h"
#include "CSceneSaveLoad.h"

#include <Engine/CSceneMgr.h>
#include <Engine/CResMgr.h>
#include <Engine/CScene.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>

#include <Engine/CScript.h>
#include "CScriptMgr.h"

#include <Engine/CParticleSystem.h>


void CSceneSaveLoad::SaveScene(CScene* _pScene, const wstring& _strSceneFilePath)
{
    // ���ҽ� ������� ����
    CResMgr::GetInst()->SaveChangedRes();

    // ==================
    //     Open File
    // ==================
    // 1. �Է¹��� ��ο��� Save�� ������ "WriteBinary" ���� ����
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, _strSceneFilePath.c_str(), L"wb");

    // 2. ������ ������ �ʾҴٸ� Assert
    assert(pFile);
    if (nullptr == pFile)
        return;

    // ==================
    //     Write File
    // ==================
    // 1. Scene �̸� ����
    _pScene->SaveToScene(pFile);

    // 2. Scene �� Layer ���� ����
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        // 2-1. Layer �̸� ����
        CLayer* pLayer = _pScene->GetLayer(i);
        pLayer->SaveToScene(pFile);

        // 2-2. Layer �� �������� Root ������Ʈ ���� ����
        vector<CGameObject*>& vecRootObj = pLayer->GetRootObjects();
        size_t iObjCount = vecRootObj.size();
        fwrite(&iObjCount, sizeof(size_t), 1, pFile);

        // 2-3.�� ��Ʈ ������Ʈ ����
        for (size_t j = 0; j < vecRootObj.size(); ++j)
        {
            SaveGameObject(vecRootObj[j], pFile);
        }
    }
    
    // ==================
    //     Close File
    // ==================
    // ���� �ݱ�
    fclose(pFile);
}

void CSceneSaveLoad::SaveGameObject(CGameObject* _pObj, FILE* _pFile)
{
    // 1. �̸�, ����, ������Ʈ ����
    _pObj->SaveToScene(_pFile);

    // 2. Script ����
    // ���� ������Ʈ�� ����� ��ũ��Ʈ�� ��������
    const vector<CScript*>& vecScript = _pObj->GetScripts();
    
    // ��ũ��Ʈ ���� ����
    size_t iScriptCount = vecScript.size();
    fwrite(&iScriptCount, sizeof(size_t), 1, _pFile);
    
    // ��ũ��Ʈ ����
    for (size_t i = 0; i < iScriptCount; ++i)
    {
        wstring strScriptName = CScriptMgr::GetScriptName(vecScript[i]);
        SaveWStringToFile(strScriptName, _pFile);   // ��ũ��Ʈ �̸� ����
        vecScript[i]->SaveToScene(_pFile);  // ��ũ��Ʈ ����
    }

    // 3.Child Object ����
    const vector<CGameObject*>& vecChild = _pObj->GetChild();
    size_t iChildCount = vecChild.size();
    fwrite(&iChildCount, sizeof(size_t), 1, _pFile);

    for (size_t i = 0; i < iChildCount; ++i)
    {
        SaveGameObject(vecChild[i], _pFile);
    }
}


CScene* CSceneSaveLoad::LoadScene(const wstring& _strSceneFilePath)
{
    // =================
    //  Ready SceneFile
    // ================= 
    // ���� ��ο��� ����θ� ����
    wstring strKey = CPathMgr::GetInst()->GetRelativePath(_strSceneFilePath);

    // CResMgr ���� ����θ� Ű������, CSceneFile �� ã�Ƴ�
    CResMgr::GetInst()->Load<CSceneFile>(strKey, strKey);

    // Load �� Scene ���� �� ���Ϸκ��� �ε�
    CScene* pLoadScene = new CScene;
    pLoadScene->SetResKey(strKey);

    // ==================
    //     Open File
    // ==================
    // 1. �Է¹��� Path���� ����� Scene File�� "ReadBinary" ���� ����
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, _strSceneFilePath.c_str(), L"rb");

    // 2. ������ ������ �ʾҴٸ� assert
    assert(pFile);
    if (nullptr == pFile)
        return nullptr;

    // ==================
    //     Read File
    // ==================
    // 1. Scene �̸� �ҷ�����
    pLoadScene->LoadFromScene(pFile);

    // 2. Scene�� Layer ���� �ҷ�����
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        // 2-1. i��° ���̾� ��������
        CLayer* pLayer = pLoadScene->GetLayer(i);
        pLayer->LoadFromScene(pFile);

        // 2-2. Layer ������ Root ������Ʈ ����
        size_t iObjCount = 0;
        fread(&iObjCount, sizeof(size_t), 1, pFile);

        // 2-3. Root ������Ʈ �о����
        for (size_t j = 0; j < iObjCount; ++j)
        {
            CGameObject* pLoadObj = LoadGameObject(pFile);
            pLoadScene->AddObject(pLoadObj, i);
        }
    }

    // ==================
    //     Open File
    // ==================
    fclose(pFile);

    return pLoadScene;
}

CGameObject* CSceneSaveLoad::LoadGameObject(FILE* _pFile)
{
    CGameObject* pLoadObj = new CGameObject;

    // 1. �̸�, ����, ������Ʈ �ҷ�����
    pLoadObj->LoadFromScene(_pFile);

    // 2. Script �ҷ�����
    // Script ���� �ҷ�����
    size_t iScriptCount = 0;
    fread(&iScriptCount, sizeof(size_t), 1, _pFile);

    for (size_t i = 0; i < iScriptCount; ++i)
    {
        // Script �̸� �ҷ�����
        wstring wstrScriptName;
        LoadWStringFromFile(wstrScriptName, _pFile);
       
        // Script �̸����� ��ũ��Ʈ ã�Ƽ� �����ϰ� ������Ʈ�� �߰����ش�
        CScript* pLoadScript = CScriptMgr::GetScript(wstrScriptName);
        pLoadObj->AddComponent(pLoadScript);

        // Script Data �ҷ�����
        pLoadScript->LoadFromScene(_pFile);
    }

    
    // 3. �ڽ� GameObject �ε� 
    size_t iChildCount = 0;
    fread(&iChildCount, sizeof(size_t), 1, _pFile);

    for (size_t i = 0; i < iChildCount; ++i)
    {
        CGameObject* pChild = LoadGameObject(_pFile);
        pLoadObj->AddChild(pChild);
    }

    return pLoadObj;
}

// ======
// Prefab
// ======
void CSceneSaveLoad::SavePrefab(CPrefab* _Prefab, const wstring& _strFilePath)
{
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, _strFilePath.c_str(), L"wb");

    SaveGameObject(_Prefab->GetProto(), pFile);

    fclose(pFile);
}

int CSceneSaveLoad::LoadPrefab(CPrefab* _Prefab, const wstring& _strFilePath)
{
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

    if (nullptr == pFile)
        return E_FAIL;

    // �����տ� ������Ÿ���� ���� ��� �ε��� GameObject�� Proto�� �������ش�.
    if (nullptr != _Prefab->GetProto())
    {
        CGameObject* pProto = LoadGameObject(pFile);
        _Prefab->SetProto(pProto);
    }

    // �����տ� ������Ÿ���� ������ �����ϰ� ���� �ε�� GameObjcet�� Proto�� �������ش�.
    //else
    //{
    //    delete(_Prefab->GetProto());
    //
    //    CGameObject* pProto = LoadGameObject(pFile);
    //    _Prefab->SetProto(pProto);
    //}
    
    fclose(pFile);

    return S_OK;
}

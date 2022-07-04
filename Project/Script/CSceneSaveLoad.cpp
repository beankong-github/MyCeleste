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
    // 리소스 변경상태 저장
    CResMgr::GetInst()->SaveChangedRes();

    // ==================
    //     Open File
    // ==================
    // 1. 입력받은 경로에서 Save할 파일을 "WriteBinary" 모드로 연다
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, _strSceneFilePath.c_str(), L"wb");

    // 2. 파일이 열리지 않았다면 Assert
    assert(pFile);
    if (nullptr == pFile)
        return;

    // ==================
    //     Write File
    // ==================
    // 1. Scene 이름 저장
    _pScene->SaveToScene(pFile);

    // 2. Scene 의 Layer 정보 저장
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        // 2-1. Layer 이름 저장
        CLayer* pLayer = _pScene->GetLayer(i);
        pLayer->SaveToScene(pFile);

        // 2-2. Layer 가 보유중인 Root 오브젝트 개수 저장
        vector<CGameObject*>& vecRootObj = pLayer->GetRootObjects();
        size_t iObjCount = vecRootObj.size();
        fwrite(&iObjCount, sizeof(size_t), 1, pFile);

        // 2-3.각 루트 오브젝트 저장
        for (size_t j = 0; j < vecRootObj.size(); ++j)
        {
            SaveGameObject(vecRootObj[j], pFile);
        }
    }
    
    // ==================
    //     Close File
    // ==================
    // 파일 닫기
    fclose(pFile);
}

void CSceneSaveLoad::SaveGameObject(CGameObject* _pObj, FILE* _pFile)
{
    // 1. 이름, 상태, 컴포넌트 저장
    _pObj->SaveToScene(_pFile);

    // 2. Script 저장
    // 게임 오브젝트에 적용된 스크립트들 가져오기
    const vector<CScript*>& vecScript = _pObj->GetScripts();
    
    // 스크립트 개수 저장
    size_t iScriptCount = vecScript.size();
    fwrite(&iScriptCount, sizeof(size_t), 1, _pFile);
    
    // 스크립트 저장
    for (size_t i = 0; i < iScriptCount; ++i)
    {
        wstring strScriptName = CScriptMgr::GetScriptName(vecScript[i]);
        SaveWStringToFile(strScriptName, _pFile);   // 스크립트 이름 저장
        vecScript[i]->SaveToScene(_pFile);  // 스크립트 저장
    }

    // 3.Child Object 저장
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
    // 최종 경로에서 상대경로만 추출
    wstring strKey = CPathMgr::GetInst()->GetRelativePath(_strSceneFilePath);

    // CResMgr 에서 상대경로를 키값으로, CSceneFile 을 찾아냄
    CResMgr::GetInst()->Load<CSceneFile>(strKey, strKey);

    // Load 할 Scene 생성 및 파일로부터 로딩
    CScene* pLoadScene = new CScene;
    pLoadScene->SetResKey(strKey);

    // ==================
    //     Open File
    // ==================
    // 1. 입력받은 Path에서 저장된 Scene File을 "ReadBinary" 모드로 연다
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, _strSceneFilePath.c_str(), L"rb");

    // 2. 파일이 열리지 않았다면 assert
    assert(pFile);
    if (nullptr == pFile)
        return nullptr;

    // ==================
    //     Read File
    // ==================
    // 1. Scene 이름 불러오기
    pLoadScene->LoadFromScene(pFile);

    // 2. Scene의 Layer 정보 불러오기
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        // 2-1. i번째 레이어 가져오기
        CLayer* pLayer = pLoadScene->GetLayer(i);
        pLayer->LoadFromScene(pFile);

        // 2-2. Layer 보유한 Root 오브젝트 개수
        size_t iObjCount = 0;
        fread(&iObjCount, sizeof(size_t), 1, pFile);

        // 2-3. Root 오브젝트 읽어오기
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

    // 1. 이름, 상태, 컴포넌트 불러오기
    pLoadObj->LoadFromScene(_pFile);

    // 2. Script 불러오기
    // Script 개수 불러오기
    size_t iScriptCount = 0;
    fread(&iScriptCount, sizeof(size_t), 1, _pFile);

    for (size_t i = 0; i < iScriptCount; ++i)
    {
        // Script 이름 불러오기
        wstring wstrScriptName;
        LoadWStringFromFile(wstrScriptName, _pFile);
       
        // Script 이름으로 스크립트 찾아서 생성하고 오브젝트에 추가해준다
        CScript* pLoadScript = CScriptMgr::GetScript(wstrScriptName);
        pLoadObj->AddComponent(pLoadScript);

        // Script Data 불러오기
        pLoadScript->LoadFromScene(_pFile);
    }

    
    // 3. 자식 GameObject 로드 
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

    // 프리팹에 프로토타입이 없을 경우 로드한 GameObject를 Proto로 설정해준다.
    if (nullptr != _Prefab->GetProto())
    {
        CGameObject* pProto = LoadGameObject(pFile);
        _Prefab->SetProto(pProto);
    }

    // 프리팹에 프로토타입이 있으면 삭제하고 새로 로드된 GameObjcet를 Proto로 설정해준다.
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

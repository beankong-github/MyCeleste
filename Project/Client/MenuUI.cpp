#include "pch.h"
#include "MenuUI.h"

#include <Engine\CCore.h>
#include <Engine\CPathMgr.h>
                
#include <Engine\CSceneMgr.h>
#include <Engine\CScene.h>
#include <Script\CScriptMgr.h>

#include <Engine\CGameObject.h>
#include <Engine\CTransform.h>
#include <Engine\CCamera.h>
#include <Engine\CCollider2D.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CTileMap.h>
#include <Engine\CLight2D.h>
#include <Engine\CParticleSystem.h>
#include <Engine\CMeshRender.h>

#include <Script\CSceneSaveLoad.h>
#include "CImGuiMgr.h"
#include "SceneOutliner.h"
#include "InspectorUI.h"

MenuUI::MenuUI()
    : UI("Menu")
    , m_strPlayPause()
    , m_bPackaging(false)
    , m_bSceneSave(false)
    , m_bSceneLoad(false)
    , m_bSceneStop(false)
    , m_bScenePause(false)
    , m_bScenePlay(false)
{
}

MenuUI::~MenuUI()
{
}

void MenuUI::update()
{
    Task();
}

void MenuUI::render()
{
    if (ImGui::BeginMainMenuBar())
    {
        render_update();

        ImGui::EndMainMenuBar();
    }
}

void MenuUI::render_update()
{
    //if (ImGui::BeginMenu("File"))
    //{
    //    ImGui::MenuItem("Packaging", NULL, &m_bPackaging);


    //    ImGui::EndMenu();
    //}

    if (ImGui::BeginMenu("Scene"))
    {
        ImGui::MenuItem("Scene Save", NULL, &m_bSceneSave);
        ImGui::MenuItem("Scene Load", NULL, &m_bSceneLoad);

        ImGui::EndMenu();
    }
   

    if (ImGui::BeginMenu("Object"))
    {
        if (ImGui::MenuItem("CreateObject", NULL))
        {
            CGameObject* pObj = new CGameObject;
            pObj->SetName(L"GameObject");
            pObj->AddComponent(new CTransform);
            Vec3 vPos(0.f, 0.f, 0.f);
            CSceneMgr::GetInst()->SpawnObject(pObj, vPos, L"GameObject", 1);
        }

        if (ImGui::BeginMenu("Add Component"))
        {
            // Inspector에 TargetObject가 있는지 확인
            bool available = false;
            CGameObject* pTarget = nullptr;

            InspectorUI* pInspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
            if (nullptr != pInspector)
            {
                pTarget = pInspector->GetTargetObject();
                available = (pTarget != nullptr);
            }
            else
            {
                available = false;
            }

			if (ImGui::MenuItem("Camera", NULL, false, available))
			{
				AddComponent(pTarget, COMPONENT_TYPE::CAMERA);
				pTarget->Camera()->CheckLayerMaskAll();
			}
			if (ImGui::MenuItem("Collider2D", NULL, false, available))
			{
				AddComponent(pTarget, COMPONENT_TYPE::COLLIDER2D);
			}
			//ImGui::MenuItem("Collider3D", NULL, false, available);

			if (ImGui::MenuItem("Animator2D", NULL, false, available))
			{
				AddComponent(pTarget, COMPONENT_TYPE::ANIMATOR2D);
			}
			//ImGui::MenuItem("Animator3D", NULL, false, available);

			if (ImGui::MenuItem("LIGHT2D", NULL, false, available))
			{
				AddComponent(pTarget, COMPONENT_TYPE::LIGHT2D);
			}
			//ImGui::MenuItem("LIGHT3D", NULL, false, available);

			if (ImGui::MenuItem("MeshRender", NULL, false, available))
			{
				AddComponent(pTarget, COMPONENT_TYPE::MESHRENDER);
			}
			if (ImGui::MenuItem("TileMap", NULL, false, available))
			{
                AddComponent(pTarget, COMPONENT_TYPE::TILEMAP);
			}
			if (ImGui::MenuItem("ParticleSystem", NULL, false, available))
			{
                AddComponent(pTarget, COMPONENT_TYPE::PARTICLESYSTEM);
		}
            //ImGui::MenuItem("LandScape",NULL, false, available);
            //ImGui::MenuItem("Decal",NULL, false, available);
            
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Add Script"))
        {
            // Inspector에 TargetObject가 있는지 확인
            bool available = false;
            CGameObject* pTarget = nullptr;

            InspectorUI* pInspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
            if (nullptr != pInspector)
            {
                pTarget = pInspector->GetTargetObject();
                available = (pTarget != nullptr);
            }
            else
            {
                available = false;
            }

            vector<wstring> vecScriptName;
            CScriptMgr::GetScriptInfo(vecScriptName);
            for (size_t i = 0; i < vecScriptName.size(); ++i)
            {
                string strScriptName = string(vecScriptName[i].begin(), vecScriptName[i].end());
                ImGui::MenuItem(strScriptName.c_str(), NULL, false, available);
            }
            ImGui::EndMenu();
        }        
        
        ImGui::EndMenu();
    }


    if (ImGui::BeginMenu("Tools"))
    {
        ImGui::EndMenu();
    }

    
    // Scene Player
    CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
    SCENE_STATE eState = pCurScene->GetSceneState();

    ImGui::Dummy(ImVec2{ ImGui::GetWindowWidth()/3, 0.f});

    // Play & Pause
    if (SCENE_STATE::PLAY == eState)
    {
        ImGui::MenuItem(ICON_KI_PLAY, NULL, &m_bScenePlay, false);
        ImGui::MenuItem(ICON_KI_PAUSE, NULL, &m_bScenePause, true);
    }
    else
    {
        ImGui::MenuItem(ICON_KI_PLAY, NULL, &m_bScenePlay, true);
        ImGui::MenuItem(ICON_KI_PAUSE, NULL, &m_bScenePause, false);
    }

    // Stop
    if (SCENE_STATE::STOP == eState)
        ImGui::MenuItem(ICON_KI_STOP, NULL, &m_bSceneStop, false);
    else
        ImGui::MenuItem(ICON_KI_STOP, NULL, &m_bSceneStop, true);
}


void MenuUI::Task()
{
    if (m_bSceneSave)
    {
        wchar_t szName[256] = {};

        OPENFILENAME ofn = {};

        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = CCore::GetInst()->GetMainHwnd();
        ofn.lpstrFile = szName;
        ofn.nMaxFile = sizeof(szName);
        ofn.lpstrFilter = L"*.scene\0";
        ofn.nFilterIndex = 0;
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;

        wstring strTileFolder = CPathMgr::GetInst()->GetContentPath();
        strTileFolder += L"scene";

        ofn.lpstrInitialDir = strTileFolder.c_str();
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        // Modal
        if (GetSaveFileName(&ofn))
        {
            CSceneSaveLoad::SaveScene(CSceneMgr::GetInst()->GetCurScene(), szName);
        }

        m_bSceneSave = false;
    }

    else if (m_bSceneLoad)
    {
        wchar_t szName[256] = {};

        OPENFILENAME ofn = {};

        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = CCore::GetInst()->GetMainHwnd();
        ofn.lpstrFile = szName;
        ofn.nMaxFile = sizeof(szName);
        ofn.lpstrFilter = L"*.scene\0";
        ofn.nFilterIndex = 0;
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;

        wstring strTileFolder = CPathMgr::GetInst()->GetContentPath();
        strTileFolder += L"scene";

        ofn.lpstrInitialDir = strTileFolder.c_str();
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        // Modal
        if (GetOpenFileName(&ofn))
        {
            CScene* pLoadScene = CSceneSaveLoad::LoadScene(szName);
            CSceneMgr::GetInst()->ChangeScene(pLoadScene);
        }

        // SceneOutliner 갱신
        SceneOutliner* pUI = (SceneOutliner*)CImGuiMgr::GetInst()->FindUI("SceneOutliner");
        pUI->Reset();
        
        m_bSceneLoad = false;
    }

    if (m_bScenePlay || m_bScenePause || m_bSceneStop) 
    {
        CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
        SCENE_STATE eState = pCurScene->GetSceneState();

        if (m_bScenePlay)
        {
            if (SCENE_STATE::PLAY != eState)
                pCurScene->SetSceneState(SCENE_STATE::PLAY);

            m_bScenePlay = false;
        }
        if (m_bScenePause)
        {
            if (SCENE_STATE::PLAY == eState)
                pCurScene->SetSceneState(SCENE_STATE::PAUSE);
            m_bScenePause = false;
        }
        if (m_bSceneStop)
        {
            if (SCENE_STATE::STOP != eState)
            {
                pCurScene->SetSceneState(SCENE_STATE::STOP);
                CSceneFile* pSceneFile = pCurScene->GetSceneFile().Get();

                wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + pSceneFile->GetRelativePath();
                CScene* pNewScene = CSceneSaveLoad::LoadScene(strFilePath);
                CSceneMgr::GetInst()->ChangeScene(pNewScene);
            }
            // SceneOutliner 갱신
            ((SceneOutliner*)CImGuiMgr::GetInst()->FindUI("SceneOutliner"))->Reset();

            m_bSceneStop = false;
        }
    }
}

void MenuUI::AddComponent(CGameObject* _pTarget, COMPONENT_TYPE _type)
{
    assert(_pTarget);

    CComponent* pCom = _pTarget->GetComponent(_type);
    if (nullptr == pCom)
    {
        switch (_type)
        {
        case COMPONENT_TYPE::CAMERA:
            _pTarget->AddComponent(new CCamera);
            break;
        case COMPONENT_TYPE::COLLIDER2D:
            _pTarget->AddComponent(new CCollider2D);
            break;
        case COMPONENT_TYPE::COLLIDER3D:
            break;
        case COMPONENT_TYPE::ANIMATOR2D:
            _pTarget->AddComponent(new CAnimator2D);
            break;
        case COMPONENT_TYPE::ANIMATOR3D:
            break;
        case COMPONENT_TYPE::LIGHT2D:
            _pTarget->AddComponent(new CLight2D);
            break;
        case COMPONENT_TYPE::LIGHT3D:
            break;
        case COMPONENT_TYPE::BOUNDINGBOX:
            break;
        case COMPONENT_TYPE::MESHRENDER:
            _pTarget->AddComponent(new CMeshRender);
            break;
        case COMPONENT_TYPE::TILEMAP:
            _pTarget->AddComponent(new CTileMap);
            break;
        case COMPONENT_TYPE::PARTICLESYSTEM:
            _pTarget->AddComponent(new CParticleSystem);
            break;
        case COMPONENT_TYPE::LANDSCAPE:
            break;
        case COMPONENT_TYPE::DECAL:
            break;
        default:
            break;
        }
    }

    InspectorUI* pInspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
    if (nullptr != pInspector)
    {
        pInspector->SetTargetObject(_pTarget);
    }
}


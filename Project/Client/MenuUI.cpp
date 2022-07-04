#include "pch.h"
#include "MenuUI.h"

#include <Engine/CCore.h>
#include <Engine/CPathMgr.h>

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Script/CScriptMgr.h>

#include <Script/CSceneSaveLoad.h>
#include "CImGuiMgr.h"
#include "SceneOutliner.h"

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

    if (ImGui::BeginMenu("Component"))
    {
        if (ImGui::BeginMenu("Add Component"))
        {
            ImGui::MenuItem("MeshRender", NULL);
            ImGui::MenuItem("Camera", NULL);
            ImGui::MenuItem("Collider2D", NULL);
            ImGui::MenuItem("Collider3D", NULL);
            ImGui::MenuItem("Animator2D", NULL);
            ImGui::MenuItem("Animator3D", NULL);

            if (ImGui::BeginMenu("Add Script"))
            {
                vector<wstring> vecScriptName;
                CScriptMgr::GetScriptInfo(vecScriptName);
                for (size_t i = 0; i < vecScriptName.size(); ++i)
                {
                    string strScriptName = string(vecScriptName[i].begin(), vecScriptName[i].end());
                    ImGui::MenuItem(strScriptName.c_str(), NULL);
                }
                ImGui::EndMenu();
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

        // SceneOutliner °»½Å
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

            m_bSceneStop = false;
        }
    }
}

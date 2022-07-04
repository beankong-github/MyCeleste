#include "pch.h"
#include "CImGuiMgr.h"

#include <Engine/CDevice.h>
#include <Engine/CGameObject.h>
#include <Engine/CSceneMgr.h>
#include <Engine/CPathMgr.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

#include "UI.h"
#include "ParamUI.h"



CImGuiMgr::CImGuiMgr()
{

}

CImGuiMgr::~CImGuiMgr()
{
    Safe_Del_Map(m_mapUI);
}

void CImGuiMgr::init(HWND _hwnd)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDefaultParent = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // 한글 폰트 추가
    wstring wstrFilePath = CPathMgr::GetInst()->GetContentPath();
    string strFontPath = string(wstrFilePath.begin(), wstrFilePath.end()) + "font\\Pretendard-Medium.ttf";
    io.Fonts->AddFontFromFileTTF(strFontPath.c_str(), 15.f, NULL, io.Fonts->GetGlyphRangesKorean());

    // Icon 추가
    //io.Fonts->AddFontDefault();
    string strIconPath = string(wstrFilePath.begin(), wstrFilePath.end()) + "font\\icon\\";
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    
    // IconsFontAwesome6
    static const ImWchar FA_icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    string _font = strIconPath + FONT_ICON_FILE_NAME_FAS;
    io.Fonts->AddFontFromFileTTF(_font.c_str(), 13.0f, &icons_config, FA_icons_ranges);
    
    // IconsKenney
    static const ImWchar KI_icons_ranges[] = { ICON_MIN_KI, ICON_MAX_16_KI, 0 };
    _font = strIconPath + FONT_ICON_FILE_NAME_KI;
    io.Fonts->AddFontFromFileTTF(_font.c_str(), 13.0f, &icons_config, KI_icons_ranges);
    
    // IconsMaterialDesign
    static const ImWchar MD_icons_ranges[] = { ICON_MIN_MD, ICON_MAX_16_MD, 0 };
    _font = strIconPath + FONT_ICON_FILE_NAME_MD;
    io.Fonts->AddFontFromFileTTF(_font.c_str(), 13.0f, &icons_config, MD_icons_ranges);


    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(_hwnd);
    ImGui_ImplDX11_Init(DEVICE, CONTEXT);


    // 기본 UI 들 생성
    CreateUI();

    // 알림설정  
    wstring strPath = CPathMgr::GetInst()->GetContentPath();
    m_hNotify = FindFirstChangeNotification(strPath.c_str(), TRUE,
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_ACTION_ADDED | FILE_ACTION_REMOVED);
}

void CImGuiMgr::progress()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Param Init
    ParamUI::KeyCount = 0;

    // UI Update
    for (auto& pair : m_mapUI)
    {
        pair.second->update();
    }

    // UI Render
    for (auto& pair : m_mapUI)
    {
        pair.second->render();
    }

    bool bOpen = true;
    
    // ImGui 데모 윈도우 표시
    ImGui::ShowDemoWindow(&bOpen);


    // Delegate 호출
    for (size_t i = 0; i < m_vecDelegate.size(); ++i)
    {
        (m_vecDelegate[i].pInst->*m_vecDelegate[i].pFunc)(m_vecDelegate[i].dwParam);
    }
    m_vecDelegate.clear();

    // Content File 변경 감지
    ObserveContent();
}

void CImGuiMgr::render()
{
    // Rendering
    ImGui::Render();   
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void CImGuiMgr::clear()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

#include "MenuUI.h"
#include "InspectorUI.h"
#include "ResourceUI.h"
#include "SceneOutliner.h"
#include "ListUI.h"
#include "LandScapeEditor.h"
#include "TileMapEditor.h"
#include "Create2DAnimationUI.h"

void CImGuiMgr::CreateUI()
{
    // MenuBar 생성
    MenuUI* pMenuUI = new MenuUI;
    pMenuUI->Activate();
    m_mapUI.insert(make_pair(pMenuUI->GetName(), pMenuUI));

    // Animation 2D Tool 추가
    Create2DAnimationUI* pCreate2DAnimUI = new Create2DAnimationUI;
    pCreate2DAnimUI->Deactivate();
    m_mapUI.insert(make_pair(pCreate2DAnimUI->GetName(), pCreate2DAnimUI));

    // InspectorUI 생성
    InspectorUI* pUI = new InspectorUI;        
    pUI->SetTargetObject(nullptr);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    // ResourceUI 생성
    ResourceUI* pResUI = new ResourceUI;    
    m_mapUI.insert(make_pair(pResUI->GetName(), pResUI));

    // SceneOutLiner
    SceneOutliner* pSceneOutliner = new SceneOutliner;
    m_mapUI.insert(make_pair(pSceneOutliner->GetName(), pSceneOutliner));

    // ListUI 생성
    ListUI* pListUI = new ListUI;    
    pListUI->Deactivate();
    pListUI->SetModalUI(true);    
    m_mapUI.insert(make_pair(pListUI->GetName(), pListUI));

    // LandScapeEditor 생성
    LandScapeEditor* pLSEidtor = new LandScapeEditor;
    pLSEidtor->Deactivate();
    m_mapUI.insert(make_pair(pLSEidtor->GetName(), pLSEidtor));

    // TileEditor 생성
    TileMapEditor* pTileMapEditor = new TileMapEditor;
    pTileMapEditor->Deactivate();
    pTileMapEditor->SetLSEditor(pLSEidtor);
    m_mapUI.insert(make_pair(pTileMapEditor->GetName(), pTileMapEditor));

}

UI* CImGuiMgr::FindUI(const string& _strKey)
{
    map<string, UI*>::iterator iter = m_mapUI.find(_strKey);

    if (m_mapUI.end() == iter)
    {
        return nullptr;
    }

    return iter->second;
}


void CImGuiMgr::ObserveContent()
{
    DWORD dwWaitStatus = WaitForSingleObject(m_hNotify, 0);

    if (dwWaitStatus == WAIT_OBJECT_0)
    {
        ResourceUI* pResUI = (ResourceUI*)FindUI("Resource");
        pResUI->Reset();

        wstring strPath = CPathMgr::GetInst()->GetContentPath();
        FindNextChangeNotification(m_hNotify);
    }
}
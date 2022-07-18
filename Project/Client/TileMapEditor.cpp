#include "pch.h"
#include "TileMapEditor.h"

#include <Engine\CTexture.h>
#include <Engine\CCollider2D.h>
#include <Engine\CSceneMgr.h>
#include <Engine\CScene.h>
#include <Engine\CTransform.h>
#include "CImGuiMgr.h"
#include "SceneOutliner.h"
#include <Script\CCollider2DScript.h>

TileMapEditor::TileMapEditor()
	: UI("TileMapEditor")
	, m_pTargetGameObject(nullptr)
	, m_pTargetTileMap(nullptr)
    , m_pLSEditor(nullptr)
    , m_pSelecterCol(nullptr)
    , m_vOffsetPos{ 0.f , }
    , m_vOffsetScale{0.f, }

{
	SetSize(Vec2{ 500, 0 });
}


TileMapEditor::~TileMapEditor()
{
}


void TileMapEditor::SetTargetObject(CGameObject* _pObj)
{
    // 타겟 게임 오브젝트는 TileMap 컴포넌트가 붙은 오브젝트여야한다.
    assert(_pObj->TileMap());

    // 초기화
    Clear();

    // 멤버 변수 다시 세팅
    m_pTargetTileMap = _pObj->TileMap();
    m_pTargetGameObject = _pObj;
    CopyTileMap(m_pTargetTileMap->GetTilaDataVector());
}

void TileMapEditor::CopyTileMap(vector<tTileData> _vecTileData)
{
    m_vecCopyTileMap.resize(_vecTileData.size());
    std::copy(_vecTileData.begin(), _vecTileData.end(), m_vecCopyTileMap.begin());

    m_vecCopyTileMapSize = m_pTargetTileMap->GetTileMapSize();
}


void TileMapEditor::Clear()
{
	m_pTargetGameObject = nullptr;
	m_pTargetTileMap = nullptr;
    m_vecCopyTileMap.clear();
}

void TileMapEditor::Apply()
{
    int size = m_vecCopyTileMap.size();
    for (int i = 0; i < size; ++i)
    {
        m_pTargetTileMap->SetTileData(i, m_vecCopyTileMap[i].iImgIdx);
    }
}

void TileMapEditor::Revert()
{
    CopyTileMap(m_pTargetTileMap->GetTilaDataVector());
}

void TileMapEditor::update()
{
}

void TileMapEditor::render_update()
{
    // ================
    //   TileMap Size
    // ================
    // 타일맵의 타일 수 가져오기
    static Vec2 iNewTileMapSize;
    int iTileMapSizeBuffer[2] = { m_vecCopyTileMapSize.x, m_vecCopyTileMapSize.y };

    ImGui::Text("TileMapSize");
    ImGui::SameLine();
    // 타일맵의 타일 수를 표시하고 편집할 Input 창을 띄운다.
    if (ImGui::InputInt2("##TileMapSize", iTileMapSizeBuffer, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        // 수정하고 Enter를 클릭하면 Modal창을 띄워 경고를 해준다,
        iNewTileMapSize = Vec2{ iTileMapSizeBuffer[0],iTileMapSizeBuffer[1] };

        if (!ImGui::IsPopupOpen("Warning"))
            ImGui::OpenPopup("Warning");
    }
    // 타일맵 사이즈 조절하면 띄워주는 경고 모달창
    if (ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text(u8"타일맵 사이즈를 재조정하면 현재 타일맵 정보가 오브젝트에 저장됩니다.\n 또한 변경한 뒤 기존의 데이터를 복구할 수 없을 수 있습니다.\n 그래도 타일맵 사이즈를 재조정 하겠습니까?");

        float item_height = ImGui::GetTextLineHeightWithSpacing();

        // Yes -> 수정된 타일맵 반영
        ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
        if (ImGui::Button("Yes", button_size))
        {
            // 기존 타일 데이터를 저장해둔다.
            vector<vector<tTileData>> vecTmpTileMap;
            vecTmpTileMap.resize((size_t)m_vecCopyTileMapSize.y, vector<tTileData>((size_t)m_vecCopyTileMapSize.x, tTileData{}));

            for (int i = 0; i < m_vecCopyTileMapSize.y; i++)
            {
                for (int j = 0; j < m_vecCopyTileMapSize.x; j++)
                {
                    vecTmpTileMap[i][j] = m_vecCopyTileMap[m_vecCopyTileMapSize.x * i + j];
                }
            }

            // 타일 초기화
            m_vecCopyTileMap.clear();

            // 백업해둔 타일을 다시 세팅한다
            m_vecCopyTileMap.resize(iNewTileMapSize.x * iNewTileMapSize.y);

            for (int i = 0; i < m_vecCopyTileMapSize.y; i++)
            {
                for (int j = 0; j < m_vecCopyTileMapSize.x; j++)
                {
                    if (j >= iNewTileMapSize.x || i >= iNewTileMapSize.y)
                        continue;
                    m_vecCopyTileMap[iNewTileMapSize.x * i + j]= vecTmpTileMap[i][j];
                }
            }

            // 적용
            m_pTargetTileMap->SetTileMapSize(iNewTileMapSize);
            Apply();
            SetTargetObject(m_pTargetGameObject);

            // 세팅 초기화
            iNewTileMapSize = Vec2{ 0.f, 0.f };
            vecTmpTileMap.clear();

            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        // No -> 모달 창 종료
        if (ImGui::Button("No", button_size))
        {
            // 세팅 초기화
            iNewTileMapSize = Vec2{ 0.f, 0.f };

            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();

        return;
    }

    // ==========================
    //   TileMap Editor Canvas
    // ==========================
    static ImVector<ImVec2> points;
    static ImVec2 offset(0.0f, 0.0f);
    static float zoom(1.f);
    static bool opt_enable_grid = true;

    // 1. 캔버스 그리기
    // 캔버스에 그리드를 표시할지 체크
    ImGui::Checkbox("Enable grid", &opt_enable_grid);

    // 캔버스 위치, 크기 잡기
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    canvas_sz.y -= 25.f;
    if (canvas_sz.x < 80.0f) canvas_sz.x = 80.0f;
    if (canvas_sz.y < 80.0f) canvas_sz.y = 80.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // 캔버스 Outline을 그리고 배경색을 채운다. 
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

    // 캔버스가 사용자의 마우스 입력을 감지하도록 버튼 추가
    ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);

    // 마우스 상태 저장 변수들
    const bool is_hovered = ImGui::IsItemHovered(); // Hovered
    const bool is_active = ImGui::IsItemActive();   // Held
    const ImVec2 origin(canvas_p0.x + offset.x, canvas_p0.y + offset.y);
    const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);
    const ImVec2 zoomed_tilesize = ImVec2{ m_pTargetTileMap->GetTileSize().x / zoom,m_pTargetTileMap->GetTileSize().y / zoom };

    // 그리드 그리기
    draw_list->PushClipRect(canvas_p0, canvas_p1, true);
    if (opt_enable_grid)
    {
        // 그리드 크기
        const float GRID_STEP = zoomed_tilesize.x;

        // 가로선 그리기
        for (float x = fmodf(offset.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));

        // 세로선 그리기
        for (float y = fmodf(offset.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
    }

    // 타일맵 그리기
    CTexture* pAtlasTexture = m_pTargetTileMap->GetAtlasTex();
    if(nullptr != pAtlasTexture)
		for (int i = 0; i < iTileMapSizeBuffer[1]; i++)
		{
			for (int j = 0; j < iTileMapSizeBuffer[0]; j++)
			{
				// 그릴 위치 찾기
				ImVec2 vLP = ImVec2{ origin.x + zoomed_tilesize.x * j, origin.y + zoomed_tilesize.y * i };
				ImVec2 vRB = ImVec2{ vLP.x + zoomed_tilesize.x, vLP.y + zoomed_tilesize.y };

				// 타일 데이터 가져오기
				tTileData tCurTile = m_vecCopyTileMap[j + i * iTileMapSizeBuffer[0]];

				// 타일의 이미지 인덱스가 -1이면 그리지 않는다.
				if (tCurTile.iImgIdx != -1)
				{
					// Right-Bottom UV 계산하기
					ImVec2 vRBUV = ImVec2{ tCurTile.vLTUV.x + m_pTargetTileMap->GetTileSliceUV().x, tCurTile.vLTUV.y + m_pTargetTileMap->GetTileSliceUV().y };

					// 타일 그리기
					draw_list->AddImage(pAtlasTexture->GetSRV().Get(), vLP, vRB, tCurTile.vLTUV, vRBUV);
				}
			}
		}

    // TileMap 영역 표시
    const ImVec2 vZoomedTileMapSize = ImVec2{ iTileMapSizeBuffer[0] * zoomed_tilesize.x ,iTileMapSizeBuffer[1] * zoomed_tilesize.y };
    draw_list->AddRect(origin, ImVec2{ origin.x + vZoomedTileMapSize.x,origin.y + vZoomedTileMapSize.y }, IM_COL32(255, 0, 0, 255));


    // collider 그리기용 변수
    static int rect[2] = { -1, -1 };

    // canvas 밖에다 클릭하면 초기화
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        rect[0] = -1;
        rect[1] = -1;
    }
    if (rect[0] != -1 && rect[1] != -1)
    {
        int startTile_col = rect[0] % iTileMapSizeBuffer[0];
        int startTile_row = rect[0] / iTileMapSizeBuffer[0];
        int endTile_col = rect[1] % iTileMapSizeBuffer[0];
        int endTile_row = rect[1] / iTileMapSizeBuffer[0];
        int colliderSize_x = (endTile_col - startTile_col + 1) * zoomed_tilesize.x;
        int colliderSize_y = (endTile_row - startTile_row + 1) * zoomed_tilesize.y;
        ImVec2 col_p0 = ImVec2(origin.x + (zoomed_tilesize.x * startTile_col), origin.y + (zoomed_tilesize.y * (startTile_row)));
        ImVec2 col_p1 = ImVec2(col_p0.x + colliderSize_x, col_p0.y + colliderSize_y);
        draw_list->AddRect(col_p0, col_p1, IM_COL32(0, 250, 0, 250));

        Vec2 vRelativeTileSize = Vec2{ m_pTargetTileMap->Transform()->GetRelativeScale().x / iTileMapSizeBuffer[0],  m_pTargetTileMap->Transform()->GetRelativeScale().y / iTileMapSizeBuffer[1] };

        // 크기 계산
        m_vOffsetScale = Vec2{ (endTile_col - startTile_col + 1),(endTile_row - startTile_row + 1) };
        m_vOffsetScale *= vRelativeTileSize;

        // 위치 계산을 좌상단 기준으로 한다
        m_vOffsetPos = Vec2{ -m_pTargetTileMap->Transform()->GetRelativeScale().x / 2, m_pTargetTileMap->Transform()->GetRelativeScale().y / 2 };
        m_vOffsetPos += Vec2{ m_vOffsetScale.x / 2, -m_vOffsetScale.y / 2 };
        m_vOffsetPos.x += startTile_col * vRelativeTileSize.x;
        m_vOffsetPos.y -= startTile_row * vRelativeTileSize.y;
        //m_vOffsetPos.x += (startTile_col + (endTile_col - startTile_col / 2)) * vRelativeTileSize.x;
        //m_vOffsetPos.y += (startTile_row + (endTile_row - startTile_row / 2)) * vRelativeTileSize.y;
    }


    // 마우스 동작
    if (is_hovered)
    {
        // 마우스 위치가 canvas안 이라면
        if (mouse_pos_in_canvas.x > 0 && mouse_pos_in_canvas.x < vZoomedTileMapSize.x && mouse_pos_in_canvas.y > 0 && mouse_pos_in_canvas.y < vZoomedTileMapSize.y)
        {
            // 마우스 위치에 있는 타일 인덱스 구하기
            int iTile_col = mouse_pos_in_canvas.x / zoomed_tilesize.x;
            int iTile_row = mouse_pos_in_canvas.y / zoomed_tilesize.y;
            int iTileIndex = iTile_col + iTile_row * iTileMapSizeBuffer[0];

            // 마우스 위치에 있는 타일 하이라이트
            ImVec2 hover_p0 = ImVec2(origin.x + (zoomed_tilesize.x * iTile_col), origin.y + (zoomed_tilesize.y * (iTile_row)));
            ImVec2 hover_p1 = ImVec2(hover_p0.x + zoomed_tilesize.x, hover_p0.y + zoomed_tilesize.y);
            draw_list->AddRectFilled(hover_p0, hover_p1, IM_COL32(120, 120, 120, 120));

            // LandScape Editor의 타입에 따라 클릭시 동작이 달라진다.
            LS_EDIT_TYPE EditType = m_pLSEditor->GetType();
            switch (EditType)
            {
            case LS_EDIT_TYPE::TILE_MAP:
                DrawTile(iTileIndex);
                break;
            case LS_EDIT_TYPE::COLLIDER:
            {
                DrawCollider();
                // 네모 그리기
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    rect[0] = iTileIndex;
                }
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                {
                    rect[1] = iTileIndex;
                }
            }
                break;
            case LS_EDIT_TYPE::PREFAB:
                break;
            default:
                break;
            }
        }


        // 중앙 클릭 => 이미지 이동
        if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
        {
            offset.x += io.MouseDelta.x;
            offset.y += io.MouseDelta.y;
        }

        // 마우스 휠 => 크기 조정
        // Minimum zoom = x0.1 (near camera limit)
        // Maximum zoom = x5.0 (it can fit an image with 1600px width)
        zoom = zoom - (io.MouseWheel * 0.1f);
        if (zoom < 0.1f)
            zoom = 0.1f;
        else if (zoom > 5.0f)
            zoom = 5.0f;

    }
    draw_list->PopClipRect();

    if (LS_EDIT_TYPE::TILE_MAP == m_pLSEditor->GetType())
    {
        // Apply, Revert 버튼
        if (ImGui::Button("Apply"))
        {
            Apply();
        }

        ImGui::SameLine();

        if (ImGui::Button("Revert"))
        {
            Revert();
        }
    }
    else if (LS_EDIT_TYPE::COLLIDER == m_pLSEditor->GetType())
    {
        // Apply, Revert 버튼
        if (ImGui::Button("ADD"))
        {
            Add();
        }
    }


}
void TileMapEditor::DrawTile(int iTileIndex)
{
    // 에디터에서 선택된 타일 가져오기
    int iSelectedIdx = m_pLSEditor->GetCurTileIdx();

    // 좌클릭 => 타일 그리기
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
        ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        m_vecCopyTileMap[iTileIndex].iImgIdx = iSelectedIdx;

        if (iSelectedIdx != -1)
        {

            int iSelected_col = m_pLSEditor->GetCurTileIdx() % (int)m_pTargetTileMap->GetAtlasTileCount().x;
            int iSelected_row = m_pLSEditor->GetCurTileIdx() / (int)m_pTargetTileMap->GetAtlasTileCount().x;

            Vec2 vLTUV = Vec2(iSelected_col * m_pTargetTileMap->GetTileSliceUV().x, iSelected_row * m_pTargetTileMap->GetTileSliceUV().y);

            m_vecCopyTileMap[iTileIndex].vLTUV = vLTUV;
        }
    }


    // 우클릭 => 타일 지우기, 
    else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) ||
        ImGui::IsMouseDragging(ImGuiMouseButton_Right))
    {
        m_vecCopyTileMap[iTileIndex].iImgIdx = -1;
    }

}

void TileMapEditor::DrawCollider()
{
    for (size_t i = 0; i < m_vecChildCol.size(); i++)
    {
        // offset pos와 size 가져오기
    }
}

void TileMapEditor::Add()
{
    wstring name = L"col";
    int sameName = 0;
    vector<CGameObject*> childs = m_pTargetGameObject->GetChild();
    for (size_t i = 0; i < childs.size(); ++i)
    {
        if (name == childs[i]->GetName().substr(0, 3))
        {
            ++sameName;
        }
    }

    name += L"_" + std::to_wstring(sameName);
    
    CGameObject* pObj = new CGameObject;
    pObj->SetName(name);
    pObj->AddComponent(new CTransform);
    pObj->AddComponent(new CCollider2D);
    pObj->AddComponent(new CCollider2DScript);

    pObj->Collider2D()->SetOffsetPos(m_vOffsetPos);
    pObj->Collider2D()->SetOffsetScale(m_vOffsetScale);

    CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
    pCurScene->AddObject(pObj, m_pTargetGameObject->GetLayerIndex());
    m_pTargetGameObject->AddChild(pObj);

    // SceneOutliner 갱신
    SceneOutliner* pUI = (SceneOutliner*)CImGuiMgr::GetInst()->FindUI("SceneOutliner");
    pUI->Reset();
}

void TileMapEditor::AddPrefab(int iTileIndex)
{
}
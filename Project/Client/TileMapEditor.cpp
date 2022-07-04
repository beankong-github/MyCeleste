#include "pch.h"
#include "TileMapEditor.h"

#include <Engine/CTexture.h>

TileMapEditor::TileMapEditor()
	: UI("TileMapEditor")
	, m_pTargetGameObject(nullptr)
	, m_pTargetTileMap(nullptr)
    , m_pLSEditor(nullptr)

{
	SetSize(Vec2{ 500, 0 });
}

TileMapEditor::~TileMapEditor()
{
}

void TileMapEditor::Clear()
{
	m_pTargetGameObject = nullptr;
	m_pTargetTileMap = nullptr;
    m_vecCopyTileMap.clear();
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
    int iTileMapSize[2] = { m_pTargetTileMap->GetTileMapSize().x, m_pTargetTileMap->GetTileMapSize().y };
    static Vec2 iNewTileMapSize = Vec2{ iTileMapSize[0],iTileMapSize[1] };

    ImGui::Text("TileMapSize");
    ImGui::SameLine();
    // 타일맵의 타일 수를 표시하고 편집할 Input 창을 띄운다.
    if (ImGui::InputInt2("##TileMapSize", iTileMapSize, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        // 수정하고 Enter를 클릭하면 Modal창을 띄워 경고를 해준다,
        iNewTileMapSize = Vec2{ iTileMapSize[0],iTileMapSize[1] };
        if (!ImGui::IsPopupOpen("Warning"))
            ImGui::OpenPopup("Warning");
    }
    // 타일맵 사이즈 조절하면 띄워주는 경고 모달창
    if (ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text(u8"타일맵 사이즈를 재조정하면 모든 타일 정보가 삭제됩니다.\n 그래도 타일맵 사이즈를 재조정 하겠습니까?");
        float item_height = ImGui::GetTextLineHeightWithSpacing();

        // Yes -> 수정된 타일맵 반영
        ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
        if (ImGui::Button("Yes", button_size))
        {
            m_pTargetTileMap->SetTileMapSize(iNewTileMapSize);
            iNewTileMapSize = Vec2{ 0.f, 0.f };

            // 세팅 초기화
            SetTargetObject(m_pTargetGameObject);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        // No -> 모달 창 종료
        if (ImGui::Button("No", button_size))
        {
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
    ImVec2 canvas_sz = { 500.f, 500.f };// ImGui::GetContentRegionAvail();
    if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
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

    for (int i = 0; i < iTileMapSize[1]; i++)
    {
        for (int j = 0; j < iTileMapSize[0]; j++)
        {
            // 그릴 위치 찾기
            ImVec2 vLP = ImVec2{ origin.x + zoomed_tilesize.x * j, origin.y + zoomed_tilesize.y * i };
            ImVec2 vRB = ImVec2{ vLP.x + zoomed_tilesize.x, vLP.y + zoomed_tilesize.y };

            // 타일 데이터 가져오기
            tTileData tCurTile = m_vecCopyTileMap[j + i * iTileMapSize[0]];

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
    const ImVec2 vZoomedTileMapSize = ImVec2{ iTileMapSize[0] * zoomed_tilesize.x ,iTileMapSize[1] * zoomed_tilesize.y };
    draw_list->AddRect(origin, ImVec2{ origin.x + vZoomedTileMapSize.x,origin.y + vZoomedTileMapSize.y }, IM_COL32(255, 0, 0, 255));


    // 마우스 동작
    if (is_hovered)
    {
        // 마우스 위치가 canvas안 이라면
        if (mouse_pos_in_canvas.x > 0 && mouse_pos_in_canvas.x < vZoomedTileMapSize.x && mouse_pos_in_canvas.y > 0 && mouse_pos_in_canvas.y < vZoomedTileMapSize.y)
        {
            // 마우스 위치에 있는 타일 인덱스 구하기
            int iTile_col = mouse_pos_in_canvas.x / zoomed_tilesize.x;
            int iTile_row = mouse_pos_in_canvas.y / zoomed_tilesize.y;
            int iTileIndex = iTile_col + iTile_row * iTileMapSize[0];

            // 마우스 위치에 있는 타일 하이라이트
            ImVec2 hover_p0 = ImVec2(origin.x + (zoomed_tilesize.x * iTile_col), origin.y + (zoomed_tilesize.y * (iTile_row)));
            ImVec2 hover_p1 = ImVec2(hover_p0.x + zoomed_tilesize.x, hover_p0.y + zoomed_tilesize.y);
            draw_list->AddRectFilled(hover_p0, hover_p1, IM_COL32(120, 120, 120, 120));

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

        // 중앙 클릭 => 이미지 이동
        if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
        {
            offset.x += io.MouseDelta.x;
            offset.y += io.MouseDelta.y;
        }

        // 마우스 휠 => 크기 조정
        // Minimum zoom = x0.1 (near camera limit)
        // Maximum zoom = x5.0 (it can fit an image with 1600px width)
        zoom = zoom - (io.MouseWheel * 0.3f);
        if (zoom < 0.1f)
            zoom = 0.1f;
        else if (zoom > 5.0f)
            zoom = 5.0f;

    }
    draw_list->PopClipRect();

    // Apply, Revert 버튼
    if (ImGui::Button("Apply"))
    {
        int size = m_vecCopyTileMap.size();
        for (int i = 0; i < size; ++i)
        {
            m_pTargetTileMap->SetTileData(i,m_vecCopyTileMap[i].iImgIdx);
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Revert"))
    {
        CopyTileMap(m_pTargetTileMap->GetTilaDataVector());
    }

}

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
    // Ÿ�ϸ��� Ÿ�� �� ��������
    int iTileMapSize[2] = { m_pTargetTileMap->GetTileMapSize().x, m_pTargetTileMap->GetTileMapSize().y };
    static Vec2 iNewTileMapSize = Vec2{ iTileMapSize[0],iTileMapSize[1] };

    ImGui::Text("TileMapSize");
    ImGui::SameLine();
    // Ÿ�ϸ��� Ÿ�� ���� ǥ���ϰ� ������ Input â�� ����.
    if (ImGui::InputInt2("##TileMapSize", iTileMapSize, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        // �����ϰ� Enter�� Ŭ���ϸ� Modalâ�� ��� ��� ���ش�,
        iNewTileMapSize = Vec2{ iTileMapSize[0],iTileMapSize[1] };
        if (!ImGui::IsPopupOpen("Warning"))
            ImGui::OpenPopup("Warning");
    }
    // Ÿ�ϸ� ������ �����ϸ� ����ִ� ��� ���â
    if (ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text(u8"Ÿ�ϸ� ����� �������ϸ� ��� Ÿ�� ������ �����˴ϴ�.\n �׷��� Ÿ�ϸ� ����� ������ �ϰڽ��ϱ�?");
        float item_height = ImGui::GetTextLineHeightWithSpacing();

        // Yes -> ������ Ÿ�ϸ� �ݿ�
        ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
        if (ImGui::Button("Yes", button_size))
        {
            m_pTargetTileMap->SetTileMapSize(iNewTileMapSize);
            iNewTileMapSize = Vec2{ 0.f, 0.f };

            // ���� �ʱ�ȭ
            SetTargetObject(m_pTargetGameObject);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        // No -> ��� â ����
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

    // 1. ĵ���� �׸���
    // ĵ������ �׸��带 ǥ������ üũ
    ImGui::Checkbox("Enable grid", &opt_enable_grid);

    // ĵ���� ��ġ, ũ�� ���
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
    ImVec2 canvas_sz = { 500.f, 500.f };// ImGui::GetContentRegionAvail();
    if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // ĵ���� Outline�� �׸��� ������ ä���. 
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

    // ĵ������ ������� ���콺 �Է��� �����ϵ��� ��ư �߰�
    ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);

    // ���콺 ���� ���� ������
    const bool is_hovered = ImGui::IsItemHovered(); // Hovered
    const bool is_active = ImGui::IsItemActive();   // Held
    const ImVec2 origin(canvas_p0.x + offset.x, canvas_p0.y + offset.y);
    const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);
    const ImVec2 zoomed_tilesize = ImVec2{ m_pTargetTileMap->GetTileSize().x / zoom,m_pTargetTileMap->GetTileSize().y / zoom };

    // �׸��� �׸���
    draw_list->PushClipRect(canvas_p0, canvas_p1, true);
    if (opt_enable_grid)
    {
        // �׸��� ũ��
        const float GRID_STEP = zoomed_tilesize.x;

        // ���μ� �׸���
        for (float x = fmodf(offset.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));

        // ���μ� �׸���
        for (float y = fmodf(offset.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
    }

    // Ÿ�ϸ� �׸���
    CTexture* pAtlasTexture = m_pTargetTileMap->GetAtlasTex();

    for (int i = 0; i < iTileMapSize[1]; i++)
    {
        for (int j = 0; j < iTileMapSize[0]; j++)
        {
            // �׸� ��ġ ã��
            ImVec2 vLP = ImVec2{ origin.x + zoomed_tilesize.x * j, origin.y + zoomed_tilesize.y * i };
            ImVec2 vRB = ImVec2{ vLP.x + zoomed_tilesize.x, vLP.y + zoomed_tilesize.y };

            // Ÿ�� ������ ��������
            tTileData tCurTile = m_vecCopyTileMap[j + i * iTileMapSize[0]];

            // Ÿ���� �̹��� �ε����� -1�̸� �׸��� �ʴ´�.
            if (tCurTile.iImgIdx != -1)
            {
                // Right-Bottom UV ����ϱ�
                ImVec2 vRBUV = ImVec2{ tCurTile.vLTUV.x + m_pTargetTileMap->GetTileSliceUV().x, tCurTile.vLTUV.y + m_pTargetTileMap->GetTileSliceUV().y };

                // Ÿ�� �׸���
                draw_list->AddImage(pAtlasTexture->GetSRV().Get(), vLP, vRB, tCurTile.vLTUV, vRBUV);
            }
        }
    }

    // TileMap ���� ǥ��
    const ImVec2 vZoomedTileMapSize = ImVec2{ iTileMapSize[0] * zoomed_tilesize.x ,iTileMapSize[1] * zoomed_tilesize.y };
    draw_list->AddRect(origin, ImVec2{ origin.x + vZoomedTileMapSize.x,origin.y + vZoomedTileMapSize.y }, IM_COL32(255, 0, 0, 255));


    // ���콺 ����
    if (is_hovered)
    {
        // ���콺 ��ġ�� canvas�� �̶��
        if (mouse_pos_in_canvas.x > 0 && mouse_pos_in_canvas.x < vZoomedTileMapSize.x && mouse_pos_in_canvas.y > 0 && mouse_pos_in_canvas.y < vZoomedTileMapSize.y)
        {
            // ���콺 ��ġ�� �ִ� Ÿ�� �ε��� ���ϱ�
            int iTile_col = mouse_pos_in_canvas.x / zoomed_tilesize.x;
            int iTile_row = mouse_pos_in_canvas.y / zoomed_tilesize.y;
            int iTileIndex = iTile_col + iTile_row * iTileMapSize[0];

            // ���콺 ��ġ�� �ִ� Ÿ�� ���̶���Ʈ
            ImVec2 hover_p0 = ImVec2(origin.x + (zoomed_tilesize.x * iTile_col), origin.y + (zoomed_tilesize.y * (iTile_row)));
            ImVec2 hover_p1 = ImVec2(hover_p0.x + zoomed_tilesize.x, hover_p0.y + zoomed_tilesize.y);
            draw_list->AddRectFilled(hover_p0, hover_p1, IM_COL32(120, 120, 120, 120));

            // �����Ϳ��� ���õ� Ÿ�� ��������
            int iSelectedIdx = m_pLSEditor->GetCurTileIdx();

            // ��Ŭ�� => Ÿ�� �׸���
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


            // ��Ŭ�� => Ÿ�� �����, 
            else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) ||
                ImGui::IsMouseDragging(ImGuiMouseButton_Right))
            {
                m_vecCopyTileMap[iTileIndex].iImgIdx = -1;
            }

        }

        // �߾� Ŭ�� => �̹��� �̵�
        if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
        {
            offset.x += io.MouseDelta.x;
            offset.y += io.MouseDelta.y;
        }

        // ���콺 �� => ũ�� ����
        // Minimum zoom = x0.1 (near camera limit)
        // Maximum zoom = x5.0 (it can fit an image with 1600px width)
        zoom = zoom - (io.MouseWheel * 0.3f);
        if (zoom < 0.1f)
            zoom = 0.1f;
        else if (zoom > 5.0f)
            zoom = 5.0f;

    }
    draw_list->PopClipRect();

    // Apply, Revert ��ư
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

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
    // Ÿ�� ���� ������Ʈ�� TileMap ������Ʈ�� ���� ������Ʈ�����Ѵ�.
    assert(_pObj->TileMap());

    // �ʱ�ȭ
    Clear();

    // ��� ���� �ٽ� ����
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
    // Ÿ�ϸ��� Ÿ�� �� ��������
    static Vec2 iNewTileMapSize;
    int iTileMapSizeBuffer[2] = { m_vecCopyTileMapSize.x, m_vecCopyTileMapSize.y };

    ImGui::Text("TileMapSize");
    ImGui::SameLine();
    // Ÿ�ϸ��� Ÿ�� ���� ǥ���ϰ� ������ Input â�� ����.
    if (ImGui::InputInt2("##TileMapSize", iTileMapSizeBuffer, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        // �����ϰ� Enter�� Ŭ���ϸ� Modalâ�� ��� ��� ���ش�,
        iNewTileMapSize = Vec2{ iTileMapSizeBuffer[0],iTileMapSizeBuffer[1] };

        if (!ImGui::IsPopupOpen("Warning"))
            ImGui::OpenPopup("Warning");
    }
    // Ÿ�ϸ� ������ �����ϸ� ����ִ� ��� ���â
    if (ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text(u8"Ÿ�ϸ� ����� �������ϸ� ���� Ÿ�ϸ� ������ ������Ʈ�� ����˴ϴ�.\n ���� ������ �� ������ �����͸� ������ �� ���� �� �ֽ��ϴ�.\n �׷��� Ÿ�ϸ� ����� ������ �ϰڽ��ϱ�?");

        float item_height = ImGui::GetTextLineHeightWithSpacing();

        // Yes -> ������ Ÿ�ϸ� �ݿ�
        ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
        if (ImGui::Button("Yes", button_size))
        {
            // ���� Ÿ�� �����͸� �����صд�.
            vector<vector<tTileData>> vecTmpTileMap;
            vecTmpTileMap.resize((size_t)m_vecCopyTileMapSize.y, vector<tTileData>((size_t)m_vecCopyTileMapSize.x, tTileData{}));

            for (int i = 0; i < m_vecCopyTileMapSize.y; i++)
            {
                for (int j = 0; j < m_vecCopyTileMapSize.x; j++)
                {
                    vecTmpTileMap[i][j] = m_vecCopyTileMap[m_vecCopyTileMapSize.x * i + j];
                }
            }

            // Ÿ�� �ʱ�ȭ
            m_vecCopyTileMap.clear();

            // ����ص� Ÿ���� �ٽ� �����Ѵ�
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

            // ����
            m_pTargetTileMap->SetTileMapSize(iNewTileMapSize);
            Apply();
            SetTargetObject(m_pTargetGameObject);

            // ���� �ʱ�ȭ
            iNewTileMapSize = Vec2{ 0.f, 0.f };
            vecTmpTileMap.clear();

            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        // No -> ��� â ����
        if (ImGui::Button("No", button_size))
        {
            // ���� �ʱ�ȭ
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

    // 1. ĵ���� �׸���
    // ĵ������ �׸��带 ǥ������ üũ
    ImGui::Checkbox("Enable grid", &opt_enable_grid);

    // ĵ���� ��ġ, ũ�� ���
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    canvas_sz.y -= 25.f;
    if (canvas_sz.x < 80.0f) canvas_sz.x = 80.0f;
    if (canvas_sz.y < 80.0f) canvas_sz.y = 80.0f;
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
    if(nullptr != pAtlasTexture)
		for (int i = 0; i < iTileMapSizeBuffer[1]; i++)
		{
			for (int j = 0; j < iTileMapSizeBuffer[0]; j++)
			{
				// �׸� ��ġ ã��
				ImVec2 vLP = ImVec2{ origin.x + zoomed_tilesize.x * j, origin.y + zoomed_tilesize.y * i };
				ImVec2 vRB = ImVec2{ vLP.x + zoomed_tilesize.x, vLP.y + zoomed_tilesize.y };

				// Ÿ�� ������ ��������
				tTileData tCurTile = m_vecCopyTileMap[j + i * iTileMapSizeBuffer[0]];

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
    const ImVec2 vZoomedTileMapSize = ImVec2{ iTileMapSizeBuffer[0] * zoomed_tilesize.x ,iTileMapSizeBuffer[1] * zoomed_tilesize.y };
    draw_list->AddRect(origin, ImVec2{ origin.x + vZoomedTileMapSize.x,origin.y + vZoomedTileMapSize.y }, IM_COL32(255, 0, 0, 255));


    // collider �׸���� ����
    static int rect[2] = { -1, -1 };

    // canvas �ۿ��� Ŭ���ϸ� �ʱ�ȭ
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

        // ũ�� ���
        m_vOffsetScale = Vec2{ (endTile_col - startTile_col + 1),(endTile_row - startTile_row + 1) };
        m_vOffsetScale *= vRelativeTileSize;

        // ��ġ ����� �»�� �������� �Ѵ�
        m_vOffsetPos = Vec2{ -m_pTargetTileMap->Transform()->GetRelativeScale().x / 2, m_pTargetTileMap->Transform()->GetRelativeScale().y / 2 };
        m_vOffsetPos += Vec2{ m_vOffsetScale.x / 2, -m_vOffsetScale.y / 2 };
        m_vOffsetPos.x += startTile_col * vRelativeTileSize.x;
        m_vOffsetPos.y -= startTile_row * vRelativeTileSize.y;
        //m_vOffsetPos.x += (startTile_col + (endTile_col - startTile_col / 2)) * vRelativeTileSize.x;
        //m_vOffsetPos.y += (startTile_row + (endTile_row - startTile_row / 2)) * vRelativeTileSize.y;
    }


    // ���콺 ����
    if (is_hovered)
    {
        // ���콺 ��ġ�� canvas�� �̶��
        if (mouse_pos_in_canvas.x > 0 && mouse_pos_in_canvas.x < vZoomedTileMapSize.x && mouse_pos_in_canvas.y > 0 && mouse_pos_in_canvas.y < vZoomedTileMapSize.y)
        {
            // ���콺 ��ġ�� �ִ� Ÿ�� �ε��� ���ϱ�
            int iTile_col = mouse_pos_in_canvas.x / zoomed_tilesize.x;
            int iTile_row = mouse_pos_in_canvas.y / zoomed_tilesize.y;
            int iTileIndex = iTile_col + iTile_row * iTileMapSizeBuffer[0];

            // ���콺 ��ġ�� �ִ� Ÿ�� ���̶���Ʈ
            ImVec2 hover_p0 = ImVec2(origin.x + (zoomed_tilesize.x * iTile_col), origin.y + (zoomed_tilesize.y * (iTile_row)));
            ImVec2 hover_p1 = ImVec2(hover_p0.x + zoomed_tilesize.x, hover_p0.y + zoomed_tilesize.y);
            draw_list->AddRectFilled(hover_p0, hover_p1, IM_COL32(120, 120, 120, 120));

            // LandScape Editor�� Ÿ�Կ� ���� Ŭ���� ������ �޶�����.
            LS_EDIT_TYPE EditType = m_pLSEditor->GetType();
            switch (EditType)
            {
            case LS_EDIT_TYPE::TILE_MAP:
                DrawTile(iTileIndex);
                break;
            case LS_EDIT_TYPE::COLLIDER:
            {
                DrawCollider();
                // �׸� �׸���
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


        // �߾� Ŭ�� => �̹��� �̵�
        if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
        {
            offset.x += io.MouseDelta.x;
            offset.y += io.MouseDelta.y;
        }

        // ���콺 �� => ũ�� ����
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
        // Apply, Revert ��ư
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
        // Apply, Revert ��ư
        if (ImGui::Button("ADD"))
        {
            Add();
        }
    }


}
void TileMapEditor::DrawTile(int iTileIndex)
{
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

void TileMapEditor::DrawCollider()
{
    for (size_t i = 0; i < m_vecChildCol.size(); i++)
    {
        // offset pos�� size ��������
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

    // SceneOutliner ����
    SceneOutliner* pUI = (SceneOutliner*)CImGuiMgr::GetInst()->FindUI("SceneOutliner");
    pUI->Reset();
}

void TileMapEditor::AddPrefab(int iTileIndex)
{
}
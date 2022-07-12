#include "pch.h"
#include "Collider2DUI.h"

#include <Engine/CCollider2D.h>

Collider2DUI::Collider2DUI()
	: ComponentUI("Collider2D", COMPONENT_TYPE::COLLIDER2D)
{
    SetSize(Vec2(0.f, 120.f));
}

Collider2DUI::~Collider2DUI()
{
}

void Collider2DUI::update()
{
	ComponentUI::update();
}

void Collider2DUI::render_update()
{
    ComponentUI::render_update();

    CCollider2D* pCol2D = GetTargetObject()->Collider2D();

    // Collider Type
    ImGui::Text("type");
    ImGui::SameLine(100);

    COLLIDER2D_TYPE eColType = pCol2D->GetCollider2DType();
    const char* szColType[2] = { "Box", "Circle" };

    if (ImGui::BeginCombo("##Col2DType", szColType[(int)eColType]))
    {
        for (int i = 0; i < 2; i++)
        {
            if (ImGui::Selectable(szColType[i]))
            {
                pCol2D->SetCollider2DType((COLLIDER2D_TYPE)i);
            }
        }
        ImGui::EndCombo();
    }

    // offset
    ImGui::Text("offset pos");
    ImGui::SameLine(100);

    float vOffset[2] = { pCol2D->GetOffsetPos().x, pCol2D->GetOffsetPos().y };
    if (ImGui::InputFloat2("##OffsetPosCol2D", vOffset))
    {
        pCol2D->SetOffsetPos(vOffset[0], vOffset[1]);
    }

    ImGui::Text("offset scale");
    ImGui::SameLine(100);

    vOffset[0] = pCol2D->GetOffsetScale().x;
    vOffset[1] = pCol2D->GetOffsetScale().y;

    if (ImGui::InputFloat2("##OffsetScaleCol2D", vOffset))
    {
        pCol2D->SetOffsetScale(vOffset[0], vOffset[1]);
    }
}
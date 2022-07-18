#include "pch.h"
#include "Collider2DUI.h"

#include <Engine/CCollider2D.h>
#include <Engine/CCollisionMgr.h>
#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CLayer.h>

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

	// 충돌 체크 박스
	if (ImGui::CollapsingHeader("Check Collision", ImGuiTreeNodeFlags_None))
	{
		SetSize(Vec2(0.f, 420.f));

		const UINT* iCollisionCheckArr = CCollisionMgr::GetInst()->GetCollisionArr();
		int maxLayer = 0;

		ImGui::Dummy(ImVec2{ 100.f, 0.f });
		ImGui::SameLine();

		for (int i = 0; i < MAX_LAYER; ++i)
		{
			string strLayer = ToString(CSceneMgr::GetInst()->GetCurScene()->GetLayer(i)->GetName());
			maxLayer = i - 1;

			// 해당 레이어의 이름이 없으면 이름을 적지 않는다
			if ("" == strLayer)
				break;
		}

		for (int i = maxLayer; i >= 0; --i)
		{
			ImGui::Text(("    " + std::to_string(i)).c_str());
			ImGui::SameLine();
		}


		for (size_t i = 0; i <= maxLayer; ++i)
		{
			string strLayer = std::to_string(i) + " : " + ToString(CSceneMgr::GetInst()->GetCurScene()->GetLayer(i)->GetName());

			// 해당 레이어의 이름이 없으면 체크 박스를 생성하지 않는다
			if ("" == strLayer)
				break;

			ImGui::NewLine();
			ImGui::Text(strLayer.c_str());
			ImGui::SameLine(120);

			for (int j = 0; j <= maxLayer - i; ++j)
			{
				bool bCollision = iCollisionCheckArr[i] & (1 << maxLayer - j);

				const string name = "##layer_" + std::to_string(i) + "_" + std::to_string(maxLayer - j);

				if (ImGui::Checkbox(name.c_str(), &bCollision))
				{
					if (bCollision)
						CCollisionMgr::GetInst()->CollisionCheck(i, maxLayer - j);
					else
						CCollisionMgr::GetInst()->CollisionOff(i, maxLayer - j);
				}
				ImGui::SameLine();
			}
		}
	}
	else
	{
		SetSize(Vec2(0.f, 120.f));
	}
}
#include "pch.h"
#include "Light2DUI.h"

#include <Engine\CLight2D.h>
#include <string.h>
Light2DUI::Light2DUI()
	: ComponentUI("Light2D", COMPONENT_TYPE::LIGHT2D)
	, m_bOpenPopup(false)
	, m_eSelcetLightType(LIGHT_COLOR_TYPE::END)
{
	SetSize(Vec2(0.f, 180.f));
}

Light2DUI::~Light2DUI()
{
}


void Light2DUI::update()
{
	ComponentUI::update();
}

void Light2DUI::render_update()
{
	ComponentUI::render_update();

	CLight2D* pTargetLight = GetTargetObject()->Light2D();

	// Light Type
	string strType[3] = { "DIRECTIONAL", "POINT", "SPOT" };
	ImGui::Text("Light Type ");
	ImGui::SameLine();
	if (ImGui::BeginCombo("##LightType", strType[pTargetLight->GetLightInfo().iLightType].c_str()))
	{
		for (int i = 0; i < 3; i++)
		{
			if (ImGui::Selectable(strType[i].c_str()))
			{
				pTargetLight->SetLightType((LIGHT_TYPE)i);
			}
		}
		ImGui::EndCombo();
	}

	// Light Indxe
	ImGui::Text("Light Index ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(pTargetLight->GetLightIdx()).c_str());

	// Light Color
	ImVec4 vColors[(int)LIGHT_COLOR_TYPE::END];

	vColors[(int)LIGHT_COLOR_TYPE::DIFFUSE] =
	{ pTargetLight->GetLightInfo().color.vDiff.x
	 , pTargetLight->GetLightInfo().color.vDiff.y
	 , pTargetLight->GetLightInfo().color.vDiff.z
	 , pTargetLight->GetLightInfo().color.vDiff.w };

	vColors[(int)LIGHT_COLOR_TYPE::SPECULAR] =
	{ pTargetLight->GetLightInfo().color.vSpec.x
	 , pTargetLight->GetLightInfo().color.vSpec.y
	 , pTargetLight->GetLightInfo().color.vSpec.z
	 , pTargetLight->GetLightInfo().color.vSpec.w };

	vColors[(int)LIGHT_COLOR_TYPE::AMBIENT] =
	{ pTargetLight->GetLightInfo().color.vAmb.x
	 , pTargetLight->GetLightInfo().color.vAmb.y
	 , pTargetLight->GetLightInfo().color.vAmb.z
	 , pTargetLight->GetLightInfo().color.vAmb.w };

	ImGui::Text("Color");
	ImGui::Text("Diffuse");
	ImGui::SameLine(50);

	if (ImGui::ColorButton("##DiffColor", vColors[(int)LIGHT_COLOR_TYPE::DIFFUSE], ImGuiColorEditFlags_DisplayHSV, ImVec2(15, 15)))
	{
		// ColorPicker È­¸é ¶ç¿î´Ù
		if (!m_bOpenPopup)
		{
			m_bOpenPopup = true;
			m_eSelcetLightType = LIGHT_COLOR_TYPE::DIFFUSE;
		}
	}

	ImGui::SameLine(100);
	ImGui::Text("Specular");
	ImGui::SameLine(155);
	if (ImGui::ColorButton("##SpecColor", vColors[(int)LIGHT_COLOR_TYPE::SPECULAR], ImGuiColorEditFlags_DisplayHSV, ImVec2(15, 15)))
	{
		if (!m_bOpenPopup)
		{
			m_bOpenPopup = true;
			m_eSelcetLightType = LIGHT_COLOR_TYPE::SPECULAR;
		}
	}

	ImGui::SameLine(200);
	ImGui::Text("Ambient");
	ImGui::SameLine(255);
	if(ImGui::ColorButton("##AmbColor", vColors[(int)LIGHT_COLOR_TYPE::AMBIENT], ImGuiColorEditFlags_DisplayHSV, ImVec2(15, 15)))
	{
		if (!m_bOpenPopup)
		{
			m_bOpenPopup = true;
			m_eSelcetLightType = LIGHT_COLOR_TYPE::AMBIENT;
		}
	}

	if (m_bOpenPopup)
	{
		if (m_eSelcetLightType != LIGHT_COLOR_TYPE::END)
		{
			OpenColorPicker(vColors[(int)m_eSelcetLightType]);

			switch (m_eSelcetLightType)
			{
			case LIGHT_COLOR_TYPE::DIFFUSE:
				pTargetLight->SetDiffuse(Vec3{ vColors[(int)m_eSelcetLightType].x, vColors[(int)m_eSelcetLightType].y, vColors[(int)m_eSelcetLightType].z });
				break;
			case LIGHT_COLOR_TYPE::SPECULAR:				
				pTargetLight->SetSpecular(Vec3{ vColors[(int)m_eSelcetLightType].x, vColors[(int)m_eSelcetLightType].y, vColors[(int)m_eSelcetLightType].z });
				break;
			case LIGHT_COLOR_TYPE::AMBIENT:				
				pTargetLight->SetAmbient(Vec3{ vColors[(int)m_eSelcetLightType].x, vColors[(int)m_eSelcetLightType].y, vColors[(int)m_eSelcetLightType].z });
				break;
			}

		}
	}

	// LightRange
	float fRange = pTargetLight->GetLightInfo().fRange;
	ImGui::SetNextItemWidth(280);
	if (ImGui::DragFloat("##LightRange", &fRange, 0.5, 0, 1000, "%0.3f", ImGuiSliderFlags_AlwaysClamp))
	{
		pTargetLight->SetRange(fRange);
	}

	// LightAngle
	float fAngle = pTargetLight->GetLightInfo().fAngle;
	ImGui::SetNextItemWidth(280);
	if (ImGui::DragFloat("##LightAngle", &fAngle, 0.5, 0, 360, "%0.3f", ImGuiSliderFlags_AlwaysClamp))
	{
		pTargetLight->SetAngle(fAngle);
	}

	// LightDir
	float vDir[3] = { pTargetLight->GetLightInfo().vLightDir.x
					, pTargetLight->GetLightInfo().vLightDir.y
					, pTargetLight->GetLightInfo().vLightDir.z };
	ImGui::SetNextItemWidth(280);
	if (ImGui::DragFloat3("##LightDir", vDir, 0.1, -1, 1, "%0.3f", ImGuiSliderFlags_AlwaysClamp))
	{
		pTargetLight->SetLightDir(Vec3{ vDir[0],vDir[1],vDir[2] });
	}

}

void Light2DUI::OpenColorPicker(ImVec4& _color)
{
	static ImVec4 backup_color = _color;

	static bool saved_palette_init = true;
	static ImVec4 saved_palette[32] = {};
	if (saved_palette_init)
	{
		for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
		{
			ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
				saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
			saved_palette[n].w = 1.0f; // Alpha
		}
		saved_palette_init = false;
	}

	if (m_bOpenPopup)
	{
		ImGui::OpenPopup("mypicker");
		backup_color = _color;
	}
	if (ImGui::BeginPopup("mypicker"))
	{
		ImGui::Text("MY CUSTOM COLOR PICKER WITH AN AMAZING PALETTE!");
		ImGui::Separator();
		ImGui::ColorPicker4("##picker", (float*)&_color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
		ImGui::SameLine();

		ImGui::BeginGroup(); // Lock X position
		ImGui::Text("Current");
		ImGui::ColorButton("##current", _color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
		ImGui::Text("Previous");
		if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
			_color = backup_color;
		ImGui::Separator();
		ImGui::Text("Palette");
		for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
		{
			ImGui::PushID(n);
			if ((n % 8) != 0)
				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

			ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
			if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
				_color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, _color.w); // Preserve alpha!

			// Allow user to drop colors into each palette entry. Note that ColorButton() is already a
			// drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
					memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
					memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
				ImGui::EndDragDropTarget();
			}

			ImGui::PopID();
		}
		if (ImGui::Button("EXIT"))
		{
			m_bOpenPopup = false;
			m_eSelcetLightType = LIGHT_COLOR_TYPE::END;
		}
		ImGui::EndGroup();
		ImGui::EndPopup();
	}

}

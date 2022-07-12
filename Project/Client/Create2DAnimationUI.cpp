#include "pch.h"
#include "Create2DAnimationUI.h"


#include <Engine/CResMgr.h>
#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>
#include <Engine/CGameObject.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CTimeMgr.h>

#include "ListUI.h"
#include "CImGuiMgr.h"

Create2DAnimationUI::Create2DAnimationUI() :
	UI("Create 2D Animation UI"),
	m_vScroll(Vec2(0.f, 0.f)),
	m_gridScroll(Vec2(0.f, 0.f)),
	m_ChangeRectSize(Vec2(0.f, 0.f)),
	m_vTexResolution(Vec2(0.f, 0.f)),
	m_pAtlasTex(nullptr),
	m_SampleUV{},
	m_animName{},
	m_fDuration(0.1f),
	m_fStride(130.f),
	m_iCheckIdx(0),
	m_iRepeatSaveCount(0),
	m_iImgProduct(1),
	m_bIsCheckPlay(false),
	m_bCheckWindow(true),
	m_bIsRepeat(true),
	m_strCheckButton{}
{
	m_animName = "Write New 2D Animation Key!";
	
	// tmp
	m_pAtlasTex = CResMgr::GetInst()->Load<CTexture>(L"texture\\link_0.png", L"texture\\link_0.png").Get();
	m_vTexResolution = Vec2(m_pAtlasTex->Width() + m_fImgZoom, m_pAtlasTex->Height() + m_fImgZoom);
}

Create2DAnimationUI::~Create2DAnimationUI()
{
}


void Create2DAnimationUI::update()
{

}

void Create2DAnimationUI::render_update()
{
	//string _texName = string(m_pTex->GetName().begin(), m_pTex->GetName().end());

	ImGui::Dummy(ImVec2(0.f, 15.f));
	ImGui::Dummy(ImVec2(15.f, 0.f));
	ImGui::SameLine();

	static ImDrawList* draw_list_1 = ImGui::GetWindowDrawList();
	static ImVector<ImVec2> points; //Rect생성용 좌표 2개
	ImGuiIO GetIO = ImGui::GetIO(); //화면 전체 기준으로 한 마우스좌표. 이 값을 static로 하면 변경 X


	ImGui::BeginChild("##AtlasCanvas_1", ImVec2(450.f, 450.f), true, ImGuiWindowFlags_NoScrollbar);


	ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
	ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // BeginChild때 설정해준 UI 사이즈.
	ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
	static bool adding_Rect = false;

	// Atals 이미지 선택
	ImGui::Text("Atlas Texture Key");
	ImGui::SameLine();
	ImGui::InputText("##TexKey", (char*)ToString(m_pAtlasTex->GetKey()).c_str(), 256, ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	if (ImGui::Button("##TexKeyListBtn", ImVec2(15, 15)))
	{
		// Texture들을 가져온다
		const map<wstring, CRes*>& mapRes = CResMgr::GetInst()->GetResList(RES_TYPE::TEXTURE);
		// ListUI 활성화한다.
		ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
		pListUI->Clear();
		pListUI->SetTitle("Texture List");
		// List Item 추가
		for (const auto& pair : mapRes)
		{
			pListUI->AddList(string(pair.first.begin(), pair.first.end()));
		}
		// List 활성화
		pListUI->Activate();
		// 더블클릭 이벤트 등록
		pListUI->SetDBCEvent(this, (DBCLKED)&Create2DAnimationUI::TextureSelect);
	}


	if (ImGui::Button("Create Frames auto"))
	{
		if (m_pAtlasTex == nullptr)
			return;

		if (m_pAtlasTex != nullptr)
		{
			// 현재 Atlas에 해당하는 XML 파일이 있는지 확인한다. 
			wstring xmlKey = m_pAtlasTex->GetKey();
			size_t pos = xmlKey.find_last_of(L".");
			assert(pos != xmlKey.npos);
			xmlKey = xmlKey.substr(0, pos);
			xmlKey += L".xml";

			// XML Map을 가져온다
			const map<wstring, CRes*> mapRes = CResMgr::GetInst()->GetResList(RES_TYPE::XMLData);

			// XML이 있다면 해당 XML이 가진 Sprite ID들을 띄워준다
			map<wstring, CRes*>::const_iterator iter = mapRes.find(xmlKey);
			if (iter != mapRes.end())
			{
				CXMLData* data = (CXMLData*)iter->second;
				const map<wstring, tSprite> sprites = data->GetSpriteDatas();

				for (const auto& pair : sprites)
				{
					wstring spriteName = pair.first;
					size_t pos = spriteName.find_last_of(L".");
					assert(pos != spriteName.npos);
					spriteName = spriteName.substr(0, pos - 2);

					static wstring name = spriteName;
					
					if (name == spriteName)
					{
						const tSprite* sprite = &pair.second;
						m_SampleUV[0] = sprite->vPos / m_vTexResolution;
						m_SampleUV[2] = sprite->vSize / m_vTexResolution;
						m_SampleUV[1] = m_SampleUV[0] + m_SampleUV[2];

						tAnim2DFrame m_t2DFrm = {};
						m_t2DFrm.vLT = m_SampleUV[0];
						m_t2DFrm.vBackground = m_SampleUV[2];  //Rect의 RB - LT 해준 값(추출영역의 크기)
						m_t2DFrm.vSlice = m_SampleUV[2];	   //Rect의 RB - LT 해준 값(추출영역의 크기)
						m_t2DFrm.fDuration = m_fDuration;
						m_t2DFrm.vOffset = Vec2(0.f, 0.f);
						m_vecAnim2DInfo.push_back(m_t2DFrm);

						Preview_Info m_preview = {};
						m_preview.vOriginSize = ImVec2(m_SampleUV[2].x * m_pAtlasTex->Width(), m_SampleUV[2].y * m_pAtlasTex->Height());
						m_preview.vSize = m_preview.vOriginSize;
						m_preview.vOriginLT = V2ToImV2(m_SampleUV[0]);
						m_preview.vLT = m_preview.vOriginLT;            //LeftTop
						m_preview.vOriginRB = V2ToImV2(m_SampleUV[1]);
						m_preview.vRB = m_preview.vOriginRB;            //RightBottom
						m_preview.vSlice = V2ToImV2(m_SampleUV[2]);     //RightBottom - LeftTop = Rect Size
						SetCheckDummy(m_preview);
						m_vecPreviewInfo.push_back(m_preview);
					}

					// Frame 이름이 달라지면 Save
					else
					{
						CreateAnimation(name);
						name = spriteName;
					}
				}

				wstring spriteName = sprites.rbegin()->first;
				size_t pos = spriteName.find_last_of(L".");
				assert(pos != spriteName.npos);
				spriteName = spriteName.substr(0, pos - 2);

				CreateAnimation(spriteName);
			}
		}
	}


	static ImVec2 offset = { 0.f, 0.f };
	static float zoom = 1;

	// Get the ImGui cursor in screen space
	ImVec2 display_p0 = ImGui::GetCursorScreenPos();
	// Get size of the tileset display
	ImVec2 display_sz = ImGui::GetContentRegionAvail();
	display_sz.x = (display_sz.x < 50.0f ? 50.0f : display_sz.x);
	display_sz.y = (display_sz.y < 50.0f ? 50.0f : display_sz.y);
	ImVec2 display_p1 = ImVec2(display_p0.x + display_sz.x, display_p0.y + display_sz.y);
	ImVec2 border_p0 = ImVec2(display_p0.x - 1.f, display_p0.y - 1.f);
	ImVec2 border_p1 = ImVec2(display_p1.x + 1.f, display_p1.y + 1.f);

	// Draw border and background color
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRectFilled(display_p0, display_p1, IM_COL32(50, 50, 50, 255));
	draw_list->AddRect(border_p0, border_p1, IM_COL32(255, 255, 255, 255));

	// Create an invisible button which will catch inputs
	auto display_rect_flags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight |
		ImGuiButtonFlags_MouseButtonMiddle;
	ImGui::InvisibleButton("display", display_sz, display_rect_flags);
	const bool is_hovered = ImGui::IsItemHovered();
	const bool is_active = ImGui::IsItemActive();

	// Calculate origin, then use it to calculate where the mouse cursor is
	ImGuiIO& io = ImGui::GetIO();
	const ImVec2 origin(display_p0.x + offset.x, display_p0.y + offset.y);
	const ImVec2 mouse_pos_in_display(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

	// Pan the tileset display
	if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)) {
		offset.x += io.MouseDelta.x;
		offset.y += io.MouseDelta.y;
	}
	// Zoom into tileset display
	if (is_hovered) {
		// Minimum zoom = x0.1 (near camera limit)
		// Maximum zoom = x5.0 (it can fit an image with 1600px width)
		zoom = zoom - (io.MouseWheel * 0.3f);
		if (zoom < 0.1f)
			zoom = 0.1f;
		else if (zoom > 5.0f)
			zoom = 5.0f;
	}

	// Clip what we're drawing next
	draw_list->PushClipRect(display_p0, display_p1, true);

	// Draw the tileset
	if (nullptr != m_pAtlasTex)
	{
		ImVec2 tileset_p0 = ImVec2(origin.x, origin.y);
		ImVec2 tileset_p1 = ImVec2(origin.x + (m_vTexResolution.x / zoom), origin.y + (m_vTexResolution.y / zoom));
		draw_list->AddImage(m_pAtlasTex->GetSRV().Get(), tileset_p0, tileset_p1, ImVec2(0, 0), ImVec2(1, 1));
	}

	if (L"" == m_strSpriteKey)
	{
		if (is_hovered && !adding_Rect && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			points.clear();
			FloatReset();

			points.push_back(mouse_pos_in_display);
			points.push_back(ImVec2{ 0, 0 });


			m_SampleUV[0].x = (mouse_pos_in_display.x / (m_vTexResolution.x / zoom));
			m_SampleUV[0].y = (mouse_pos_in_display.y / (m_vTexResolution.y / zoom));

			_vCalculValue = Vec2(m_SampleUV[0].x, m_SampleUV[0].y);

			adding_Rect = true;
		}
		if (adding_Rect)
		{
			points.back() = mouse_pos_in_display;

			m_SampleUV[1].x = (mouse_pos_in_display.x / (m_vTexResolution.x / zoom));
			m_SampleUV[1].y = (mouse_pos_in_display.y / (m_vTexResolution.y / zoom));

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				//UV단위
				m_OriginUV[1] = m_SampleUV[1];

				// 프레임의 UV크기가 0.01f 이하이면 그리지 않는다
				m_SampleUV[2].x = m_SampleUV[1].x - m_SampleUV[0].x;
				m_SampleUV[2].y = m_SampleUV[1].y - m_SampleUV[0].y;
				if (m_SampleUV[2].x < 0.01f)
				{
					points.clear();
					FloatReset();
				}
				adding_Rect = false;
			}

		}
	}
	else
	{
		tSprite* data = CResMgr::GetInst()->FindSpriteOffsetInfo(m_pAtlasTex->GetKey(), m_strSpriteKey);

		if (nullptr != data)
		{
			m_SampleUV[0] = data->vPos / m_vTexResolution;
			m_SampleUV[2] = data->vSize / m_vTexResolution;
			m_SampleUV[1] = m_SampleUV[0] + m_SampleUV[2];

			points.resize(2);
		}
	}

	if (points.Size > 1)
	{
		//픽셀단위 렉트영역 좌표값 설정
		m_vDrawRectInfo[0] = Vec2(origin.x + (m_SampleUV[0].x * (m_vTexResolution.x / zoom)), origin.y + (m_SampleUV[0].y *(m_vTexResolution.y / zoom)));
		m_vDrawRectInfo[1] = Vec2(origin.x + (m_SampleUV[1].x * (m_vTexResolution.x / zoom)), origin.y + (m_SampleUV[1].y * (m_vTexResolution.y / zoom)));

		draw_list->AddRect(ImVec2(m_vDrawRectInfo[0].x, m_vDrawRectInfo[0].y),
			ImVec2(m_vDrawRectInfo[1].x, m_vDrawRectInfo[1].y),
			IM_COL32(0, 255, 0, 255), 2.0f);

	}

	//추가된 모든 애니메이션의 흰색 렉트영역 표시
	//for (int i = 0; i < m_vecPreviewInfo.size(); ++i)
	//{
	//	draw_list_1->AddRect(
	//		ImVec2(origin.x + m_vecPreviewInfo[i].vLT.x * m_vTexResolution.x, origin.y + m_vecPreviewInfo[i].vLT.y * m_vTexResolution.y),
	//		ImVec2(origin.x + m_vecPreviewInfo[i].vSlice.x * m_vTexResolution.x, origin.y + m_vecPreviewInfo[i].vSlice.y * m_vTexResolution.y),
	//		IM_COL32(255, 255, 255, 255), 2.0f);
	//}

	//추가된 애니메이션의 흰색 렉트영역 표시
	//if (m_vecPreviewInfo.size() > 0)
	//{
	//	draw_list->AddRect(
	//		ImVec2(origin.x + m_vecPreviewInfo[m_iCheckIdx].vLT.x * m_vTexResolution.x,
	//			origin.y + m_vecPreviewInfo[m_iCheckIdx].vLT.y * m_vTexResolution.y),
	//		ImVec2(origin.x + m_vecPreviewInfo[m_iCheckIdx].vRB.x * m_vTexResolution.x,
	//			origin.y + m_vecPreviewInfo[m_iCheckIdx].vRB.y * m_vTexResolution.y),
	//		IM_COL32(255, 255, 255, 255), 2.0f);
	//}


	ImGui::EndChild();


	ImGui::SameLine(490.f, 0.f);
	ImGui::BeginChild("##Create2DAmim_InfoWindow", ImVec2(400.f, 450.f), true, ImGuiWindowFlags_NoScrollbar);

	ImGui::PushItemWidth(250.f);
	ImGui::Dummy(ImVec2(0.f, 5.f));

	static float _canvasMousePos[2];
	_canvasMousePos[0] = mouse_pos_in_display.x;
	_canvasMousePos[1] = mouse_pos_in_display.y;
	ImGui::Text(" Canvas MousePos");
	ImGui::SameLine(m_fStride, 0.f);
	ImGui::DragFloat2("##Canvas Mouse Pos_1", _canvasMousePos, 0.f, 0.f, 0.f, "%.3f", ImGuiSliderFlags_NoInput);

	ImGui::Text(" Sprite Key");
	ImGui::SameLine(m_fStride, 0.f);
	ImGui::InputText("##SpriteKey", (char*)ToString(m_strSpriteKey).c_str(), 256, ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	if (ImGui::Button("##SpriteKeyListBtn", Vec2(15, 15)))
	{
		// XML Map을 가져온다
		const map<wstring, CRes*> mapRes = CResMgr::GetInst()->GetResList(RES_TYPE::XMLData);

		if (m_pAtlasTex != nullptr)
		{
			// 현재 Atlas에 해당하는 XML 파일이 있는지 확인한다. 
			wstring spriteKey = m_pAtlasTex->GetKey();
			size_t pos = spriteKey.find_last_of(L".");
			assert(pos != spriteKey.npos);
			spriteKey = spriteKey.substr(0, pos);
			spriteKey += L".xml";

			// XML이 있다면 해당 XML이 가진 Sprite ID들을 띄워준다
			map<wstring, CRes*>::const_iterator iter = mapRes.find(spriteKey);
			if (iter != mapRes.end())
			{
				// ListUI 활성화한다.
				ListUI* pListUI = (ListUI*)CImGuiMgr::GetInst()->FindUI("##ListUI");
				pListUI->Clear();
				pListUI->SetTitle("Sprite List");

				CXMLData* data = (CXMLData*)iter->second;
				const map<wstring, tSprite> sprites = data->GetSpriteDatas();

				for (const auto& pair : sprites)
				{
					pListUI->AddList(string(pair.first.begin(), pair.first.end()));
				}
				// List 활성화
				pListUI->Activate();
				// 더블클릭 이벤트 등록
				pListUI->SetDBCEvent(this, (DBCLKED)&Create2DAnimationUI::SpriteKeySelect);
			}
		}

	}


	ImGui::PushItemWidth(250.f);
	static float _sampleUV_1[2];
	_sampleUV_1[0] = m_SampleUV[0].x;
	_sampleUV_1[1] = m_SampleUV[0].y;
	ImGui::Text(" Sample UV LT");
	ImGui::SameLine(m_fStride, 0.f);
	ImGui::DragFloat2("##Sample UV LT_1", _sampleUV_1, 0.f, 0.f, 0.f, "%.3f", ImGuiSliderFlags_NoInput);


	static float _sampleUV_2[2];
	_sampleUV_2[0] = m_SampleUV[1].x;
	_sampleUV_2[1] = m_SampleUV[1].y;
	ImGui::Text(" Sample UV RB");
	ImGui::SameLine(m_fStride, 0.f);
	ImGui::DragFloat2("##Sample UV RB_1", _sampleUV_2, 0.f, 0.f, 0.f, "%.3f", ImGuiSliderFlags_NoInput);


	//ImGui::PushItemWidth(80.f);
	ImGui::Text(" Set Rect Distance");
	ImGui::SameLine(m_fStride, 0.f);
	ImGui::DragFloat2("##Set Rect Distance", _RectDist, 0.1f);


	ImGui::Text(" Frame Duration");
	ImGui::SameLine(m_fStride, 0.f);
	ImGui::InputFloat("##Frame Duration_1", &m_fDuration);


	ImGui::Text(" Animation Name");
	ImGui::SameLine(m_fStride, 0.f);
	ImGui::InputText("##Animation Name_1", (char*)m_animName.c_str(), m_animName.capacity());

	ImGui::Text(" Repeat");
	ImGui::SameLine(m_fStride, 0.f);
	ImGui::Checkbox("##Animation Repeat", &m_bIsRepeat);


	ImGui::Text(" Save Button");
	ImGui::SameLine(m_fStride, 0.f);
	if (ImGui::Button("Save 2D Anim Info Button"))
	{
		tAnim2DFrame m_t2DFrm = {};
		m_t2DFrm.vLT = m_SampleUV[0];
		m_t2DFrm.vBackground = m_SampleUV[2];  //Rect의 RB - LT 해준 값(추출영역의 크기)
		m_t2DFrm.vSlice = m_SampleUV[2];	   //Rect의 RB - LT 해준 값(추출영역의 크기)
		m_t2DFrm.fDuration = m_fDuration;
		m_t2DFrm.vOffset = Vec2(0.f, 0.f);
		m_vecAnim2DInfo.push_back(m_t2DFrm);

		Preview_Info m_preview = {};
		m_preview.vOriginSize = ImVec2(m_SampleUV[2].x * m_pAtlasTex->Width(), m_SampleUV[2].y * m_pAtlasTex->Height());
		m_preview.vSize = m_preview.vOriginSize;
		m_preview.vOriginLT = V2ToImV2(m_SampleUV[0]);
		m_preview.vLT = m_preview.vOriginLT;            //LeftTop
		m_preview.vOriginRB = V2ToImV2(m_SampleUV[1]);
		m_preview.vRB = m_preview.vOriginRB;            //RightBottom
		m_preview.vSlice = V2ToImV2(m_SampleUV[2]);     //RightBottom - LeftTop = Rect Size
		SetCheckDummy(m_preview);
		m_vecPreviewInfo.push_back(m_preview);
	}



	ImGui::Text(" Delete Button");
	ImGui::SameLine(m_fStride, 0.f);
	if (ImGui::Button("PopBack 2D Anim Info Button"))
	{
		if (m_vecAnim2DInfo.size() > 0)
		{
			m_vecAnim2DInfo.pop_back();
			m_vecPreviewInfo.pop_back();
			if (m_iCheckIdx > m_vecPreviewInfo.size() - 1) m_iCheckIdx--;
			m_bIsCheckPlay = false;
		}
	}


	ImGui::Text(" Clear Button");
	ImGui::SameLine(m_fStride, 0.f);
	if (ImGui::Button("Clear 2D Anim Info Button"))
	{
		VecClearFunc();
	}


	ImGui::Text(" Create Button");
	ImGui::SameLine(m_fStride, 0.f);
	if (ImGui::Button("Create 2D Animation Info Struct"))
	{
		if (m_vecAnim2DInfo.size() > 0)
		{
			CAnimation2D* pAnim2D = new CAnimation2D;
			//여기서 이 애니메이션이 반복인지 아닌지 결정해서 넘겨주기
			pAnim2D->SetAnimFrame(m_pAtlasTex, m_vecAnim2DInfo);
			pAnim2D->SetRepeat(m_bIsRepeat);

			m_pTargetObj->Animator2D()->AddAnim(m_animName.c_str(), pAnim2D);

			//Delegate : Animator2DUI::UpdateAnimList()
			(m_Inst->*m_AddAnimEvent)((DWORD_PTR)m_animName.c_str());

			VecClearFunc();
		}
	}
	ImGui::EndChild();



	//ImGui::Dummy(ImVec2(0.f, 12.f));
	//ImGui::NewLine();
	//ImGui::SameLine(25.f);
	//ImGui::Text("Preview Window");
	//ImGui::NewLine();
	//ImGui::SameLine(23.f);
	//// ======================================= Preview Window ======================================= //
	//ImGui::BeginChild("##Create2DAmim_PreviewWindow", ImVec2(860.f, 80.f), true);
	//
	//ImVec2 preview_Pos = ImGui::GetCursorScreenPos();
	//ImVec2 preview_Size = ImGui::GetContentRegionAvail();   // BeginChild때 설정해준 UI 사이즈.
	//ImVec2 preview_Psz = ImVec2(preview_Pos.x + preview_Size.x, preview_Pos.y + preview_Size.y);
	//
	//
	//for (int i = 0; i < m_vecPreviewInfo.size(); ++i)
	//{
	//	ImGui::Image(m_pTex->GetSRV().Get(), m_vecPreviewInfo[i].vSize, m_vecPreviewInfo[i].vLT, m_vecPreviewInfo[i].vSlice);
	//	ImGui::SameLine(0.f, 10.f);
	//}
	//
	//draw_list_1->PushClipRect(preview_Pos, preview_Psz, true);
	//const float GRID_STEP = 8.0f;
	//for (float x = fmodf(0.f, GRID_STEP); x < preview_Psz.x; x += GRID_STEP)
	//	draw_list_1->AddLine(ImVec2(preview_Pos.x + x, preview_Pos.y), ImVec2(preview_Pos.x + x, preview_Psz.y), IM_COL32(200, 200, 200, 40));//x축 라인 Add
	//for (float y = fmodf(0.f, GRID_STEP); y < preview_Psz.y; y += GRID_STEP)
	//	draw_list_1->AddLine(ImVec2(preview_Pos.x, preview_Pos.y + y), ImVec2(preview_Psz.x, preview_Pos.y + y), IM_COL32(200, 200, 200, 40));//y축 라인 Add
	//draw_list_1->PopClipRect();
	//
	//ImGui::EndChild();
	// ======================================= Preview Window ======================================= //



	// ======================================= Check Anim Window ======================================= //
	//ImGuiWindowFlags_   2
	ImGui::Begin("Check Animation##1", &m_bCheckWindow, ImGuiWindowFlags_NoResize);
	static ImDrawList* draw_list_2 = ImGui::GetWindowDrawList();//현재 윈도우창의 DrawList 얻어오기

	if (!m_bIsCheckPlay) m_strCheckButton = "Play";
	else m_strCheckButton = "Stop";

	// ======= Check할 애니메이션 이미지 창 ======= //
	ImVec2 _checkSize = ImVec2(m_SampleUV[2].x * m_pAtlasTex->Width(), m_SampleUV[2].y * m_pAtlasTex->Height());
	ImGui::BeginChild("##Check Animation##2", ImVec2(500.f, 500.f), true);
	ImVec2 CheckCanvasPos = ImGui::GetCursorScreenPos();
	CheckCanvasSize = ImGui::GetContentRegionAvail();
	ImVec2 CheckCanvasPsz = ImVec2(CheckCanvasPos.x + CheckCanvasSize.x, CheckCanvasPos.y + CheckCanvasSize.y);


	draw_list_2->PushClipRect(CheckCanvasPos, CheckCanvasPsz, true);
	draw_list_2->AddLine(ImVec2(CheckCanvasPos.x + CheckCanvasSize.x / 2.f, CheckCanvasPos.y),
		ImVec2(CheckCanvasPos.x + CheckCanvasSize.x / 2.f, CheckCanvasPos.y + CheckCanvasSize.y),
		IM_COL32(255, 255, 255, 200));
	draw_list_2->AddLine(ImVec2(CheckCanvasPos.x, CheckCanvasPos.y + CheckCanvasSize.y / 2.f),
		ImVec2(CheckCanvasPos.x + CheckCanvasSize.x, CheckCanvasPos.y + CheckCanvasSize.y / 2.f),
		IM_COL32(255, 255, 255, 200));
	draw_list_2->PopClipRect();


	if (m_vecPreviewInfo.size() > 0)
	{
		ImGui::Dummy(ImVec2(0.f, m_vecPreviewInfo[m_iCheckIdx].vCheckDummy.y));
		ImGui::Dummy(ImVec2(m_vecPreviewInfo[m_iCheckIdx].vCheckDummy.x, 0.f));
		ImGui::SameLine(0.f, 0.f);
		ImVec2 _temp = ImVec2(m_vecPreviewInfo[m_iCheckIdx].vSize.x, m_vecPreviewInfo[m_iCheckIdx].vSize.y);
		ImGui::Image(m_pAtlasTex->GetSRV().Get(), _temp, m_vecPreviewInfo[m_iCheckIdx].vLT, m_vecPreviewInfo[m_iCheckIdx].vRB);
	}

	ImGui::EndChild();
	// ======= Check할 애니메이션 이미지 창 ======= //



	// ======= 애니메이션 넘기기, Play 버튼 ======= //
	//ImGui::Dummy(ImVec2(CheckCanvasSize.x / 2.f - 230.f, 0.f));
	ImGui::Dummy(ImVec2(3.f, 0.f));
	ImGui::SameLine();

	if (ImGui::RadioButton("X1", &m_iImgProduct, 1))
	{
		for (int i = 0; i < m_vecPreviewInfo.size(); ++i)
		{
			SetCheckDummy(m_vecPreviewInfo[i]);
		}
	}
	ImGui::SameLine();

	if (ImGui::RadioButton("X2", &m_iImgProduct, 2))
	{
		for (int i = 0; i < m_vecPreviewInfo.size(); ++i)
		{
			SetCheckDummy(m_vecPreviewInfo[i]);
		}
	}
	ImGui::SameLine();

	if (ImGui::RadioButton("X4", &m_iImgProduct, 4))
	{
		for (int i = 0; i < m_vecPreviewInfo.size(); ++i)
		{
			SetCheckDummy(m_vecPreviewInfo[i]);
		}
	}

	ImGui::SameLine();
	ImGui::Dummy(ImVec2(CheckCanvasSize.x / 2.f - 195.f, 0.f));
	ImGui::SameLine();

	ImGui::PushButtonRepeat(true);
	if (ImGui::ArrowButton("##left_anim2DUI", ImGuiDir_Left) && m_vecPreviewInfo.size() > 0)
	{
		m_iCheckIdx--;
		if (m_iCheckIdx < 0) m_iCheckIdx = m_vecPreviewInfo.size() - 1;
	}

	ImGui::SameLine(0.0f, 5.f);

	if (ImGui::Button(m_strCheckButton.c_str()) && m_vecPreviewInfo.size() > 0)
	{
		m_bIsCheckPlay = !m_bIsCheckPlay;
	}

	if (m_bIsCheckPlay)
	{
		m_fDT += DT;

		if (m_fDT >= m_fDuration)
		{
			if (m_iCheckIdx >= m_vecPreviewInfo.size() - 1) m_iCheckIdx = -1;
			m_iCheckIdx++;
			m_fDT = 0.f;
		}
	}

	ImGui::SameLine(0.0f, 5.f);

	if (ImGui::ArrowButton("##right_anim2DUI", ImGuiDir_Right) && m_vecPreviewInfo.size() > 0)
	{
		m_iCheckIdx++;
		if (m_iCheckIdx >= m_vecPreviewInfo.size()) m_iCheckIdx = 0;
	}


	ImGui::SameLine(0.0f, 5.f);
	static char szChar[64];
	sprintf_s(szChar, 64, " Index / Size : %d / %d", m_iCheckIdx, (int)m_vecPreviewInfo.size());
	ImGui::Text(szChar);

	ImGui::SameLine(0.0f, 6.f);
	if (ImGui::Button("Delete##_11"))
	{
		if (m_vecPreviewInfo.size() > 0)
		{
			m_vecPreviewInfo.erase(m_vecPreviewInfo.begin() + m_iCheckIdx);

			if (m_vecPreviewInfo.size() == 0) m_iCheckIdx = 0;
			else m_iCheckIdx--;
		}
	}


	if (m_vecPreviewInfo.size() > 0)
	{
		_previewRectPos[0] = m_vecPreviewInfo[m_iCheckIdx].fSetRectPos[0];
		_previewRectPos[1] = m_vecPreviewInfo[m_iCheckIdx].fSetRectPos[1];
		_previewRectSize[0] = m_vecPreviewInfo[m_iCheckIdx].fSetRectSize[0];
		_previewRectSize[1] = m_vecPreviewInfo[m_iCheckIdx].fSetRectSize[1];
	}

	ImGui::Dummy(ImVec2(0.f, 3.f));
	ImGui::Dummy(ImVec2(3.f, 0.f));
	ImGui::SameLine();
	ImGui::PushItemWidth(130.f);
	ImGui::Text("Set Rect Pos");
	ImGui::SameLine(0.f, 5.f);

	if (ImGui::DragFloat2("##Set Rect Pos_1", _previewRectPos, 0.05f))
	{
		if (m_vecPreviewInfo.size() > 0)
		{
			m_vecPreviewInfo[m_iCheckIdx].fSetRectPos[0] = _previewRectPos[0];
			m_vecPreviewInfo[m_iCheckIdx].fSetRectPos[1] = _previewRectPos[1];

			//미리보기(흰색 렉트)영역 이동 로직
			m_vecPreviewInfo[m_iCheckIdx].vLT.x = m_vecPreviewInfo[m_iCheckIdx].vOriginLT.x + (_previewRectPos[0] / m_vTexResolution.x);
			m_vecPreviewInfo[m_iCheckIdx].vRB.x = m_vecPreviewInfo[m_iCheckIdx].vOriginRB.x + (_previewRectPos[0] / m_vTexResolution.x);
			m_vecPreviewInfo[m_iCheckIdx].vSlice.x = m_vecPreviewInfo[m_iCheckIdx].vRB.x - m_vecPreviewInfo[m_iCheckIdx].vLT.x;

			m_vecPreviewInfo[m_iCheckIdx].vLT.y = m_vecPreviewInfo[m_iCheckIdx].vOriginLT.y + (_previewRectPos[1] / m_vTexResolution.y);
			m_vecPreviewInfo[m_iCheckIdx].vRB.y = m_vecPreviewInfo[m_iCheckIdx].vOriginRB.y + (_previewRectPos[1] / m_vTexResolution.y);
			m_vecPreviewInfo[m_iCheckIdx].vSlice.y = m_vecPreviewInfo[m_iCheckIdx].vRB.y - m_vecPreviewInfo[m_iCheckIdx].vLT.y;

			//변경된 흰색 렉트의 좌표로 AnimInfo 좌표도 변경
			m_vecAnim2DInfo[m_iCheckIdx].vLT.x = m_vecPreviewInfo[m_iCheckIdx].vLT.x;
			m_vecAnim2DInfo[m_iCheckIdx].vSlice.x = m_vecPreviewInfo[m_iCheckIdx].vSlice.x;
			m_vecAnim2DInfo[m_iCheckIdx].vBackground.x = m_vecPreviewInfo[m_iCheckIdx].vSlice.x;
			m_vecAnim2DInfo[m_iCheckIdx].vLT.y = m_vecPreviewInfo[m_iCheckIdx].vLT.y;
			m_vecAnim2DInfo[m_iCheckIdx].vSlice.y = m_vecPreviewInfo[m_iCheckIdx].vSlice.y;
			m_vecAnim2DInfo[m_iCheckIdx].vBackground.y = m_vecPreviewInfo[m_iCheckIdx].vSlice.y;
		}
	}

	ImGui::SameLine(0.f, 10.0f);

	ImGui::Text("Set Rect Size");
	ImGui::SameLine(0.f, 5.f);
	if (ImGui::DragFloat2("##Set Rect Size_1", _previewRectSize, 0.05f))
	{
		if (m_vecPreviewInfo.size() > 0)
		{
			m_vecPreviewInfo[m_iCheckIdx].fSetRectSize[0] = _previewRectSize[0];
			m_vecPreviewInfo[m_iCheckIdx].fSetRectSize[1] = _previewRectSize[1];

			//미리보기(흰색 렉트)영역 크기 조절 로직
			//RB값을 조절한 뒤에 RB - LT 한 값을 Slice에 넣어준다 LT값은 건드리지 않는다
			//m_vecPreviewInfo[m_iCheckIdx].vLT.x = m_vecPreviewInfo[m_iCheckIdx].vOriginLT.x + (_previewRectSize[0] / m_vTexResolution.x);
			m_vecPreviewInfo[m_iCheckIdx].vRB.x = m_vecPreviewInfo[m_iCheckIdx].vOriginRB.x + (_previewRectSize[0] / m_vTexResolution.x);
			m_vecPreviewInfo[m_iCheckIdx].vSlice.x = m_vecPreviewInfo[m_iCheckIdx].vRB.x - m_vecPreviewInfo[m_iCheckIdx].vLT.x;

			//m_vecPreviewInfo[m_iCheckIdx].vLT.y = m_vecPreviewInfo[m_iCheckIdx].vOriginLT.y + (_previewRectSize[1] / m_vTexResolution.y);
			m_vecPreviewInfo[m_iCheckIdx].vRB.y = m_vecPreviewInfo[m_iCheckIdx].vOriginRB.y + (_previewRectSize[1] / m_vTexResolution.y);
			m_vecPreviewInfo[m_iCheckIdx].vSlice.y = m_vecPreviewInfo[m_iCheckIdx].vRB.y - m_vecPreviewInfo[m_iCheckIdx].vLT.y;
			m_vecPreviewInfo[m_iCheckIdx].vSize.x = m_vecPreviewInfo[m_iCheckIdx].vSlice.x * m_vTexResolution.x;
			m_vecPreviewInfo[m_iCheckIdx].vSize.y = m_vecPreviewInfo[m_iCheckIdx].vSlice.y * m_vTexResolution.y;

			//변경된 흰색 렉트의 좌표로 AnimInfo 좌표도 변경
			//m_vecAnim2DInfo[m_iCheckIdx].vLT.x = m_vecPreviewInfo[m_iCheckIdx].vLT.x;
			m_vecAnim2DInfo[m_iCheckIdx].vSlice.x = m_vecPreviewInfo[m_iCheckIdx].vSlice.x;
			m_vecAnim2DInfo[m_iCheckIdx].vBackground.x = m_vecPreviewInfo[m_iCheckIdx].vSlice.x;
			//m_vecAnim2DInfo[m_iCheckIdx].vLT.y = m_vecPreviewInfo[m_iCheckIdx].vLT.y;
			m_vecAnim2DInfo[m_iCheckIdx].vSlice.y = m_vecPreviewInfo[m_iCheckIdx].vSlice.y;
			m_vecAnim2DInfo[m_iCheckIdx].vBackground.y = m_vecPreviewInfo[m_iCheckIdx].vSlice.y;
		}
	}



	ImGui::PopButtonRepeat();
	// ======= 애니메이션 넘기기, Play 버튼 ======= //

	ImGui::End();
	// ======================================= Check Anim Window ======================================= //


}

void Create2DAnimationUI::SetTargetObj(CGameObject* _obj)
{
	m_fImgZoom = 0.f;
	m_pTargetObj = _obj;
	//m_pTex = m_pTargetObj->Animator2D()->GetCurAnim()->GetAtlasTex();
	//m_vTexResolution = Vec2(m_pTex->Width() + m_fImgZoom, m_pTex->Height() + m_fImgZoom);
}

void Create2DAnimationUI::CreateAnimation(wstring _name)
{
	if (m_vecAnim2DInfo.size() > 0)
	{
		CAnimation2D* pAnim2D = new CAnimation2D;
		//여기서 이 애니메이션이 반복인지 아닌지 결정해서 넘겨주기
		pAnim2D->SetName(_name);
		pAnim2D->SetAnimFrame(m_pAtlasTex, m_vecAnim2DInfo);
		pAnim2D->SetRepeat(m_bIsRepeat);

		m_pTargetObj->Animator2D()->AddAnim(ToString(_name).c_str(), pAnim2D);

		//Delegate : Animator2DUI::UpdateAnimList()
		(m_Inst->*m_AddAnimEvent)((DWORD_PTR)_name.c_str());

		VecClearFunc();
	}
}

ImVec2 Create2DAnimationUI::V2ToImV2(Vec2& _vec2)
{
	ImVec2 _imvec2;
	_imvec2.x = _vec2.x;
	_imvec2.y = _vec2.y;
	return _imvec2;
}

void Create2DAnimationUI::VecClearFunc()
{
	m_vecAnim2DInfo.clear();
	m_vecPreviewInfo.clear();
	m_iCheckIdx = 0;
	m_bIsCheckPlay = false;
}

void Create2DAnimationUI::FloatReset()
{
	for (int i = 0; i < 2; ++i)
	{
		_RectLT[i] = 0.f; 
		_RectRB[i] = 0.f;
		_RectSize[i] = 0.f;
		m_SampleUV[i].x = 0.f; 
		m_SampleUV[i].y = 0.f;
		m_OriginUV[i].x = 0.f; 
		m_OriginUV[i].y = 0.f;
	}

	m_ChangeRectSize.x = 0.f;
	m_ChangeRectSize.y = 0.f;
}

void Create2DAnimationUI::SetCheckDummy(Preview_Info& _preview)
{
	_preview.vSize = ImV2XInt(_preview.vOriginSize, m_iImgProduct);
	//CheckCanvas의 Border 때문에 2픽셀 작아진 크기로 CanvasSize가 잡혀서 좌표 오류가 났다. 그 부분까지 보정해주기
	_preview.vCheckDummy = ImVec2((CheckCanvasSize.x) / 2.f - _preview.vSize.x / 2.f,
		(CheckCanvasSize.y - 5.f) / 2.f - _preview.vSize.y / 2.f);
}


/*
	const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
	if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
	{
		scrolling.x += io.MouseDelta.x;
		scrolling.y += io.MouseDelta.y;
	}
*/

void Create2DAnimationUI::TextureSelect(DWORD_PTR _param)
{
	string strSelectedName = (char*)_param;
	wstring strTexKey = wstring(strSelectedName.begin(), strSelectedName.end());

	m_pAtlasTex = (CTexture*)CResMgr::GetInst()->FindRes<CTexture>(strTexKey).Get();
	if(m_pAtlasTex != nullptr)
		m_vTexResolution = Vec2(m_pAtlasTex->Width(), m_pAtlasTex->Height());
}

void Create2DAnimationUI::SpriteKeySelect(DWORD_PTR _param)
{
	string strSelectedName = (char*)_param;
	m_strSpriteKey = wstring(strSelectedName.begin(), strSelectedName.end());
}

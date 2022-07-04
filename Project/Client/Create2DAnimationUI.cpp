#include "pch.h"
#include "Create2DAnimationUI.h"


#include <Engine/CResMgr.h>
#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>
#include <Engine/CGameObject.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CTimeMgr.h>


Create2DAnimationUI::Create2DAnimationUI() :
	UI("Create 2D Animation UI"),
	m_vScroll(Vec2(0.f, 0.f)),
	m_gridScroll(Vec2(0.f, 0.f)),
	m_ChangeRectSize(Vec2(0.f, 0.f)),
	m_vTexResolution(Vec2(0.f, 0.f)),
	m_pTex(nullptr),
	m_SampleUV{},
	m_animName{},
	m_fImgZoom(0.f),
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
	m_pTex = CResMgr::GetInst()->Load<CTexture>(L"texture\\link_0.png", L"texture\\link_0.png").Get();
	m_vTexResolution = Vec2(m_pTex->Width() + m_fImgZoom, m_pTex->Height() + m_fImgZoom);
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
	static ImVector<ImVec2> points; //Rect������ ��ǥ 2��
	ImGuiIO GetIO = ImGui::GetIO(); //ȭ�� ��ü �������� �� ���콺��ǥ. �� ���� static�� �ϸ� ���� X


	ImGui::BeginChild("AtlasCanvas##_1", ImVec2(450.f, 450.f), true, ImGuiWindowFlags_NoScrollbar);


	ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
	ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // BeginChild�� �������� UI ������.
	ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
	const ImVec2 origin(canvas_p0.x, canvas_p0.y);       // Lock scrolled origin
	//GetIo.MousePos - GetCursorScreenPos = ���� ������â �������� LT�� 0,0 RB�� �ػ��� ���콺 ��ǥ
	Vec2 mouse_pos_in_canvas(GetIO.MousePos.x - origin.x, GetIO.MousePos.y - origin.y);
	static bool adding_Rect = false;

	//draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
	//Atlas �̹��� �ε�
	//ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.0f, 0.0f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.0f, 0.0f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.0f, 0.0f, 0.f));
	ImGui::ImageButton(m_pTex->GetSRV().Get(), m_vTexResolution, ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), 0);
	ImGui::PopStyleColor(3);
	ImGui::PopID();


	// Draw grid + all lines in the canvas
	//draw_list->PushClipRect(canvas_p0, canvas_p1, true);//���ڼ� �� UI ������
	//
	//// =========== ���ڼ� �׸��� =========== //
	//const float GRID_STEP = 64.0f;
	//for (float x = fmodf(m_gridScroll.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
	//	draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));//x�� ���� Add
	//for (float y = fmodf(m_gridScroll.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
	//	draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));//y�� ���� Add
	//// =========== ���ڼ� �׸��� =========== //
	//
	//draw_list->PopClipRect();

	// ================================= ���콺 ���� ================================= //


	//�̹��� ��Ŭ�� ����
	if (ImGui::IsItemHovered() & ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		m_vScroll.x = ImGui::GetScrollX();
		m_vScroll.y = ImGui::GetScrollY();
	}

	//��Ŭ������ �̹��� Ŭ���ϰ� �巡�� ����	
	if (ImGui::IsItemHovered() & ImGui::IsMouseDragging(ImGuiMouseButton_Right))
	{
		ImGui::SetScrollX((m_vScroll.x - ImGui::GetMouseDragDelta(1).x));
		ImGui::SetScrollY((m_vScroll.y - ImGui::GetMouseDragDelta(1).y));

		if (m_gridScroll.x >= 0.f && m_gridScroll.x <= ImGui::GetScrollMaxX()) m_gridScroll.x -= ImGui::GetIO().MouseDelta.x;
		if (m_gridScroll.y >= 0.f && m_gridScroll.y <= ImGui::GetScrollMaxY()) m_gridScroll.y -= ImGui::GetIO().MouseDelta.y;
	}

	//�簢�� ������ ���� points ���� �߰�
	if (ImGui::IsItemHovered() && !adding_Rect && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		points.clear();
		FloatReset();

		points.push_back(mouse_pos_in_canvas);
		points.push_back(mouse_pos_in_canvas);

		_vCalculValue = mouse_pos_in_canvas;

		m_SampleUV[0] = (mouse_pos_in_canvas / m_vTexResolution);
		m_OriginUV[0] = m_SampleUV[0];

		adding_Rect = true;
	}
	if (adding_Rect)
	{
		points.back() = mouse_pos_in_canvas;

		m_SampleUV[1] = (mouse_pos_in_canvas / m_vTexResolution);

		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			//UV����
			m_OriginUV[1] = m_SampleUV[1];
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

	if (points.Size > 1)
	{
		//�ȼ����� ��Ʈ���� ��ǥ�� ����
		m_vDrawRectInfo[0] = Vec2(origin.x + (m_SampleUV[0].x * m_vTexResolution.x), origin.y + (m_SampleUV[0].y * m_vTexResolution.y));
		m_vDrawRectInfo[1] = Vec2(origin.x + (m_SampleUV[1].x * m_vTexResolution.x), origin.y + (m_SampleUV[1].y * m_vTexResolution.y));
	}

	//�巡�׷� ��Ʈ���� ����
	for (int n = 0; n < points.Size; n += 2)
	{
		draw_list_1->AddRect(ImVec2(m_vDrawRectInfo[n].x, m_vDrawRectInfo[n].y),
						     ImVec2(m_vDrawRectInfo[n + 1].x, m_vDrawRectInfo[n + 1].y),
					         IM_COL32(0, 255, 0, 255), 2.0f);
	}

	//�߰��� ��� �ִϸ��̼��� ��� ��Ʈ���� ǥ��
	//for (int i = 0; i < m_vecPreviewInfo.size(); ++i)
	//{
	//	draw_list_1->AddRect(
	//		ImVec2(origin.x + m_vecPreviewInfo[i].vLT.x * m_vTexResolution.x, origin.y + m_vecPreviewInfo[i].vLT.y * m_vTexResolution.y),
	//		ImVec2(origin.x + m_vecPreviewInfo[i].vSlice.x * m_vTexResolution.x, origin.y + m_vecPreviewInfo[i].vSlice.y * m_vTexResolution.y),
	//		IM_COL32(255, 255, 255, 255), 2.0f);
	//}

	//�߰��� �ִϸ��̼��� ��� ��Ʈ���� ǥ��
	if (m_vecPreviewInfo.size() > 0)
	{
		draw_list_1->AddRect(
			ImVec2(origin.x + m_vecPreviewInfo[m_iCheckIdx].vLT.x * m_vTexResolution.x,
				origin.y + m_vecPreviewInfo[m_iCheckIdx].vLT.y * m_vTexResolution.y),
			ImVec2(origin.x + m_vecPreviewInfo[m_iCheckIdx].vRB.x * m_vTexResolution.x,
				origin.y + m_vecPreviewInfo[m_iCheckIdx].vRB.y * m_vTexResolution.y),
			IM_COL32(255, 255, 255, 255), 2.0f);
	}

	//Ȯ�� / ���
	if (ImGui::IsItemHovered())
	{
		if (KEY_PRESSED(KEY::K))
			m_fImgZoom += 0.5f;

		if (KEY_PRESSED(KEY::L) && m_fImgZoom > 0.f)
			m_fImgZoom -= 0.5f;

		if (m_fImgZoom < 0.f) m_fImgZoom = 0.f;
	}

	// ================================= ���콺 ���� ================================= //

	ImGui::EndChild();


	ImGui::SameLine(490.f, 0.f);
	ImGui::BeginChild("##Create2DAmim_InfoWindow", ImVec2(400.f, 450.f), true, ImGuiWindowFlags_NoScrollbar);

	ImGui::PushItemWidth(250.f);
	ImGui::Dummy(ImVec2(0.f, 5.f));

	static float _ScreenMousePos[2];
	_ScreenMousePos[0] = GetIO.MousePos.x;
	_ScreenMousePos[1] = GetIO.MousePos.y;
	ImGui::Text(" Screen MousePos");
	ImGui::SameLine(m_fStride, 0.f);
	ImGui::DragFloat2("##Screen Mouse Pos_1", _ScreenMousePos, 0.f, 0.f, 0.f, "%.3f", ImGuiSliderFlags_NoInput);

	static float _canvasMousePos[2];
	_canvasMousePos[0] = mouse_pos_in_canvas.x;
	_canvasMousePos[1] = mouse_pos_in_canvas.y;
	ImGui::Text(" Canvas MousePos");
	ImGui::SameLine(m_fStride, 0.f);
	ImGui::DragFloat2("##Canvas Mouse Pos_1", _canvasMousePos, 0.f, 0.f, 0.f, "%.3f", ImGuiSliderFlags_NoInput);


	//_RectLT[0] = m_SampleUV[0].x * m_vTexResolution.x; �̷��� ǥ���ϸ� ���� ���÷�Ʈ�� ��ġ�� �״�� ǥ��. 
    //�׷��� �̰��� �״�� ���ϸ� �ؿ��� �� ���ؼ� 2�谡 �ǹ����� ��ġ�� ���� ��������.
    //��Ʈ�� ��ġ�� ������ ���� ��ǥ�� 2�� ����, 1���� ��ȭ, 1���� ����. ��ȭ���� �������� ���� �̵������ָ� �ȴ�
	_RectLT[0] = m_SampleUV[0].x * m_vTexResolution.x;
	_RectLT[1] = m_SampleUV[0].y * m_vTexResolution.y;

	//ImGui::PushItemWidth(80.f);
	ImGui::Text(" Set Rect Pos");
	ImGui::SameLine(m_fStride, 0.f);
	ImGui::DragFloat2("##Set Rect Pos_1", _RectLT, 0.1f);

	ImGui::Text(" Set Rect Size");
	ImGui::SameLine(m_fStride, 0.f);
	if (ImGui::DragFloat2("##Sample Rect Size_1", _RectSize, 0.1f))
	{
		m_ChangeRectSize.x = _RectSize[0] / m_vTexResolution.x;
		m_ChangeRectSize.y = _RectSize[1] / m_vTexResolution.y;
	}
	
	m_SampleUV[0].x = m_OriginUV[0].x + ((_RectLT[0] - _vCalculValue.x) / m_vTexResolution.x);
	m_SampleUV[0].y = m_OriginUV[0].y + ((_RectLT[1] - _vCalculValue.y) / m_vTexResolution.y);

	m_SampleUV[1].x = m_OriginUV[1].x + m_ChangeRectSize.x + ((_RectLT[0] - _vCalculValue.x) / m_vTexResolution.x);
	m_SampleUV[1].y = m_OriginUV[1].y + m_ChangeRectSize.y + ((_RectLT[1] - _vCalculValue.y) / m_vTexResolution.y);

	m_SampleUV[2].x = m_SampleUV[1].x - m_SampleUV[0].x;
	m_SampleUV[2].y = m_SampleUV[1].y - m_SampleUV[0].y;


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


	ImGui::Text(" Save Button");
	ImGui::SameLine(m_fStride, 0.f);
	if (ImGui::Button("Save 2D Anim Info Button"))
	{
		tAnim2DFrame m_t2DFrm = {};
		m_t2DFrm.vLT = m_SampleUV[0];
		m_t2DFrm.vBackground = m_SampleUV[2];  //Rect�� RB - LT ���� ��(���⿵���� ũ��)
		m_t2DFrm.vSlice = m_SampleUV[2];	   //Rect�� RB - LT ���� ��(���⿵���� ũ��)
		m_t2DFrm.fDuration = m_fDuration;
		m_t2DFrm.vOffset = Vec2(0.f, 0.f);
		m_vecAnim2DInfo.push_back(m_t2DFrm);

		Preview_Info m_preview = {};
		m_preview.vOriginSize = ImVec2(m_SampleUV[2].x * m_pTex->Width(), m_SampleUV[2].y * m_pTex->Height());
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
			//���⼭ �� �ִϸ��̼��� �ݺ����� �ƴ��� �����ؼ� �Ѱ��ֱ�
			pAnim2D->SetAnimFrame(m_pTex, m_vecAnim2DInfo);
			pAnim2D->SetRepeat(m_bIsRepeat);

			m_pTargetObj->Animator2D()->AddAnim(m_animName.c_str(), pAnim2D);

			//Delegate : Animator2DUI::AddAnimList()
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
	//ImVec2 preview_Size = ImGui::GetContentRegionAvail();   // BeginChild�� �������� UI ������.
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
	//	draw_list_1->AddLine(ImVec2(preview_Pos.x + x, preview_Pos.y), ImVec2(preview_Pos.x + x, preview_Psz.y), IM_COL32(200, 200, 200, 40));//x�� ���� Add
	//for (float y = fmodf(0.f, GRID_STEP); y < preview_Psz.y; y += GRID_STEP)
	//	draw_list_1->AddLine(ImVec2(preview_Pos.x, preview_Pos.y + y), ImVec2(preview_Psz.x, preview_Pos.y + y), IM_COL32(200, 200, 200, 40));//y�� ���� Add
	//draw_list_1->PopClipRect();
	//
	//ImGui::EndChild();
	// ======================================= Preview Window ======================================= //



	// ======================================= Check Anim Window ======================================= //
	//ImGuiWindowFlags_   2
	ImGui::Begin("Check Animation##1", &m_bCheckWindow, ImGuiWindowFlags_NoResize);
	static ImDrawList* draw_list_2 = ImGui::GetWindowDrawList();//���� ������â�� DrawList ������

	if (!m_bIsCheckPlay) m_strCheckButton = "Play";
	else m_strCheckButton = "Stop";

	// ======= Check�� �ִϸ��̼� �̹��� â ======= //
	ImVec2 _checkSize = ImVec2(m_SampleUV[2].x * m_pTex->Width(), m_SampleUV[2].y * m_pTex->Height());
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
		ImGui::Image(m_pTex->GetSRV().Get(), _temp, m_vecPreviewInfo[m_iCheckIdx].vLT, m_vecPreviewInfo[m_iCheckIdx].vRB);
	}

	ImGui::EndChild();
	// ======= Check�� �ִϸ��̼� �̹��� â ======= //



	// ======= �ִϸ��̼� �ѱ��, Play ��ư ======= //
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

			//�̸�����(��� ��Ʈ)���� �̵� ����
			m_vecPreviewInfo[m_iCheckIdx].vLT.x = m_vecPreviewInfo[m_iCheckIdx].vOriginLT.x + (_previewRectPos[0] / m_vTexResolution.x);
			m_vecPreviewInfo[m_iCheckIdx].vRB.x = m_vecPreviewInfo[m_iCheckIdx].vOriginRB.x + (_previewRectPos[0] / m_vTexResolution.x);
			m_vecPreviewInfo[m_iCheckIdx].vSlice.x = m_vecPreviewInfo[m_iCheckIdx].vRB.x - m_vecPreviewInfo[m_iCheckIdx].vLT.x;

			m_vecPreviewInfo[m_iCheckIdx].vLT.y = m_vecPreviewInfo[m_iCheckIdx].vOriginLT.y + (_previewRectPos[1] / m_vTexResolution.y);
			m_vecPreviewInfo[m_iCheckIdx].vRB.y = m_vecPreviewInfo[m_iCheckIdx].vOriginRB.y + (_previewRectPos[1] / m_vTexResolution.y);
			m_vecPreviewInfo[m_iCheckIdx].vSlice.y = m_vecPreviewInfo[m_iCheckIdx].vRB.y - m_vecPreviewInfo[m_iCheckIdx].vLT.y;

			//����� ��� ��Ʈ�� ��ǥ�� AnimInfo ��ǥ�� ����
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

			//�̸�����(��� ��Ʈ)���� ũ�� ���� ����
			//RB���� ������ �ڿ� RB - LT �� ���� Slice�� �־��ش� LT���� �ǵ帮�� �ʴ´�
			//m_vecPreviewInfo[m_iCheckIdx].vLT.x = m_vecPreviewInfo[m_iCheckIdx].vOriginLT.x + (_previewRectSize[0] / m_vTexResolution.x);
			m_vecPreviewInfo[m_iCheckIdx].vRB.x = m_vecPreviewInfo[m_iCheckIdx].vOriginRB.x + (_previewRectSize[0] / m_vTexResolution.x);
			m_vecPreviewInfo[m_iCheckIdx].vSlice.x = m_vecPreviewInfo[m_iCheckIdx].vRB.x - m_vecPreviewInfo[m_iCheckIdx].vLT.x;

			//m_vecPreviewInfo[m_iCheckIdx].vLT.y = m_vecPreviewInfo[m_iCheckIdx].vOriginLT.y + (_previewRectSize[1] / m_vTexResolution.y);
			m_vecPreviewInfo[m_iCheckIdx].vRB.y = m_vecPreviewInfo[m_iCheckIdx].vOriginRB.y + (_previewRectSize[1] / m_vTexResolution.y);
			m_vecPreviewInfo[m_iCheckIdx].vSlice.y = m_vecPreviewInfo[m_iCheckIdx].vRB.y - m_vecPreviewInfo[m_iCheckIdx].vLT.y;
			m_vecPreviewInfo[m_iCheckIdx].vSize.x = m_vecPreviewInfo[m_iCheckIdx].vSlice.x * m_vTexResolution.x;
			m_vecPreviewInfo[m_iCheckIdx].vSize.y = m_vecPreviewInfo[m_iCheckIdx].vSlice.y * m_vTexResolution.y;

			//����� ��� ��Ʈ�� ��ǥ�� AnimInfo ��ǥ�� ����
			//m_vecAnim2DInfo[m_iCheckIdx].vLT.x = m_vecPreviewInfo[m_iCheckIdx].vLT.x;
			m_vecAnim2DInfo[m_iCheckIdx].vSlice.x = m_vecPreviewInfo[m_iCheckIdx].vSlice.x;
			m_vecAnim2DInfo[m_iCheckIdx].vBackground.x = m_vecPreviewInfo[m_iCheckIdx].vSlice.x;
			//m_vecAnim2DInfo[m_iCheckIdx].vLT.y = m_vecPreviewInfo[m_iCheckIdx].vLT.y;
			m_vecAnim2DInfo[m_iCheckIdx].vSlice.y = m_vecPreviewInfo[m_iCheckIdx].vSlice.y;
			m_vecAnim2DInfo[m_iCheckIdx].vBackground.y = m_vecPreviewInfo[m_iCheckIdx].vSlice.y;
		}
	}



	ImGui::PopButtonRepeat();
	// ======= �ִϸ��̼� �ѱ��, Play ��ư ======= //

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
	//CheckCanvas�� Border ������ 2�ȼ� �۾��� ũ��� CanvasSize�� ������ ��ǥ ������ ����. �� �κб��� �������ֱ�
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
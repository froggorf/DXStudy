#include "UTestComponent.h"
#include <Engine/RenderCore/EditorScene.h>

#include "Engine/Mesh/USkeletalMesh.h"
#include "../Actor/ATestActor2.h"
#include "../AnimInstance/UMyAnimInstance.h"

UTestComponent::UTestComponent()
{
	std::shared_ptr<USkeletalMesh> PaladinSkeleton = USkeletalMesh::GetSkeletalMesh("SK_MyUEFN");
	if (!PaladinSkeleton)
	{
		return;
	}
}

XMFLOAT2 GetDrawPanelPosFromLocal(const XMFLOAT2& LocalPos, const XMFLOAT2&          LocalHorizontalSize,
								const XMFLOAT2    LocalVerticalSize, const XMFLOAT2& DrawPanelSize, bool bFlipY = true)
{
	// LocalPos를 DrawPanelSize로 정규화
	// ex) [-100,100], [0,600] 같은 Local Size를
	//		[0, 400] , [0,300] 같은 DrawPanel Size로 조정
	// 특히, 윈도우 포지션은 좌상단이 0,0 이지만,
	// DrawPanel은 좌 하단이므로, 해당 값에 맞게 반환하기 위해 bFlipY 값을 통해 반영
	XMFLOAT2 ReturnValue{LocalPos};

	ReturnValue.x = ReturnValue.x - std::min(LocalHorizontalSize.x, LocalHorizontalSize.y);
	ReturnValue.y = ReturnValue.y - std::min(LocalVerticalSize.x, LocalVerticalSize.y);

	float Width = max(LocalHorizontalSize.x, LocalHorizontalSize.y) - std::min(
		LocalHorizontalSize.x, LocalHorizontalSize.y);
	float Height = max(LocalVerticalSize.x, LocalVerticalSize.y) - std::min(LocalVerticalSize.x, LocalVerticalSize.y);

	ReturnValue.x = ReturnValue.x * (DrawPanelSize.x / Width);
	ReturnValue.y = ReturnValue.y * (DrawPanelSize.y / Height);

	if (bFlipY)
	{
		ReturnValue.y = DrawPanelSize.y - ReturnValue.y;
	}
	return ReturnValue;
}

XMFLOAT2 GetLocalPosFromDrawPanel(const XMFLOAT2& DrawPanelPos, const XMFLOAT2&      LocalHorizontalSize,
								const XMFLOAT2    LocalVerticalSize, const XMFLOAT2& DrawPanelSize)
{
	XMFLOAT2 ReturnValue{DrawPanelPos};

	float Width = max(LocalHorizontalSize.x, LocalHorizontalSize.y) - std::min(
		LocalHorizontalSize.x, LocalHorizontalSize.y);
	float Height = max(LocalVerticalSize.x, LocalVerticalSize.y) - std::min(LocalVerticalSize.x, LocalVerticalSize.y);

	ReturnValue.x = ReturnValue.x * (Width / DrawPanelSize.x);
	ReturnValue.y = ReturnValue.y * (Height / DrawPanelSize.y);

	ReturnValue.x = ReturnValue.x + std::min(LocalHorizontalSize.x, LocalHorizontalSize.y);
	ReturnValue.y = ReturnValue.y + std::min(LocalVerticalSize.x, LocalVerticalSize.y);

	ReturnValue.y = LocalVerticalSize.y - ReturnValue.y - 5.0f; // Gap;

	ReturnValue.x = static_cast<int>(ReturnValue.x * 100) / 100;
	ReturnValue.y = static_cast<int>(ReturnValue.y * 100) / 100;
	return ReturnValue;
}

#ifdef WITH_EDITOR
void UTestComponent::DrawDetailPanel(UINT ComponentDepth)
{
	UActorComponent::DrawDetailPanel(ComponentDepth);

	ImGui::SliderFloat("Speed", &TestSpeed, 0.0f, 600.0f);
	//ImGui::SliderFloat("Direction", &TestAngle, -180.0f,180.0f);

	/*ImGui::SliderFloat("AimOffset_X", &AimOffsetX, -90.0f,90.0f);
	ImGui::SliderFloat("AimOffset_Y", &AimOffsetY, -90.0f,90.0f);*/

	ImGui::Dummy(ImVec2{1.0f, 50.0f});
	if (ImGui::Button("Idle", ImVec2{100.0f, 30.0f}))
	{
		TargetAnim = 0;
	}
	ImGui::SameLine();
	if (ImGui::Button("Pistol", ImVec2{100.0f, 30.0f}))
	{
		TargetAnim = 1;
	}
	ImGui::SameLine();
	if (ImGui::Button("Aim", ImVec2{100.0f, 30.0f}))
	{
		TargetAnim = 2;
	}

	if (ImGui::Button("BlendIn NoBlend"))
	{
		if (AM_NoBlend)
		{
			UMyAnimInstance::MyAnimInstance->Montage_Play(AM_NoBlend, 0.0f);
		}
	}

	ImDrawList* DrawList   = ImGui::GetWindowDrawList();
	ImVec2      CanvasPos  = ImGui::GetCursorScreenPos();
	auto        CanvasSize = ImVec2{200.0f, 150.0f};
	AM_Blend1s_Linear->BlendIn.GetCurve()->FloatCurve.DrawCurve(DrawList, CanvasPos, CanvasSize);

	if (ImGui::Button("BlendIn 0.25s - Linear"))
	{
		if (AM_Blend1s_Linear)
		{
			UMyAnimInstance::MyAnimInstance->Montage_Play(AM_Blend1s_Linear);
		}
	}

	CanvasPos = ImGui::GetCursorScreenPos();
	AM_Test->BlendIn.GetCurve()->FloatCurve.DrawCurve(DrawList, CanvasPos, CanvasSize);
	if (ImGui::Button("BlendIn 0.25s - Hermite"))
	{
		if (AM_Test)
		{
			UMyAnimInstance::MyAnimInstance->Montage_Play(AM_Test);
		}
	}

	CanvasPos = ImGui::GetCursorScreenPos();
	AM_CustomCurve->BlendIn.GetCurve()->FloatCurve.DrawCurve(DrawList, CanvasPos, CanvasSize);
	if (ImGui::Button("BlendIn 0.25s - CustomCurve"))
	{
		if (AM_CustomCurve)
		{
			UMyAnimInstance::MyAnimInstance->Montage_Play(AM_CustomCurve);
		}
	}

	//auto d2 = ImGui::GetWindowDrawList();

	//ImVec2 CurPos = ImGui::GetCursorScreenPos();
	//ImVec2 DrawRectPos = CurPos;
	//
	//d2->PushClipRect(DrawRectPos, DrawRectPos+DrawSize);
	//d2->AddRectFilled(DrawRectPos, DrawRectPos+ DrawSize, IM_COL32(50,50,50,255));

	//	for(int i = 0; i < CurrentEdge.size(); ++i)
	//	{
	//		XMFLOAT2 FirstPos = CurrentEdge[i]->StartPoint->Position;
	//		XMFLOAT2 SecondPos = CurrentEdge[i]->EndPoint->Position;
	//		XMFLOAT2 AdjustedFirstPos = GetDrawPanelPosFromLocal(FirstPos,HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});
	//		XMFLOAT2 AdjustedSecondPos= GetDrawPanelPosFromLocal(SecondPos, HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});
	//
	//		d2->AddLine(ImVec2{DrawRectPos.x+ AdjustedFirstPos.x,DrawRectPos.y+AdjustedFirstPos.y}, ImVec2{DrawRectPos.x+AdjustedSecondPos.x,DrawRectPos.y+AdjustedSecondPos.y}, IM_COL32(150,150,150,255),1);
	//	}
	//
	//	for(int i = 0 ; i < CurrentPoints.size(); ++i)
	//	{
	//		XMFLOAT2 AdjustedTestValue = GetDrawPanelPosFromLocal(CurrentPoints[i]->Position, HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});
	//		d2->AddCircleFilled(ImVec2{DrawRectPos.x + AdjustedTestValue.x, DrawRectPos.y + AdjustedTestValue.y}, 5, IM_COL32(255,255,255,255));
	//	}
	//
	//	ImGuiIO& IO = ImGui::GetIO();
	//	static ImVec2 CurTestPos;
	//	if(IO.KeyCtrl)
	//	{
	//		CurTestPos= IO.MousePos - ImVec2{10.0f,-10.0f};
	//	}
	//	XMFLOAT2 LeftTopBorderPos = GetDrawPanelPosFromLocal({HorizontalValue.x+Gap.x, VerticalValue.x + Gap.y}, HorizontalValue,VerticalValue,{DrawSize.x, DrawSize.y},false);
	//	XMFLOAT2 RightBottomBorderPos = GetDrawPanelPosFromLocal({HorizontalValue.y - Gap.x, VerticalValue.y-Gap.y}, HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y},false);
	//	CurTestPos.x = std::clamp(CurTestPos.x,LeftTopBorderPos.x+DrawRectPos.x, RightBottomBorderPos.x+DrawRectPos.x);
	//	CurTestPos.y = std::clamp(CurTestPos.y,LeftTopBorderPos.y+DrawRectPos.y, RightBottomBorderPos.y+DrawRectPos.y);
	//
	//
	//
	//	// 마우스 위치에서 Rect시작 위치를 뺴줌으로써 x축은 0부터 시작하지만, y축은 반대의 좌표계를 가지므로,
	//	// y축의 height에서 해당 값을 빼주어야 해당 렉트의 위치로 적용된다.
	//	XMFLOAT2 CurTestPosToRectSize {CurTestPos.x - DrawRectPos.x,(CurTestPos.y - DrawRectPos.y)};
	//	CurrentValue = GetLocalPosFromDrawPanel(CurTestPosToRectSize, HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});
	//	for(int i = 0; i < CurrentTriangles.size(); ++i)
	//	{
	//		XMFLOAT2 AdjustedT1 = GetDrawPanelPosFromLocal(CurrentTriangles[i]->Points[0]->Position,HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});
	//		XMFLOAT2 AdjustedT2 = GetDrawPanelPosFromLocal(CurrentTriangles[i]->Points[1]->Position,HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});
	//		XMFLOAT2 AdjustedT3 = GetDrawPanelPosFromLocal(CurrentTriangles[i]->Points[2]->Position,HorizontalValue,VerticalValue,{DrawSize.x,DrawSize.y});
	//
	//		EPointAndTriangleState State = FindPointAndTriangleState(
	//			CurTestPosToRectSize,
	//			AdjustedT1,
	//			AdjustedT2,
	//			AdjustedT3
	//		);
	//		if(State== EPointAndTriangleState::PATS_Inside)
	//		{
	//			d2->AddTriangleFilled(
	//				{DrawRectPos.x + AdjustedT1.x, DrawRectPos.y + AdjustedT1.y},
	//				{DrawRectPos.x + AdjustedT2.x, DrawRectPos.y + AdjustedT2.y},
	//				{DrawRectPos.x + AdjustedT3.x, DrawRectPos.y + AdjustedT3.y},
	//				IM_COL32(200,200,200,255)
	//			);
	//
	//			
	//
	//			break;
	//		}
	//		else if(State == EPointAndTriangleState::PATS_InEdge)
	//		{
	//			for(int EdgeIndex = 0; EdgeIndex < CurrentEdge.size();++EdgeIndex)
	//			{
	//				// TODO: 엣지에 있을 때에 대한 처리...
	//				
	//			}
	//		}
	//	}
	//
	//
	//	d2->AddLine(ImVec2{ CurTestPos.x-5.0f, CurTestPos.y}, ImVec2{CurTestPos.x+5.0f,CurTestPos.y},IM_COL32(0,255,0,255),2);
	//	d2->AddLine(ImVec2{ CurTestPos.x, CurTestPos.y-5.0f}, ImVec2{CurTestPos.x,CurTestPos.y+5.0f},IM_COL32(0,255,0,255),2);
	//
	//	d2->PopClipRect();
	//	
}
#endif

void UTestComponent::BeginPlay()
{
	UActorComponent::BeginPlay();

	AM_Test           = UAnimMontage::GetAnimationAsset("AM_Blend1s");
	AM_NoBlend        = UAnimMontage::GetAnimationAsset("AM_NoBlend");
	AM_Blend1s_Linear = UAnimMontage::GetAnimationAsset("AM_Blend1s_Linear");
	AM_CustomCurve    = UAnimMontage::GetAnimationAsset("AM_Blend1s_CustomCurve");
}

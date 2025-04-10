
#include "UTestComponent.h"
#include <Engine/RenderCore/EditorScene.h>

#include "Engine/Mesh/USkeletalMesh.h"
#include "../Actor/ATestActor2.h"
#include "../AnimInstance/UMyAnimInstance.h"

UTestComponent::UTestComponent()
{
	std::shared_ptr<USkeletalMesh> PaladinSkeleton = USkeletalMesh::GetSkeletalMesh("SK_MyUEFN");
	if(!PaladinSkeleton)
	{
		return;
	}



	
}


XMFLOAT2 GetDrawPanelPosFromLocal(const XMFLOAT2& LocalPos, const XMFLOAT2& LocalHorizontalSize, const XMFLOAT2 LocalVerticalSize, const XMFLOAT2& DrawPanelSize, bool bFlipY = true)
{
	// LocalPos를 DrawPanelSize로 정규화
	// ex) [-100,100], [0,600] 같은 Local Size를
	//		[0, 400] , [0,300] 같은 DrawPanel Size로 조정
	// 특히, 윈도우 포지션은 좌상단이 0,0 이지만,
	// DrawPanel은 좌 하단이므로, 해당 값에 맞게 반환하기 위해 bFlipY 값을 통해 반영
	XMFLOAT2 ReturnValue{LocalPos};

	ReturnValue.x = ReturnValue.x - std::min(LocalHorizontalSize.x,LocalHorizontalSize.y);
	ReturnValue.y = ReturnValue.y - std::min(LocalVerticalSize.x,LocalVerticalSize.y);

	float Width = max(LocalHorizontalSize.x,LocalHorizontalSize.y) - std::min(LocalHorizontalSize.x,LocalHorizontalSize.y);
	float Height = max(LocalVerticalSize.x,LocalVerticalSize.y) - std::min(LocalVerticalSize.x,LocalVerticalSize.y);

	ReturnValue.x = ReturnValue.x * (DrawPanelSize.x / Width);
	ReturnValue.y = ReturnValue.y * (DrawPanelSize.y / Height);

	if(bFlipY)
	{
		ReturnValue.y = DrawPanelSize.y-ReturnValue.y;
	}
	return ReturnValue;
}

XMFLOAT2 GetLocalPosFromDrawPanel(const XMFLOAT2& DrawPanelPos, const XMFLOAT2& LocalHorizontalSize, const XMFLOAT2 LocalVerticalSize, const XMFLOAT2& DrawPanelSize)
{
	XMFLOAT2 ReturnValue{DrawPanelPos};


	float Width = max(LocalHorizontalSize.x,LocalHorizontalSize.y) - std::min(LocalHorizontalSize.x,LocalHorizontalSize.y);
	float Height = max(LocalVerticalSize.x,LocalVerticalSize.y) - std::min(LocalVerticalSize.x,LocalVerticalSize.y);

	ReturnValue.x = ReturnValue.x * (Width / DrawPanelSize.x);
	ReturnValue.y = ReturnValue.y * (Height / DrawPanelSize.y);

	ReturnValue.x = ReturnValue.x + std::min(LocalHorizontalSize.x,LocalHorizontalSize.y);
	ReturnValue.y = ReturnValue.y + std::min(LocalVerticalSize.x,LocalVerticalSize.y);

	ReturnValue.y = LocalVerticalSize.y - ReturnValue.y -5.0f; // Gap;

	ReturnValue.x = static_cast<int>(ReturnValue.x * 100)/100;
	ReturnValue.y = static_cast<int>(ReturnValue.y * 100)/100;
	return ReturnValue;
}

float HermiteCurve(float t, float P0, float P1, float T0, float T1) {
	float b1 = 2 * t * t * t - 3 * t * t + 1;;
	float b2 = t * t * t - 2 * t * t + t;;
	float b3 = -2 * t * t * t + 3 * t * t;;
	float b4 = t * t * t - t * t;;

	// Hermite curve value
	return b1 * P0 + b2 * T0 + b3 * P1 + b4 * T1;
}


enum class ECurveMode
{
	ECM_Linear, ECM_Hermite
};
struct FRichCurveKey
{
	float Time;
	float Value;
	float ArriveTangent;
	float LeaveTangent;
};
struct FRichCurve
{
	ECurveMode CurveMode;
	std::vector<FRichCurveKey> Keys;

	float Eval(float InTime)
	{
		size_t KeySize = Keys.size(); 
		if(KeySize == 0)
		{
			return 0.0f;
		}
		if(InTime >= Keys[KeySize-1].Time)
		{
			return Keys[KeySize-1].Value;
		}
		if(InTime <= Keys[0].Time)
		{
			return Keys[0].Value;
		}


		for(int i = 0; i < KeySize; ++i)
		{
			// i ~ i+1 사이에 있다면 해당 값 반환
			if(Keys[i].Time <= InTime)
			{
				// 해당 Time 마지막 Time 이상인지는 앞에서 체크함 (InTime >= Keys[KeySize-1].Time)
				if(InTime < Keys[i+1].Time )
				{
					// i 와 i + 1 을 보간
					float t = InTime / (Keys[i].Time + Keys[i+1].Time);
					switch(CurveMode)
					{
					case ECurveMode::ECM_Linear:
						
						return std::lerp(Keys[i].Value,Keys[i+1].Value,t);
					break;
					case ECurveMode::ECM_Hermite:
						return HermiteCurve(t, Keys[i].Value, Keys[i+1].Value, Keys[i].LeaveTangent, Keys[i].ArriveTangent);
					break;
					default:
						assert(0);
					break;
					}
				}
			}
		}
		MY_LOG("Eval", EDebugLogLevel::DLL_Error, "Error");
		assert(0);
		
	}
};

ImVec2 operator*(float Value, ImVec2 Point)
{
	return ImVec2{Point.x*Value, Point.y*Value};
}

ImVec2 BezierPoint(const ImVec2& P0, const ImVec2& P1, const ImVec2& P0LeaveTangent, const ImVec2& P1ArriveTangent, float t)
{
	float u = 1.0f - t;
	float tt = t * t;
	float uu = u * u;
	float uuu = uu * u;
	float ttt = tt * t;

	ImVec2 Point = uuu * P0; // (1-t)^3 * P0
	Point += 3 * uu * t * P1; // 3(1-t)^2 * t * P1
	Point += 3 * u * tt * P0LeaveTangent; // 3(1-t) * t^2 * P2
	Point += ttt * P1ArriveTangent; // t^3 * P3
	return Point;
}

// 베지어 커브 그리기 함수
void DrawBezierCurve(ImDrawList* DrawList, const ImVec2& P0, const ImVec2& P1, const ImVec2& P2, const ImVec2& P3, ImU32 Color, int Segments = 100)
{
	ImVec2 PreviousPoint = P0;
	for (int i = 1; i <= Segments; ++i)
	{
		float t = i / (float)Segments;
		ImVec2 CurrentPoint = BezierPoint(P0, P1, P2, P3, t);
		DrawList->AddLine(PreviousPoint, CurrentPoint, Color, 2.0f);
		PreviousPoint = CurrentPoint;
	}
}

void DrawHermiteCurve(ImDrawList* DrawList, ImVec2 CanvasPos, ImVec2 CanvasSize, const FRichCurve& Curve)
{
	UINT KeySize = Curve.Keys.size();
	const auto& Keys = Curve.Keys;
	if(Keys.size() == 0 )
	{
		//early return
		return;
	}

	for(UINT KeyIndex = 0; KeyIndex < KeySize-1; ++KeyIndex)
	{
		if(KeyIndex+1 >= KeySize)
		{
			break;
		}

		ImVec2 PreviousPoint = { Keys[KeyIndex].Time,Keys[KeyIndex].Value};
		PreviousPoint.y = CanvasPos.y +  (1-PreviousPoint.y) * CanvasSize.y;
		PreviousPoint.x = CanvasPos.x + PreviousPoint.x * CanvasSize.x;		
		int SegmentCount = 100;
		for(int SegmentIndex = 0; SegmentIndex < SegmentCount; ++SegmentIndex)
		{
			float NewTimeNormalized = static_cast<float>(SegmentIndex) / SegmentCount;
			
			float NewValue =  HermiteCurve(NewTimeNormalized, Keys[KeyIndex].Value, Keys[KeyIndex+1].Value, Keys[KeyIndex].LeaveTangent, Keys[KeyIndex+1].ArriveTangent);
			ImVec2 CurrentKey = {NewTimeNormalized*(Keys[KeyIndex+1].Time - Keys[KeyIndex].Time) + Keys[KeyIndex].Time ,NewValue};
			// 윈도우 좌표계는 좌상단이 기준점이므로 1 - 
			CurrentKey.y = 1- CurrentKey.y;

			ImVec2 CanvasKeyPos = { CanvasPos.x + CurrentKey.x*CanvasSize.x, CanvasPos.y+ CurrentKey.y*CanvasSize.y};
			DrawList->AddLine(PreviousPoint, ImVec2{CanvasKeyPos.x,CanvasKeyPos.y}, IM_COL32(100*KeyIndex,250,0,255), 2.0f);
			PreviousPoint = CanvasKeyPos;

		}
	}
}

void UTestComponent::DrawDetailPanel(UINT ComponentDepth)
{
	UActorComponent::DrawDetailPanel(ComponentDepth);

	ImGui::SliderFloat("Speed", &TestSpeed, 0.0f,600.0f);
	//ImGui::SliderFloat("Direction", &TestAngle, -180.0f,180.0f);

	/*ImGui::SliderFloat("AimOffset_X", &AimOffsetX, -90.0f,90.0f);
	ImGui::SliderFloat("AimOffset_Y", &AimOffsetY, -90.0f,90.0f);*/

	ImGui::Dummy(ImVec2{1.0f,50.0f});
	if(ImGui::Button("Idle", ImVec2{100.0f,30.0f}))
	{
		TargetAnim = 0;
	}
	ImGui::SameLine();
	if(ImGui::Button("Pistol", ImVec2{100.0f,30.0f}))
	{
		TargetAnim = 1;
	}
	ImGui::SameLine();
	if(ImGui::Button("Aim", ImVec2{100.0f,30.0f}))
	{
		TargetAnim = 2;
	}

	if(ImGui::Button("PlayAnimMontage - Reload1"))
	{
		if(AM_Test)
		{
			UMyAnimInstance::MyAnimInstance->Montage_Play(AM_Test, 0.0f);	
		}
		
	}
	if(ImGui::Button("PlayAnimMontage - Reload2"))
	{
		if(AM_Test)
		{
			UMyAnimInstance::MyAnimInstance->Montage_Play(AM_Test, AM_Test->GetStartTimeFromSectionName("Reload2"));	
		}

	}
	if(ImGui::Button("PlayAnimMontage - Fire"))
	{
		if(AM_Test)
		{
			UMyAnimInstance::MyAnimInstance->Montage_Play(AM_Test, AM_Test->GetStartTimeFromSectionName("Fire"));	
		}

	}

	{

		static FRichCurve Curve;
		static bool bIsInitialize = false;
		if(!bIsInitialize)
		{
			bIsInitialize = true;
			Curve.CurveMode = ECurveMode::ECM_Hermite;
			Curve.Keys.push_back(FRichCurveKey{0.0f, 0.0f, 0.0f,0.0f});

			Curve.Keys.push_back(FRichCurveKey{1.0f, 1.0f, 0.0f,0.0f});
		}

		ImDrawList* DrawList = ImGui::GetWindowDrawList();
		ImVec2 CanvasPos = ImGui::GetCursorScreenPos();
		ImVec2 CanvasSize = ImVec2(300, 200);

		// 캔버스 영역 그리기
		DrawList->AddRectFilled(CanvasPos, CanvasPos + CanvasSize, IM_COL32(50, 50, 50, 255));
		DrawList->AddRect(CanvasPos, CanvasPos + CanvasSize, IM_COL32(255, 255, 255, 255));

		// 베지어 곡선 그리기
		//DrawBezierCurve(DrawList, CanvasPos + P0, CanvasPos + P1, CanvasPos + P2, CanvasPos + P3, IM_COL32(0, 255, 0, 255));
		DrawHermiteCurve(DrawList, CanvasPos, CanvasSize, Curve);

		// 제어점 그리기 및 드래그 처리
		//ImVec2* Points[] = { &P0, &P1, &P2, &P3 };
		//for (int i = 0; i < 4; ++i)
		//{
		//	ImVec2 Point = CanvasPos + *Points[i];
		//	if(ImGui::IsMouseHoveringRect(Point - ImVec2(10.0f,10.0f), Point+ImVec2(10.0f,10.0f)))
		//	{
		//		DrawList->AddCircleFilled(Point, 10.0f, IM_COL32(255,0,0, 255));
		//	}
		//	else
		//	{
		//		DrawList->AddCircleFilled(Point, 10.0f, IM_COL32(100,100,100, 255));
		//	}

		//	// 마우스 입력 처리
		//	if (ImGui::IsMouseDown(0) && ImGui::IsMouseHoveringRect(Point - ImVec2(10,10), Point + ImVec2(10,10)))
		//	{
		//		*Points[i] = ImGui::GetMousePos() - CanvasPos;
		//	}
		//}

		ImGui::Dummy(CanvasSize); // 캔버스 공간 확보
		static float X;
		static float Y;
		if(ImGui::SliderFloat("X",&X, -1.0f,2.0f))
		{
			Y = Curve.Eval(X);
		}
		ImGui::Text("Y : %f", Y);
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

void UTestComponent::BeginPlay()
{
	UActorComponent::BeginPlay();

	AM_Test = UAnimMontage::GetAnimationAsset("AM_Test");
}




#include "CoreMinimal.h"
#include "UCurveBase.h"

#include "Engine/RenderCore/EditorScene.h"

float HermiteCurve(float t, float P0, float P1, float T0, float T1)
{
	float b1 = 2 * t * t * t - 3 * t * t + 1;;
	float b2 = t * t * t - 2 * t * t + t;;
	float b3 = -2 * t * t * t + 3 * t * t;;
	float b4 = t * t * t - t * t;;

	// Hermite curve value
	return b1 * P0 + b2 * T0 + b3 * P1 + b4 * T1;
}

float FRichCurve::Eval(float InTime) const

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
	return 0.0f;
}

#ifdef WITH_EDITOR
void FRichCurve::DrawLinearCurve(ImDrawList* DrawList, ImVec2 CanvasPos, ImVec2 CanvasSize) const
{
	UINT KeySize = Keys.size();
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
		ImVec2 FirstPoint = {Keys[KeyIndex].Time, Keys[KeyIndex].Value};
		FirstPoint.y = 1-FirstPoint.y;
		FirstPoint = CanvasPos+ (FirstPoint*CanvasSize);

		ImVec2 SecondPoint = {Keys[KeyIndex+1].Time, Keys[KeyIndex+1].Value};
		SecondPoint.y = 1-SecondPoint.y;
		SecondPoint = CanvasPos+ (SecondPoint*CanvasSize);

		DrawList->AddLine(FirstPoint,SecondPoint,IM_COL32(0,255,0,255),2);
		DrawList->AddCircleFilled(FirstPoint,6.0f,IM_COL32(255,0,0,255));
		DrawList->AddCircleFilled(SecondPoint,6.0f,IM_COL32(255,0,0,255));

	}
}

void FRichCurve::DrawCurve(ImDrawList* DrawList, ImVec2 CanvasPos, ImVec2 CanvasSize) const
{
	// 캔버스 영역 그리기
	DrawList->AddRectFilled(CanvasPos, CanvasPos + CanvasSize, IM_COL32(50, 50, 50, 255));
	DrawList->AddRect(CanvasPos, CanvasPos + CanvasSize, IM_COL32(255, 255, 255, 255));
	switch (CurveMode)
	{
	case ECurveMode::ECM_Linear:
		DrawLinearCurve(DrawList,CanvasPos, CanvasSize);
		break;
	case ECurveMode::ECM_Hermite:
		DrawHermiteCurve(DrawList,CanvasPos,CanvasSize);
		break;
	default:
		break;
	}
	ImGui::Dummy(CanvasSize);
}

void FRichCurve::DrawHermiteCurve(ImDrawList* DrawList, ImVec2 CanvasPos, ImVec2 CanvasSize) const
{
	UINT KeySize = Keys.size();
	//const auto& Keys = Keys;
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
		// KeyIndex 점 그리기
		DrawList->AddCircleFilled(PreviousPoint, 10.0f, IM_COL32(255,0,0,255));


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

		ImVec2 FirstPoint = { Keys[KeyIndex].Time,Keys[KeyIndex].Value};
		FirstPoint.y = CanvasPos.y +  (1-FirstPoint.y) * CanvasSize.y;
		FirstPoint.x = CanvasPos.x + FirstPoint.x * CanvasSize.x;
		DrawList->AddCircleFilled(FirstPoint, 6.0f, IM_COL32(255,0,0,255));


		ImVec2 SecondPoint = { Keys[KeyIndex+1].Time,Keys[KeyIndex+1].Value};
		SecondPoint.y = CanvasPos.y +  (1-SecondPoint.y) * CanvasSize.y;
		SecondPoint.x = CanvasPos.x + SecondPoint.x * CanvasSize.x;
		DrawList->AddCircleFilled(SecondPoint, 6.0f, IM_COL32(255,0,0,255));
	}
}

void UCurveBase::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);

	if(GetCurveMap().contains(GetName()))
	{
		return;
	}

	std::string CurveName = GetName();
	GetCurveMap()[CurveName] = shared_from_this();
}

float UCurveFloat::GetFloatValue(float InTime) const
{
	return FloatCurve.Eval(InTime);
}

void FAlphaBlend::Update(float DeltaSeconds)
{
}

void UCurveFloat::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UCurveBase::LoadDataFromFileData(AssetData);

	if(AssetData.contains("Keys"))
	{
		auto KeyData = AssetData["Keys"];
		for(const auto& Data : KeyData)
		{
			float Time = Data["Time"];
			float Value = Data["Value"];
			float ArriveTangent = Data["ArriveTangent"];
			float LeaveTangent = Data["LeaveTangent"];
			FloatCurve.Keys.emplace_back(Time, Value,ArriveTangent,LeaveTangent);
		}
	}
	ECurveMode CurveMode = ECurveMode::ECM_Linear;
	if(AssetData.contains("CurveMode"))
	{
		std::string CurveModeName = AssetData["CurveMode"];
		if(CurveModeName == "Linear")
		{
			CurveMode = ECurveMode::ECM_Linear;
		}
		else if(CurveModeName == "Hermite")
		{
			CurveMode = ECurveMode::ECM_Hermite;
		}
	}
	FloatCurve.CurveMode = CurveMode;
}


#endif

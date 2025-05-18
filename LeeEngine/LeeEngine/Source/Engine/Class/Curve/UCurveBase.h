// 04.11
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once
#include "Engine/MyEngineUtils.h"
#include "Engine/UObject/UObject.h"

float HermiteCurve(float t, float P0, float P1, float T0, float T1);

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
	ECurveMode                 CurveMode;
	std::vector<FRichCurveKey> Keys;

	float Eval(float InTime) const;

#ifdef WITH_EDITOR
	void DrawLinearCurve(ImDrawList* DrawList, ImVec2 CanvasPos, ImVec2 CanvasSize) const;
	void DrawCurve(ImDrawList* DrawList, ImVec2 CanvasPos, ImVec2 CanvasSize) const;
	void DrawHermiteCurve(ImDrawList* DrawList, ImVec2 CanvasPos, ImVec2 CanvasSize) const;

#endif
};

class UCurveBase : public UObject, public std::enable_shared_from_this<UCurveBase>
{
	MY_GENERATED_BODY(UCurveBase)
	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	static std::shared_ptr<UCurveBase> GetCurveAssetCache(const std::string& CurveName)
	{
		auto TargetAssetIter = GetCurveMap().find(CurveName);
		if (TargetAssetIter != GetCurveMap().end())
		{
			return (TargetAssetIter->second);
		}
		return nullptr;
	}

private:
	static std::unordered_map<std::string, std::shared_ptr<UCurveBase>>& GetCurveMap()
	{
		static std::unordered_map<std::string, std::shared_ptr<UCurveBase>> CurveCacheMap;
		return CurveCacheMap;
	}
};

class UCurveFloat : public UCurveBase
{
	MY_GENERATED_BODY(UCurveFloat)
	float GetFloatValue(float InTime) const;

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	const FRichCurve& GetRichCurve() const
	{
		return FloatCurve;
	}

public:
	FRichCurve FloatCurve;
};

struct FAlphaBlend
{
	FAlphaBlend()
		: CustomCurve(nullptr), BlendTime(0.0f)
	{
	};

	void SetCurveFloat(const std::shared_ptr<UCurveFloat>& InCustomCurve)
	{
		CustomCurve = InCustomCurve;
	}

	void SetBlendTime(float InBlendTime)
	{
		BlendTime = InBlendTime;
	}

	float GetBlendTime() const
	{
		return BlendTime;
	}

	void Update(float DeltaSeconds);

	const std::shared_ptr<UCurveFloat>& GetCurve() const
	{
		return CustomCurve;
	}

private:
	std::shared_ptr<UCurveFloat> CustomCurve;
	// Blend Time
	float BlendTime;

	// update value 0.0f ~ 1.0f
	float AlphaLerp;
};

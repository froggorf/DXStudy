// 03.27
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UAnimationAsset.h"
#include "UAnimSequence.h"

enum class EPointAndTriangleState
{
	PATS_Inside,
	PATS_InEdge,
	PATS_Outside,
};

struct FAnimClipPoint
{
	XMFLOAT2 Position;
	std::shared_ptr<UAnimSequence> AnimSequence;

	FAnimClipPoint(const XMFLOAT2& InPosition, const std::shared_ptr<UAnimSequence>& InAnimSequence)
	{
		Position = InPosition;
		AnimSequence = InAnimSequence;
	}

	bool IsPointOnSegment(const XMFLOAT2& P1, const XMFLOAT2& P2) const;

};
struct FAnimClipEdge
{
	std::shared_ptr<FAnimClipPoint> StartPoint;
	std::shared_ptr<FAnimClipPoint> EndPoint;
	float LengthSq = 0.0f;

	FAnimClipEdge(const std::shared_ptr<FAnimClipPoint>& InStartPoint, const std::shared_ptr<FAnimClipPoint>& InEndPoint)
	{
		StartPoint = InStartPoint;
		EndPoint = InEndPoint;
	}

	bool IsPointOnSegment(const XMFLOAT2& TargetPoint);
};

struct FAnimClipTriangle
{
	std::vector<std::shared_ptr<FAnimClipPoint>> Points;
	std::vector<std::shared_ptr<FAnimClipEdge>> Edges;

	EPointAndTriangleState FindPointAndTriangleState(const XMFLOAT2& TargetPoint);
};


// ==============================================================

class UBlendSpace : public UAnimationAsset
{
	MY_GENERATED_BODY(UBlendSpace)
public:

	static std::shared_ptr<UBlendSpace> GetAnimationAsset(const std::string& AnimationName)
	{
		
		if(std::shared_ptr<UAnimationAsset> FindAsset = UAnimationAsset::GetAnimationAsset(AnimationName))
		{
			return std::dynamic_pointer_cast<UBlendSpace>(FindAsset);	
		}
		return nullptr;	
		
	}

	virtual void GetAnimationBoneMatrices(const XMFLOAT2& AnimValue, float CurrentAnimTime, std::vector<XMMATRIX>& OutMatrices,
		std::vector<FAnimNotifyEvent>& OutActiveNotifies);

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;
protected:
private:
	// 연산량이 많으므로, 가급적 초기화 시 한번만 호출
	// 추가된 Points에 맞게 Edge와 Triangle을 만드는 함수
	void CreateEdgeAndTriangle();
	void CreateEdge();
	void CreateTriangle();

	
	static void TriangleInterpolation(const XMFLOAT2& AnimValue, const std::shared_ptr<FAnimClipTriangle>& Triangle, float AnimTime, std::vector<XMMATRIX>& OutMatrices,
		std::vector<FAnimNotifyEvent>& OutActiveNotifies);
	static void LinearInterpolation(const XMFLOAT2& CurrentValue,const std::shared_ptr<FAnimClipEdge>& Edge, float AnimTime, std::vector<XMMATRIX>& OutMatrices,
		std::vector<FAnimNotifyEvent>& OutActiveNotifies);
	static void CalculateOneAnimation(const std::shared_ptr<FAnimClipPoint>& Point, float AnimTime, std::vector<XMMATRIX>& OutMatrices,
		std::vector<FAnimNotifyEvent>& OutActiveNotifies);
public:
protected:
	XMFLOAT2 HorizontalValue = {};
	XMFLOAT2 VerticalValue = {};

	std::vector< std::shared_ptr<FAnimClipPoint>> CurrentPoints;
	std::vector<std::shared_ptr<FAnimClipEdge>> CurrentEdge;
	std::vector <std::shared_ptr<FAnimClipTriangle>> CurrentTriangles;

private:
	static XMFLOAT2 Gap;
};
// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include <Engine/Animation/UAnimSequence.h>

#include "ThirdParty/ImGui/imgui.h"
#include "Engine/Components/UActorComponent.h"

struct FAnimClipPoint
{
	XMFLOAT2 Position;
	std::shared_ptr<UAnimSequence> AnimSequence;

	FAnimClipPoint(const XMFLOAT2& InPosition, const std::shared_ptr<UAnimSequence>& InAnimSequence)
	{
		Position = InPosition;
		AnimSequence = InAnimSequence;
	}
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
	
};

struct FAnimClipTriangle
{
	std::vector<std::shared_ptr<FAnimClipPoint>> Points;
	std::vector<std::shared_ptr<FAnimClipEdge>> Edges;


};

class UTestComponent : public UActorComponent
{
	MY_GENERATED_BODY(UTestComponent)
public:
	UTestComponent();

#ifdef WITH_EDITOR
	void DrawDetailPanel(UINT ComponentDepth) override;
#endif

	void GetAnimsForBlend(XMFLOAT2& OutCurrentValue, std::vector<std::shared_ptr<FAnimClipPoint>>& OutPoints);

	const std::vector<std::shared_ptr<FAnimClipEdge>>& GetCurrentEdge() const {return CurrentEdge;}
	const std::vector<std::shared_ptr<FAnimClipTriangle>>& GetCurrentTriangles() const {return CurrentTriangles;}
	const std::vector<std::shared_ptr<FAnimClipPoint>>& GetTestValue() const {return TestValue;}
protected:
private:
public:
	float TestValue1 = 0.0f;

	XMFLOAT2 Gap = {5.0f,5.0f};
	ImVec2 DrawSize = ImVec2{400.0f,300.0f};
	XMFLOAT2 HorizontalValue;
	XMFLOAT2 VerticalValue;
	std::vector<std::shared_ptr<FAnimClipEdge>> CurrentEdge;
	std::vector <std::shared_ptr<FAnimClipTriangle>> CurrentTriangles;

	XMFLOAT2 CurrentValue = XMFLOAT2{0.0f,0.0f};

	std::vector< std::shared_ptr<FAnimClipPoint>> TestValue;

protected:
private:
};
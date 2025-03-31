// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "Engine/Components/UActorComponent.h"

struct Edge
{
	XMFLOAT2 StartPos;
	XMFLOAT2 EndPos;
	float LengthSq;
};

struct Triangle
{
	std::vector<XMFLOAT2> Points;
	std::vector<Edge> Edges;


};

class UTestComponent : public UActorComponent
{
	MY_GENERATED_BODY(UTestComponent)
public:
	UTestComponent();



#ifdef WITH_EDITOR
	void DrawDetailPanel(UINT ComponentDepth) override;
#endif
protected:
private:
public:
	float TestValue1 = 0.0f;

	std::vector<Edge> CurrentEdge;
	std::vector<Triangle> CurrentTriangles;

	std::vector< XMFLOAT2> TestValue;
protected:
private:
};
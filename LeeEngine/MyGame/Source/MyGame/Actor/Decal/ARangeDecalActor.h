#pragma once
#include "Engine/GameFramework/AActor.h"

/*
 몬스터의 장판형 공격 스킬의 범위를 안내해주는 액터
 Decal 컴퍼넌트를 통해 표현
 */

class UDecalComponent;

struct FRangeDecalData
{
	float Progress;													// 진척도(0 ~ 1)
	float BorderThickness = 0.01f;									// 범위 바깥 테두리 두께
	XMFLOAT3 BaseColor = XMFLOAT3{0.4f, 0.25f, 0.0f};		// 베이스 색상
	XMFLOAT3 ActiveColor = XMFLOAT3{ 0.3f, 0.0f, 0.0f};		// 차오르는 범위 색상
	float HalfAngleDeg = 180.0f;									// 부채꼴의 반각 (원형 장판일 시 180 설정)
	XMFLOAT2 ForwardXZ = {0.0f, 1.0f};							// 부채꼴의 방향 Forward Vector2	
};


class ARangeDecalActor : public AActor
{
	MY_GENERATE_BODY(ARangeDecalActor)

	ARangeDecalActor();
	~ARangeDecalActor() override = default;

	ARangeDecalActor* SetProgress(float NewProgress);
	ARangeDecalActor* SetBaseColor(const XMFLOAT3& NewBaseColor);
	ARangeDecalActor* SetActiveColor(const XMFLOAT3& ActiveColor);
	ARangeDecalActor* SetHalfAngleDeg(float NewHalfAngleDeg);
	ARangeDecalActor* SetForward(const XMFLOAT3& NewForward);
	ARangeDecalActor* SetForward(const XMFLOAT2& NewForward);

	static std::shared_ptr<UMaterialInterface> MI_SkillRangeDecal;

protected:
	std::shared_ptr<UDecalComponent> DecalComp;
	FRangeDecalData DecalData;
};

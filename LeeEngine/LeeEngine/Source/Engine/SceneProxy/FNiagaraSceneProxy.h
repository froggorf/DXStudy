// 05.03
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include "FPrimitiveSceneProxy.h"
#include "Engine/MyEngineUtils.h"

#define MaxParticleCount 500
#define ParticleDataRegister 20

// 파티클 구조체
struct FParticleData
{
	XMFLOAT3	LocalPos;		// 소유 오브젝트로 부터의 상대 좌표
	XMFLOAT3	WorldPos;		// 파티클의 월드 좌표
	XMFLOAT3	WorldInitScale; // 파티클 생성 시 초기 크기
	XMFLOAT3	WorldScale;		// 파티클 월드 크기
	XMFLOAT4	Color;			// 파티클 색상

	XMFLOAT3	Force;			// 파티클에 주어지고 있는 힘의 총합
	XMFLOAT3	Velocity;		// 파티클 속도
	float	Mass;			// 파티클 질량

	float	Age;			// 파티클 나이, Age 가 Life 에 도달하면 수명이 다한 것
	float	Life;			// 파티클 최대 수명
	float	NormalizedAge;  // 전체 수명 대비, 현재 Age 비율. 자신의 Age 를 Life 대비 정규화 한 값

	int		Active;			// 파티클 활성화 여부
};


class FNiagaraSceneProxy : public FPrimitiveSceneProxy
{
public:
	FNiagaraSceneProxy(UINT InPrimitiveID);
	~FNiagaraSceneProxy() override = default;

	void Draw() override;
	void TickCS();
public:
protected:
	std::shared_ptr<FStructuredBuffer> ParticleBuffer;
};

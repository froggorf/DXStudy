struct FParticleData
{
	float3	LocalPos;		// 소유 오브젝트로 부터의 상대 좌표
	float3	WorldPos;		// 파티클의 월드 좌표

	float3	WorldInitScale; // 파티클 생성 시 초기 크기
	float3	WorldScale;		// 파티클 월드 크기
	float4	Color;			// 파티클 색상

	float3  Force;			// 파티클에 주어지고 있는 힘의 총합
	float3  Velocity;		// 파티클 속도
	float	Mass;			// 파티클 질량

	float	Age;			// 파티클 나이, Age 가 Life 에 도달하면 수명이 다한 것
	float	Life;			// 파티클 최대 수명
	float	NormalizedAge;  // 전체 수명 대비, 현재 Age 비율. 자신의 Age 를 Life 대비 정규화 한 값

	int		Active;			// 파티클 활성화 여부
};

// 엔진 폴더 내에도 #define ParticleRegister 으로 정의 되어 있음
#define ParticleRegister t20
StructuredBuffer<FParticleData> gParticleData : register(ParticleRegister);
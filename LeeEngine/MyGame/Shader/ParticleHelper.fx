struct FParticleData
{
	float4 DynamicParam;

	float3 LocalPos; // 소유 오브젝트로 부터의 상대 좌표
	float3 WorldPos; // 파티클의 월드 좌표
	float3 WorldInitPos;
	float3 WorldRotation;
	float3 WorldInitScale; // 파티클 생성 시 초기 크기
	float3 WorldScale;     // 파티클 월드 크기
	float4 StartColor;
	float4 Color;          // 파티클 색상

	float3 Force;    // 파티클에 주어지고 있는 힘의 총합
	float3 Velocity; // 파티클 속도
	float  Mass;     // 파티클 질량

	float Age;           // 파티클 나이, Age 가 Life 에 도달하면 수명이 다한 것
	float Life;          // 파티클 최대 수명
	float NormalizedAge; // 전체 수명 대비, 현재 Age 비율. 자신의 Age 를 Life 대비 정규화 한 값

	int UCount;
	int VCount;

	// Orbit 모듈용
	float OrbitRadius;      // 원의 반지름
	float OrbitPhase;       // 시작 각도(랜덤)
	float OrbitSpeed;       // 각속도(초당 라디안)

	int Active; // 파티클 활성화 여부
};

// CPU 코드 내에도 ParticleDataRegister 로 20이 지정되어있음
#define ParticleDataRegister t20
StructuredBuffer<FParticleData> gParticle : register(t20);

struct FParticleSpawn
{
	uint   SpawnCount;
	float3 vPadding;
};

// Particle Module
struct FParticleModule
{
	// Activate State
	int ActivateState; // 파티클 전체에 대한 활성 상태 (0 : Activate, 1 : Deactivate, 2 : Reset)

	// Spawn Modlue
	float  SpawnRate;  // 초당 파티클 생성량
	float4 StartColor; // 초기 파티클 색상
	int bStartColorRandom;
	float4 StartColorMin;
	float4 StartColorMax;
	int    bIsLoop;    // 반복되는 파티클

	float3 MinScale;    // 생성 시 최소 크기
	float3 MaxScale;    // 생성 시 최대 크기
	float3 MinRotation; // 생성 시 최소 회전
	float3 MaxRotation; // 생성 시 최대 회전
	float  MinLife;     // 생성 시 최소 주기
	float  MaxLife;     // 생성 시 최대 주기
	int    SpawnShape;  // 0 : Box, 1 : Sphere
	float3 SpawnShapeScale;
	int    SpaceType; // 0 : Local, 1 : World

	uint   BlockSpawnShape;      // 0 : Box,  1: Sphere
	float3 BlockSpawnShapeScale; // SpawnShapeScale.x == Radius

	// Spawn Burst
	uint  SpawnBurstCount; // 한번에 발생시키는 Particle 수
	uint  SpawnBurstRepeat;
	float SpawnBurstRepeatTime;
	float AccSpawnBurstRepeatTime;

	// Add Velocity
	uint   AddVelocityType; // 0 : Random, 1 : FromCenter, 2 : ToCenter, 4 : Fixed 
	float3 AddVelocityFixedDir;
	float3 AddMinSpeed;
	float3 AddMaxSpeed;

	// Scale Module
	float StartScale;
	float EndScale;

	// Add Tick Velocity 모듈
	float3 AddTickVelocity;

	// Render Module
	float4 EndColor;          // 파티클 최종 색상
	int    FadeOut;           // 0 : Off, 1 : Normalized Age
	float  StartRatio;        // FadeOut 효과가 시작되는 Normalized Age 지점
	uint   VelocityAlignment; // 속도 정렬 0 : Off, 1 : On
	uint   CrossMesh;         // 십자형태 메쉬 사용 0 : Off, 1 : ON

	// UVAnim
	int UCount;
	int VCount;

	// AddRotation 모듈
	float3 AddRotation;

	// 추가 데이터
	float3 ObjectWorldPos;
	float3 ObjectWorldScale;
	float4 ObjectWorldRotation;

	// Dynamic Param 모듈
	float4 DynamicParamMin;
	float4 DynamicParamMax;

	// Orbit 모듈
	float OrbitRadiusMin;
	float OrbitRadiusMax;
	float OrbitSpeedMin;
	float OrbitSpeedMax;

	// Module On / Off
	int Module[11];
};

// 쿼터니언 → 3x3 회전행렬
float3x3 QuaternionToMatrix(float4 q)
{
	float x  = q.x,   y  = q.y,   z  = q.z, w = q.w;
	float xx = x * x, yy = y * y, zz = z * z;
	float xy = x * y, xz = x * z, yz = y * z;
	float wx = w * x, wy = w * y, wz = w * z;

	return float3x3(1.0 - 2.0 * (yy + zz), 2.0 * (xy - wz), 2.0 * (xz + wy), 2.0 * (xy + wz), 1.0 - 2.0 * (xx + zz), 2.0 * (yz - wx), 2.0 * (xz - wy), 2.0 * (yz + wx), 1.0 - 2.0 * (xx + yy));
}

// Roll-Pitch-Yaw (Z-X-Y) 순서로 쿼터니언 생성 (도 단위 입력)
float4 EulerToQuaternion_RollPitchYaw(float3 euler)
{
	float3 rad = euler * (PI / 180.0f); // 도 → 라디안

	float cz = cos(rad.z * 0.5); // Roll (Z)
	float sz = sin(rad.z * 0.5);
	float cx = cos(rad.x * 0.5); // Pitch (X)
	float sx = sin(rad.x * 0.5);
	float cy = cos(rad.y * 0.5); // Yaw (Y)
	float sy = sin(rad.y * 0.5);

	float4 q;
	q.x = sx * cy * cz + cx * sy * sz;
	q.y = cx * sy * cz - sx * cy * sz;
	q.z = cx * cy * sz + sx * sy * cz;
	q.w = cx * cy * cz - sx * sy * sz;
	return q;
}

// Euler → 쿼터니언 → 회전행렬 (Roll-Pitch-Yaw 기본)
float3x3 EulerRotationToMatrix(float3 euler)
{
	// 다이렉트 X11 좌표계를 활용했으므로 역수를 취해줘야함
	euler *= -1;
	float4 quat = EulerToQuaternion_RollPitchYaw(euler);
	return QuaternionToMatrix(quat);
}

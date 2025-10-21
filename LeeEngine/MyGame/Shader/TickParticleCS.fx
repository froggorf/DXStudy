#include "Global.fx"
#include "ParticleHelper.fx"

// float 1개의 seed 기반 random 값 생성 함수
float3 GetRandom(float seed)
{
	// 여러 해시와 삼각함수, 상수 조합
	float a = frac(sin(seed * 12.9898 + gTime) * 43758.5453);
	float b = frac(cos(seed * 78.233 + a) * 12345.6789);
	float c = frac(sin((a + b) * 45.164 + seed) * 98765.4321);

	float3 v = float3(a, b, c);

	// 더 섞기
	v = frac(v * float3(23.1, 17.7, 42.3) + sin(v.yzx * 5.13 + gTime * 0.13));
	return v;
}

// 새로운 파티클을 추가하는 함수
void ParticleInit(inout FParticleData Particle, in FParticleModule Module, float NormalizedThreadID)
{
	Particle = (FParticleData)0.f;

	// 파티클 활성화
	Particle.Active = 1;

	// Spawm Module
	// Box
	if (0 == Module.SpawnShape)
	{
		float3 Random = GetRandom(NormalizedThreadID);

		// 랜덤 범위를 -0.5 ~ 0.5f 로 변경 후, 스폰영역 크기를 곱해서 위치값을 구한다.
		float3 vSpawnPos       = (Random - 0.5f) * Module.SpawnShapeScale;
		Particle.LocalPos.xyz = vSpawnPos;
	}
	// Sphere
	else if (1 == Module.SpawnShape)
	{
		float3 Random = GetRandom(NormalizedThreadID);

		float Radius = Module.SpawnShapeScale.x * 0.5f; // 구의 반지름 (x축 기준)

		// 랜덤 구 내부 위치
		float Theta = Random.x * 2.0f * 3.14159265f; // 0 ~ 2pi
		float PHI   = acos(1.0f - 2.0f * Random.y);   // 0 ~ pi
		float R     = pow(Random.z, 1.0f/3.0f) * Radius; // 0 ~ radius (균일분포)

		float X = R * sin(PHI) * cos(Theta);
		float Y = R * sin(PHI) * sin(Theta);
		float Z = R * cos(PHI);

		float3 SpawnPos = float3(X, Y, Z);
		Particle.LocalPos.xyz = SpawnPos;
	}
	// Cylinder
	else if (2 == Module.SpawnShape)
	{
		float3 Random = GetRandom(NormalizedThreadID);
		float  radius  = (Module.SpawnShapeScale.x + Module.SpawnShapeScale.z) / 2 * 0.5f;
		float  height  = Module.SpawnShapeScale.y;

		float theta = Random.x * 2.0f * 3.14159265f;
		float r     = sqrt(Random.y) * radius;

		float x = r * cos(theta);
		float y = (Random.z - 0.5f) * height;
		float z = r * sin(theta);

		float3 vSpawnPos       = float3(x, y, z);
		Particle.LocalPos.xyz = vSpawnPos;
	}

	Particle.WorldRotation.xyz = GetRandom(NormalizedThreadID).xyz * (Module.MaxRotation - Module.MinRotation) + Module.MinRotation;

	// 파티클 Life 랜덤 설정
	Particle.Age  = 0.f;
	float3 vRandom = GetRandom(NormalizedThreadID + 0.1f);
	Particle.Life = (Module.MaxLife - Module.MinLife) * vRandom.r + Module.MinLife;

	// 파티클 초기 색 설정
	if (0 != Module.bStartColorRandom)
	{
		float Rand = GetRandom(NormalizedThreadID + gTime);
		Particle.Color.x = Rand * (Module.StartColorMax.x - Module.StartColorMin.x) + Module.StartColorMin.x;
		Particle.Color.y = Rand * (Module.StartColorMax.y - Module.StartColorMin.y) + Module.StartColorMin.y;
		Particle.Color.z = Rand * (Module.StartColorMax.z - Module.StartColorMin.z) + Module.StartColorMin.z;
		Particle.Color.w = Rand * (Module.StartColorMax.w - Module.StartColorMin.w) + Module.StartColorMin.w;
		Particle.StartColor = Particle.Color;
	}
	else
	{
		Particle.Color = Module.StartColor;
		Particle.StartColor = Module.StartColor;
	}

	// 파티클 초기 크기 설정
	Particle.WorldInitScale.xyz = Particle.WorldScale.xyz = (Module.MaxScale - Module.MinScale) * vRandom.g + Module.MinScale;

	// 파티클 질량 설정
	Particle.Mass = 1.f;

	// 파티클 초기 속도 추가
	Particle.Velocity = (float4)0.f;

	if (0 != Module.Module[2])
	{
		float3 Random = GetRandom(NormalizedThreadID + 0.2f);

		float3 fSpeed = Module.AddMinSpeed + (Module.AddMaxSpeed - Module.AddMinSpeed) * Random;

		// Random
		if (0 == Module.AddVelocityType)
			Particle.Velocity.xyz = fSpeed;
		else if (1 == Module.AddVelocityType)
			Particle.Velocity.xyz = normalize(Random - 0.5f) * fSpeed;
			// FromCenter
		else if (2 == Module.AddVelocityType)
			Particle.Velocity.xyz = normalize(Particle.LocalPos) * fSpeed;
			// ToCenter
		else if (3 == Module.AddVelocityType)
			Particle.Velocity.xyz = -normalize(Particle.LocalPos) * fSpeed;
			// Fixed
		else
			Particle.Velocity.xyz = normalize(Module.AddVelocityFixedDir) * fSpeed;
	}

	// Particle 의 World 좌표 계산
	Particle.WorldPos.xyz = Particle.LocalPos.xyz + Module.ObjectWorldPos;

	// Dynamic Param 모듈
	if (0 != Module.Module[8])
	{
		Particle.DynamicParam.x = GetRandom(NormalizedThreadID*gTime) * (Module.DynamicParamMax.x - Module.DynamicParamMin.x) + Module.DynamicParamMin.x;
		Particle.DynamicParam.y = GetRandom(NormalizedThreadID) * (Module.DynamicParamMax.y - Module.DynamicParamMin.y) + Module.DynamicParamMin.y;
		Particle.DynamicParam.z = GetRandom(NormalizedThreadID) * (Module.DynamicParamMax.z - Module.DynamicParamMin.z) + Module.DynamicParamMin.z;
		Particle.DynamicParam.w = GetRandom(gTime) * (Module.DynamicParamMax.w - Module.DynamicParamMin.w) + Module.DynamicParamMin.w;
	}

	// Orbit 모듈
	if (0 != Module.Module[9])
	{
		float3 Random = GetRandom(NormalizedThreadID + 0.5f);
		Particle.OrbitRadius = Module.OrbitRadiusMin + (Module.OrbitRadiusMax - Module.OrbitRadiusMin) * Random.x;
		Particle.OrbitPhase  = 2.0f * PI * Random.y; // 0 ~ 2pi
		Particle.OrbitSpeed  = Module.OrbitSpeedMin + (Module.OrbitSpeedMax - Module.OrbitSpeedMin) * Random.z;
	}
}

RWStructuredBuffer<FParticleData>  gBuffer : register(u0);
RWStructuredBuffer<FParticleSpawn> gSpawnCount : register(u1);
// ParticleDataRegister는 렌더링 시에는 ParticleData를 담는 데이터를 SRV에 담기위한 용도로 사용되지만,
// 컴퓨트 셰이더에서는 같은 파티클을 다루는 역할이라는 의미로 해당 레지스터를 사용
StructuredBuffer<FParticleModule> gModule : register(ParticleDataRegister);

#define MaxParticleCount 500

[numthreads(256, 1, 1)]void CS_TickParticle(int3 ThreadID : SV_DispatchThreadID)
{
	if (MaxParticleCount <= ThreadID.x)
	{
		return;
	}

	// 만약 비활성화 / 리셋 상태라면
	if (gModule[0].ActivateState > 0)
	{
		gBuffer[ThreadID.x].Active = 0;
		// 비활성화일 경우 종료
		if (gModule[0].ActivateState == 1)
		{
			return;
		}
	}

	// 현재 파티클이 비활성화 중이라면 새로운 파티클을 생성
	if (gBuffer[ThreadID.x].Active == 0)
	{
		int Success = 0;

		while (0 < gSpawnCount[0].SpawnCount)
		{
			int DestValue     = gSpawnCount[0].SpawnCount;
			int Value         = gSpawnCount[0].SpawnCount - 1;
			int OriginalValue = 0;

			InterlockedCompareExchange(gSpawnCount[0].SpawnCount, DestValue, Value, OriginalValue);
			if (OriginalValue == DestValue)
			{
				Success = 1;
			}
		}

		// 활성화된 파티클 초기화 작업
		if (Success)
		{
			float NormalizedThreadID = (float)ThreadID.x / (float)MaxParticleCount;
			ParticleInit(gBuffer[ThreadID.x], gModule[0], NormalizedThreadID);
		}
	}

	// 파티클 업데이트
	else
	{
		// NormalizedAge (0~1)
		gBuffer[ThreadID.x].NormalizedAge = gBuffer[ThreadID.x].Age / gBuffer[ThreadID.x].Life;

		// 이번 프레임의 누적된 힘을 초기화
		gBuffer[ThreadID.x].Force.xyz = float3(0.0f, 0.0f, 0.0f);

		// 속도에 따른 이동
		float Accel = gBuffer[ThreadID.x].Force / gBuffer[ThreadID.x].Mass;

		// Module[0] : Space
		// 0 : Local Space
		// 1 : World Space
		if (0 == gModule[0].SpaceType)
		{
			gBuffer[ThreadID.x].LocalPos += gBuffer[ThreadID.x].Velocity * gDeltaTime;
			gBuffer[ThreadID.x].WorldPos = gBuffer[ThreadID.x].LocalPos + gModule[0].ObjectWorldPos;
		}
		else
		{
			gBuffer[ThreadID.x].LocalPos += gBuffer[ThreadID.x].Velocity * gDeltaTime;
			gBuffer[ThreadID.x].WorldPos += gBuffer[ThreadID.x].Velocity * gDeltaTime;
		}

		// Scale 모듈에 따라 현재 크기 업데이트
		if (gModule[0].Module[3])
		{
			float CurScale                 = (gModule[0].EndScale - gModule[0].StartScale) * gBuffer[ThreadID.x].NormalizedAge + gModule[0].StartScale;
			gBuffer[ThreadID.x].WorldScale = gBuffer[ThreadID.x].WorldInitScale * CurScale;
		}

		// Drag 감속 모듈
		if (gModule[0].Module[4])
		{
			gBuffer[ThreadID.x].Velocity += gModule[0].AddTickVelocity * gDeltaTime;
		}

		// 렌더링 관련 옵션들
		if (gModule[0].Module[5])
		{
			gBuffer[ThreadID.x].Color = (gModule[0].EndColor - gBuffer[ThreadID.x].StartColor) * gBuffer[ThreadID.x].NormalizedAge + gBuffer[ThreadID.x].StartColor;
			if (gModule[0].FadeOut && gBuffer[ThreadID.x].NormalizedAge >= gModule[0].StartRatio)
			{
				float fRatio = saturate(1.f - (gBuffer[ThreadID.x].NormalizedAge - gModule[0].StartRatio) / (1.f - gModule[0].StartRatio));

				gBuffer[ThreadID.x].Color.a = gModule[0].EndColor.a * fRatio;
			}
		}

		// UVAnim
		if (gModule[0].Module[6])
		{
			gBuffer[ThreadID.x].UCount = gModule[0].UCount;
			gBuffer[ThreadID.x].VCount = gModule[0].VCount;
		}
		else
		{
			gBuffer[ThreadID.x].UCount = 0;
			gBuffer[ThreadID.x].VCount = 0;
		}

		// Add Rotation
		if (gModule[0].Module[7])
		{
			gBuffer[ThreadID.x].WorldRotation += gModule[0].AddRotation * gDeltaTime;
		}

		// Orbit 모듈
		if (gModule[0].Module[9])
		{
			float CurrAngle = gBuffer[ThreadID.x].OrbitPhase + gBuffer[ThreadID.x].OrbitSpeed * gBuffer[ThreadID.x].Age;
			float PrevAngle = gBuffer[ThreadID.x].PrevAngle;

			// 현재 위치에서 이동량 계산
			float CurrX = gBuffer[ThreadID.x].OrbitRadius * cos(CurrAngle);
			float CurrZ = gBuffer[ThreadID.x].OrbitRadius * sin(CurrAngle);

			float PrevX = gBuffer[ThreadID.x].OrbitRadius * cos(PrevAngle);
			float PrevZ = gBuffer[ThreadID.x].OrbitRadius * sin(PrevAngle);

			// 이동량(Delta) 계산
			float DeltaX = CurrX - PrevX;
			float DeltaZ = CurrZ - PrevZ;

			// 위치에 더해줌 (Velocity 이동과 합산)
			gBuffer[ThreadID.x].LocalPos.x += DeltaX;
			gBuffer[ThreadID.x].LocalPos.z += DeltaZ;
			// Module[0] : Space
			// 0 : Local Space
			// 1 : World Space
			if (0 == gModule[0].SpaceType)
			{
				gBuffer[ThreadID.x].WorldPos = gBuffer[ThreadID.x].LocalPos + gModule[0].ObjectWorldPos;
			}
			else
			{
				gBuffer[ThreadID.x].WorldPos.x += DeltaX;
				gBuffer[ThreadID.x].WorldPos.z += DeltaZ;
			}
		}

		gBuffer[ThreadID.x].Age += gDeltaTime;
		// 파티클의 수명이 다했는지 체크
		if (gBuffer[ThreadID.x].Age >= gBuffer[ThreadID.x].Life)
		{
			// 반복 재생이 아닐 경우 삭제
			if (gModule[0].bIsLoop == 0)
			{
				gBuffer[ThreadID.x].Active = 0;
				gBuffer[ThreadID.x].Age    = 0.f;
			}
			else
			{
				gBuffer[ThreadID.x].Age -= gBuffer[ThreadID.x].Life;
			}
		}

		
	}
}

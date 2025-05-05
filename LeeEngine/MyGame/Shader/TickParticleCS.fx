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
void ParticleInit(inout FParticleData _Particle, in FParticleModule _Module
    , float _NomalizedThreadID)
{
    _Particle = (FParticleData) 0.f;

    // 파티클 활성화
    _Particle.Active = 1;

    // Spawm Module
    // Box
    if (0 == _Module.SpawnShape)
    {
        float3 vRandom = GetRandom(_NomalizedThreadID);

        // 랜덤 범위를 -0.5 ~ 0.5f 로 변경 후, 스폰영역 크기를 곱해서 위치값을 구한다.
        float3 vSpawnPos = (vRandom - 0.5f) * _Module.SpawnShapeScale;
        _Particle.LocalPos.xyz = vSpawnPos;
    }
    // Sphere
    else if (1 == _Module.SpawnShape)
    {
        _Particle.LocalPos.xyz = float3(0.0f,0.0f,0.0f);
    }

    _Particle.WorldRotation.xyz = GetRandom(_NomalizedThreadID).xyz * (_Module.MaxRotation - _Module.MinRotation) + _Module.MinRotation;

    // 파티클 Life 랜덤 설정
    _Particle.Age = 0.f;
    float3 vRandom = GetRandom(_NomalizedThreadID + 0.1f);
    _Particle.Life = (_Module.MaxLife - _Module.MinLife) * vRandom.r + _Module.MinLife;

    // 파티클 초기 색 설정
    _Particle.Color = _Module.StartColor;

    // 파티클 초기 크기 설정
    _Particle.WorldInitScale.xyz = _Particle.WorldScale.xyz = (_Module.MaxScale - _Module.MinScale) * vRandom.g + _Module.MinScale;
    
    // 파티클 질량 설정
    _Particle.Mass = 1.f;

    // 파티클 초기 속도 추가
    _Particle.Velocity = (float4) 0.f;

    if (0 != _Module.Module[2])
    {
        float3 vRandom = GetRandom(_NomalizedThreadID + 0.2f);

        float3 fSpeed = _Module.AddMinSpeed + (_Module.AddMaxSpeed - _Module.AddMinSpeed) * vRandom;

        // Random
        if(0 == _Module.AddVelocityType)
            _Particle.Velocity.xyz = fSpeed;
        else if (1 == _Module.AddVelocityType)                        
            _Particle.Velocity.xyz = normalize(vRandom - 0.5f) * fSpeed;
        // FromCenter
        else if (2 == _Module.AddVelocityType)                        
            _Particle.Velocity.xyz = normalize(_Particle.LocalPos) * fSpeed;
        // ToCenter
        else if (3 == _Module.AddVelocityType)
            _Particle.Velocity.xyz = -normalize(_Particle.LocalPos) * fSpeed;
        // Fixed
        else
            _Particle.Velocity.xyz = normalize(_Module.AddVelocityFixedDir) * fSpeed;
    }

    // Particle 의 World 좌표 계산
    _Particle.WorldPos.xyz = _Particle.LocalPos.xyz + _Module.ObjectWorldPos;
}


RWStructuredBuffer<FParticleData> gBuffer : register(u0);
RWStructuredBuffer<FParticleSpawn> gSpawnCount : register(u1);
// ParticleDataRegister는 렌더링 시에는 ParticleData를 담는 데이터를 SRV에 담기위한 용도로 사용되지만,
// 컴퓨트 셰이더에서는 같은 파티클을 다루는 역할이라는 의미로 해당 레지스터를 사용
StructuredBuffer<FParticleModule> gModule : register(ParticleDataRegister);

#define MaxParticleCount 500

[numthreads(256, 1, 1)]
void CS_TickParticle(int3 ThreadID : SV_DispatchThreadID)
{
	if (MaxParticleCount <= ThreadID.x)
	{
		return;
	}

    if (gBuffer[ThreadID.x].Active == 0)
    {
        int Success = 0;

        while (0 < gSpawnCount[0].SpawnCount)
        {
            int DestValue = gSpawnCount[0].SpawnCount;
            int Value = gSpawnCount[0].SpawnCount - 1;
            int OriginalValue = 0;

            InterlockedCompareExchange(gSpawnCount[0].SpawnCount, DestValue, Value, OriginalValue);
            //InterlockedExchange(g_SpawnCount[0].SpawnCount, Value, OriginalValue);

            // 교환 성공
            if (OriginalValue == DestValue)
            {
                Success = 1;
            }
        }    

        // 활성화된 파티클 초기화 작업
        if (Success)
        {
            float NormalizedThreadID = (float) ThreadID.x / (float)MaxParticleCount;
            ParticleInit(gBuffer[ThreadID.x], gModule[0], NormalizedThreadID);
        }
    }

    // 파티클 업데이트
    else
    {
		// NormalizedAge (0~1)
        gBuffer[ThreadID.x].NormalizedAge = gBuffer[ThreadID.x].Age / gBuffer[ThreadID.x].Life;

        // 이번 프레임의 누적된 힘을 초기화
        gBuffer[ThreadID.x].Force.xyz = float3(0.0f, 0.0f,0.0f);

        //TODO: 속도관련 이동이 있을 시 이곳에 추가 구현하기

        // 속도에 따른 이동
        float Accel = gBuffer[ThreadID.x].Force / gBuffer[ThreadID.x].Mass;

        // Module[0] : Space
		// 0 : Local Space
		// 1 : World Space
        if(0 == gModule[0].SpaceType)
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
		if(gModule[0].Module[3])
		{
			float CurScale = (gModule[0].EndScale - gModule[0].StartScale) * gBuffer[ThreadID.x].NormalizedAge + gModule[0].StartScale;
			gBuffer[ThreadID.x].WorldScale = gBuffer[ThreadID.x].WorldInitScale * CurScale;
		}

        // Drag 감속 모듈
		if (gModule[0].Module[4])
		{
			gBuffer[ThreadID.x].Velocity += gModule[0].AddTickVelocity * gDeltaTime;
		}


        // 렌더링 관련 옵션들
        if(gModule[0].Module[5])
        {
			gBuffer[ThreadID.x].Color = (gModule[0].EndColor - gModule[0].StartColor) * gBuffer[ThreadID.x].NormalizedAge + gModule[0].StartColor;
            if(gModule[0].FadeOut && gBuffer[ThreadID.x].NormalizedAge >= gModule[0].StartRatio)
            {
	            float fRatio = saturate(1.f - (gBuffer[ThreadID.x].NormalizedAge - gModule[0].StartRatio) / (1.f - gModule[0].StartRatio));

                gBuffer[ThreadID.x].Color.a = gModule[0].EndColor.a * fRatio;
            }

        }

        // UVAnim
        if(gModule[0].Module[6])
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
        if(gModule[0].Module[7])
        {
	        gBuffer[ThreadID.x].WorldRotation += gModule[0].AddRotation * gDeltaTime;
        }

		gBuffer[ThreadID.x].Age += gDeltaTime;
		// 파티클의 수명이 다했는지 체크
		if (gBuffer[ThreadID.x].Age >= gBuffer[ThreadID.x].Life)
		{
			gBuffer[ThreadID.x].Active = 0;
			gBuffer[ThreadID.x].Age = 0.f;
            return;
		}

    }
}


#ifndef _DECAL
#define _DECAL

#include "Global.fx"
#include "MaterialNode.fx"

SamplerState samLinear : register( s0 );

Texture2D    POSITION_TARGET : register( t10 );
Texture2D    DecalTexture : register( t0 );

cbuffer cbDecal : register( b7 )
{
	float4x4 Dummy1;

	int gDecalIsLight;
	float3 Pad;
};

struct VS_IN
{
	float3 vPos : POSITION;
};

struct VS_OUT
{
	float4 Position : SV_Position;
};

VS_OUT VS_Decal(VS_IN _in)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.Position = mul(mul(mul(float4(_in.vPos, 1.f), World),gView),gProjection);

	return output;
}

struct PS_OUT
{
	float4 Color : SV_Target0;
	float4 Emissive : SV_Target1;
};

PS_OUT PS_Decal(VS_OUT _in)
{
	PS_OUT output = (PS_OUT) 0.f;

	float2 vScreenUV = _in.Position.xy / gResolution;
	float4 vViewPos = POSITION_TARGET.Sample(samLinear, vScreenUV);

	// 해당 영역에 존재하는 물체가 없다
	if (vViewPos.x == 0.f && vViewPos.y == 0.f && vViewPos.z == 0.f)
	{
		discard;
	}

	float3 vLocalPos = mul(mul(float4(vViewPos.xyz, 1.f), gViewInv), WorldInv).xyz;
	if (vLocalPos.x < -0.5f || 0.5f < vLocalPos.x 
	|| vLocalPos.y < -0.5f || 0.5f < vLocalPos.y 
	|| vLocalPos.z < -0.5f || 0.5f < vLocalPos.z)
	{
		discard;
	}

	float2 UV = float2(vLocalPos.x + 0.5f, 1.f - (vLocalPos.z + 0.5f));

	float4 DecalColor = DecalTexture.Sample(samLinear, UV);

	if (gDecalIsLight)
	{
		DecalColor.rgb *= DecalColor.a;
		output.Emissive = DecalColor;
	}
	else
	{
		output.Color = DecalColor;
		output.Emissive = float4(0.0f,0.0f,0.0f,0.0f);
	}

	return output;
}

cbuffer cbSkillRange : register(b4)
{
	float Progress; // 스킬 준비 진행도 (0~1)
	float BorderThickness; // 테두리 두께
	
	// 현재 구현의 편의를 위해 float 만 가능함
	float BaseColorR;
	float BaseColorG;
	float BaseColorB;
	float ActiveColorR;
	float ActiveColorG;
	float ActiveColorB;

	
	// 부채꼴의 반각(Ded)
	float HalfAngleDeg;
	// ForwardX/Z 를 통해서 부채꼴의 정면을 구함
	float ForwardX;
	float ForwardZ;
}


// 부채꼴 범위 안에 있는지 확인하는 용도
bool IsInsideFanShape(float2 UV, float2 Center, float Radius, float2 Forward, float HalfAngleRad)
{
	float2 Dir = UV - Center;
	float Dist = length(Dir);
    
    // 반경 밖이면 제외
	if (Dist > Radius)
		return false;
    
    // 정규화된 방향
	float2 NormalizedDir = normalize(Dir);
    
    // Forward 벡터와 현재 방향 사이의 각도 계산
	float DotProduct = dot(NormalizedDir, normalize(Forward));
	float Angle = acos(DotProduct);
    
    // HalfAngle 내부에 있으면 true
	return (Angle <= HalfAngleRad);
}

PS_OUT PS_SkillRangeDecal(VS_OUT _in)
{
	PS_OUT output = (PS_OUT) 0.f;

	float2 vScreenUV = _in.Position.xy / gResolution;
	float4 vViewPos = POSITION_TARGET.Sample(samLinear, vScreenUV);

    // 해당 영역에 존재하는 물체가 없다
	if (vViewPos.x == 0.f && vViewPos.y == 0.f && vViewPos.z == 0.f)
	{
		discard;
	}

	float3 vLocalPos = mul(mul(float4(vViewPos.xyz, 1.f), gViewInv), WorldInv).xyz;
	if (vLocalPos.x < -0.5f || 0.5f < vLocalPos.x 
    || vLocalPos.y < -0.5f || 0.5f < vLocalPos.y 
    || vLocalPos.z < -0.5f || 0.5f < vLocalPos.z)
	{
		discard;
	}

	float Radius = 0.5f;
	float4 BaseColor = float4(BaseColorR, BaseColorG, BaseColorB, 1.0f); // 주황색
	float4 ActiveColor = float4(ActiveColorR, ActiveColorG, ActiveColorB, 1.0f); // 빨간색
	float4 BorderColor = float4(1.0, 1.0, 1.0, 1.0f); // 노란색 테두리

	// 월드 좌표는 Z+ 가 ForwardVector 이므로
	float2 UV = float2(vLocalPos.x + 0.5f, vLocalPos.z + 0.5f);
	float2 Center = float2(0.5f, 0.5f);

    // 거리 계산
	float Dist = distance(UV, Center);
    
    // 원형 영역 바깥은 완전히 렌더링하지 않음
	if (Dist > Radius)
	{
		discard;
	}

	// 부채꼴 내부인지 확인
	float2 Forward = normalize(float2(ForwardX, ForwardZ));
	float HalfAngleRad = radians(HalfAngleDeg);
	if (!IsInsideFanShape(UV, Center, Radius, Forward, HalfAngleRad))
	{
		discard;
	}
    
    // 테두리 마스크 계산 - 다른 접근 방식
    // 1. 원의 가장자리에 가까울수록 1, 멀어질수록 0
	float edgeDistance = Radius - Dist;
	float BorderMask = 1.0 - smoothstep(0.0, BorderThickness, edgeDistance);
    
    // 진행도에 따른 채움 효과 - Radius까지 완전히 채워지도록
	float FillRadius = Progress * Radius;
	float FillMask = step(Dist, FillRadius);
    
    // 기본 색상 (주황색)과 활성 색상(빨간색) 혼합
	float4 FillColor = lerp(BaseColor, ActiveColor, FillMask);
    
    // 테두리 효과 - 노란색 그라데이션
	float BorderGlow = (sin(gTime * 5.0) * 0.5 + 0.5) * 0.3; // 시간에 따른 반짝임
    
    // 노란색 테두리로 그라데이션 효과
	float NormalizedDist = Dist / Radius;
	float gradientPower = 2.0; // 그라데이션 강도
	float gradientEffect = pow(NormalizedDist, gradientPower);
    
    // 최종 색상 계산
	float4 FinalColor;
    
    // 테두리 부분은 노란색 그라데이션
	if (BorderMask > 0.01)
	{
        // 테두리 영역
		FinalColor = lerp(FillColor, BorderColor, BorderMask * (0.7 + BorderGlow));
	}
	else
	{
        // 내부 영역
		FinalColor = FillColor;
	}
    
    // 발광 효과 설정
	if (gDecalIsLight)
	{
        // 발광 효과로 처리
		output.Emissive = FinalColor;
		output.Color = float4(0, 0, 0, 0);
	}
	else
	{
        // 일반 색상으로 처리
		output.Color = FinalColor;
        
        // 테두리만 발광 효과
		float emissiveIntensity = BorderMask * (0.5 + BorderGlow);
		output.Emissive = float4(BorderColor.rgb * emissiveIntensity, 0.0f);
	}

	return output;
}

#endif
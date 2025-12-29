#ifndef __MaterialNode_FX__
#define __MaterialNode_FX__

// UV를 이동시켜서 텍스쳐가 움직이는 효과를 보여주기 위한 용도
float2 Panner(in float2 CurUV, in float Time, in float2 Speed)
{
	return CurUV + Time * Speed;
}

// 특정 위치를 중심으로 원형범위 마스크를 만드는 함수
// 중심에서 반지름 까지는 1을 반환, 경계에서는 0으로 떨어지는 값 반환
float RadialGradientExponential(in float2 UVs, in float2 CenterPosition, in float Radius, in float Density)
{
	// 중심에서 현재 UV까지의 거리 계산
	float Dist = distance(UVs, CenterPosition);

	// 거리/radius로 경계 결정, density로 부드러움 조절
	float Mask = exp(-pow(Dist / Radius, Density));

	return Mask;
}

// Fresnel -> 표면의 법선 방향과 시선 방향의 각도에 따라서 반사율이 달라지는 현상
float Fresnel(in float3 Normal, in float3 WorldPixelPos, in float4x4 CameraViewInvMat, in float Exponent, in float BaseReflectFraction)
{
	float3 CameraWorldPos = CameraViewInvMat[3].xyz;
	float3 ViewDir = normalize(CameraWorldPos - WorldPixelPos);

	Normal = normalize(Normal);

	// Fresnel 계산
	float Fresnel = BaseReflectFraction + (1.0 - BaseReflectFraction) * pow(1.0 - dot(Normal, ViewDir), Exponent);
	return Fresnel;
}

float3 Fresnel(float3 Normal,float3 ViewDirection,float Exponent = 5.0,float BaseReflectFraction = 0.04)
{
    // 벡터 정규화
	float3 N = normalize(Normal);
	float3 V = normalize(ViewDirection);
    
    // 내적 계산 (코사인 각도)
	float NDotV = saturate(dot(N, V));
    
    // 프레넬 계산
	float FresnelTerm = pow(1.0 - NDotV, Exponent);
    
    // 슐릭 근사 (Schlick's approximation)
	float Fresnel = BaseReflectFraction + (1.0 - BaseReflectFraction) * FresnelTerm;
    
	return saturate(Fresnel);
}

float3 CalculateViewDirection(float3 WorldPosition)
{
	float3 CameraWorldPos = mul(float4(0, 0, 0, 1), gViewInv).xyz;
	return normalize(CameraWorldPos - WorldPosition);
}
#endif
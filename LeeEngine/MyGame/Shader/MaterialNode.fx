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

// Note: AI를 활용해 함수 구현
float3 SimpleGrassWind(float3 worldPos, float3 normal, float2 texCoord, float time)
{
    // 🌿 강화된 바람 파라미터
	float windIntensity = 2.0f;
	float windWeight = 1.0f;
	float windSpeed = 1.5f;
    
    // 🌪️ 복합적인 바람 방향 (시간에 따라 변화)
	float3 primaryWind = float3(1.0, 0.0, 0.3);
	float3 secondaryWind = float3(0.7, 0.0, -0.4);
	float windDirBlend = sin(time * 0.3) * 0.5 + 0.5;
	float3 windDir = normalize(lerp(primaryWind, secondaryWind, windDirBlend));
    
    // 🌊 복잡한 바람 패턴 (여러 레이어)
	float timeOffset = time * windSpeed;
	float spatialX = worldPos.x * 0.05;
	float spatialZ = worldPos.z * 0.08;
    
    // 메인 바람 (큰 움직임)
	float mainWind = sin(timeOffset + spatialX + spatialZ);
    
    // 세부 바람 (작은 떨림)
	float detailWind1 = sin(timeOffset * 3.7 + spatialX * 2.1) * 0.4;
	float detailWind2 = sin(timeOffset * 6.3 + spatialZ * 1.8) * 0.2;
    
    // 돌풍 효과 (간헐적인 강한 바람)
	float gustPhase = time * 0.8 + worldPos.x * 0.02;
	float gust = pow(saturate(sin(gustPhase)), 8.0) * 1.5;
    
    // 🎭 최종 바람 노이즈 조합 (-1 ~ 1 범위 유지)
	float windNoise = mainWind + detailWind1 + detailWind2 + gust;
    
    // 🍃 높이 기반 가중치 (아래쪽은 덜 움직임)
	float heightInfluence = pow(saturate(texCoord.y), 0.8);
    
    // 🌱 노멀 영향 완화 (모든 방향에서 어느 정도 움직임)
	float normalInfluence = saturate(dot(normal, float3(0, 1, 0))) * 0.3 + 0.7;
    
    // ⬆️ Y축 움직임 추가 (핵심!)
	float verticalWind = sin(timeOffset * 1.2 + spatialX * 1.5 + spatialZ * 0.8) * 0.4;
	verticalWind += sin(timeOffset * 2.8 + worldPos.y * 0.1) * 0.2; // 높이별 차등
    
    // 🌀 3D 바람 벡터 생성
	float3 finalWindDir = float3(
        windDir.x, // 기존 X축
        verticalWind, // 새로운 Y축 움직임
        windDir.z               // 기존 Z축
    );
    
    // 최종 바람 오프셋
	float3 windOffset = finalWindDir * windIntensity * windNoise * heightInfluence * normalInfluence;
    
	return windOffset;
}
#endif
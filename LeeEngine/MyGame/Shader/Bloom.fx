#ifndef __BLOOM_FX_
#define __BLOOM_FX_
#include "PostProcess.fx"
// -> In PostProcess.fx
//Texture2D    SceneTexture : register( t0 );
//SamplerState DefaultSampler : register( s0 );
SamplerState ClampSampler : register(s2);
Texture2D	   EmissiveTexture : register(t1);

float3 GaussianBlur(float2 uv, float2 pixelSize)
{
	// 가우시안 커널 값 (대략적인 값, 실제로는 더 정교하게 계산)
	static const float weights[5] = {0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f};
	float3 color = EmissiveTexture.Sample(ClampSampler, uv).rgb * weights[0];

	for(int i = 1; i < 5; ++i)
	{
		color += EmissiveTexture.Sample(ClampSampler, uv + float2(pixelSize.x * i, 0)).rgb * weights[i];
		color += EmissiveTexture.Sample(ClampSampler, uv - float2(pixelSize.x * i, 0)).rgb * weights[i];
		color += EmissiveTexture.Sample(ClampSampler, uv + float2(0, pixelSize.y * i)).rgb * weights[i];
		color += EmissiveTexture.Sample(ClampSampler, uv - float2(0, pixelSize.y * i)).rgb * weights[i];
	}
	return color;
}

float4 PS_MakeEmissiveDownSampling(VS_OUT Input) : SV_TARGET
{
	float2 ScreenUV = Input.Position.xy / (gResolution.xy/4);
	return float4(EmissiveTexture.Sample(DefaultSampler,ScreenUV).rgb,1.0f);
}


float4 PS_Bloom(VS_OUT Input) : SV_TARGET
{
	float2 ScreenUV = Input.Position.xy / gResolution.xy;
	
	// 1. SceneTexture에서 최종 색상 샘플
	float3 sceneColor = SceneTexture.Sample(DefaultSampler, ScreenUV).rgb;

	// 2. EmissiveTexture에서 Bloom 추출
	float2 Dimension = gResolution / 4;
	float2 pixelSize = 1.0 / Dimension;
	float3 blur = GaussianBlur(ScreenUV, pixelSize);

	const float bloomIntensity = 0.08f;
	blur *= bloomIntensity;
	// 3. 합성
	float3 finalColor = sceneColor + blur;
	return float4(blur,1.0f);

	// 5. 출력
	//return float4(finalColor, 1.0f);
}
#endif
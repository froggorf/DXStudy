#ifndef __BLOOM_FX_
#define __BLOOM_FX_
#include "PostProcess.fx"
// -> In PostProcess.fx
//Texture2D    SceneTexture : register( t0 );
//SamplerState DefaultSampler : register( s0 );
SamplerState ClampSampler : register(s2);
Texture2D	   EmissiveTexture : register(t1);

float3 Blur(float2 uv, float2 pixelSize)
{
	static const float kernel[7][7] = {
		{0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067},
		{0.00002292, 0.00078634, 0.00655798, 0.01331842, 0.00655798, 0.00078634, 0.00002292},
		{0.00019117, 0.00655798, 0.05472157, 0.11116507, 0.05472157, 0.00655798, 0.00019117},
		{0.00038771, 0.01331842, 0.11116507, 0.22508352, 0.11116507, 0.01331842, 0.00038771},
		{0.00019117, 0.00655798, 0.05472157, 0.11116507, 0.05472157, 0.00655798, 0.00019117},
		{0.00002292, 0.00078634, 0.00655798, 0.01331842, 0.00655798, 0.00078634, 0.00002292},
		{0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067}
	};

	float3 color = float3(0,0,0);
	[unroll]
	for(int y = -3; y <= 3; ++y)
	{
		[unroll]
		for(int x = -3; x <= 3; ++x)
		{
			color += EmissiveTexture.Sample(ClampSampler, uv + float2(pixelSize.x * x, pixelSize.y * y)).rgb * kernel[y+3][x+3];
		}
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
	float2 Dimension = gResolution;
	float2 pixelSize = 1.0 / Dimension;
	float3 blur = Blur(ScreenUV, pixelSize);

	const float bloomIntensity = 0.5f;
	blur *= bloomIntensity;
	// 3. 합성
	float3 finalColor = sceneColor + blur;
	//return float4(blur,1.0f);

	// 5. 출력
	return float4(finalColor, 1.0f);
}
#endif
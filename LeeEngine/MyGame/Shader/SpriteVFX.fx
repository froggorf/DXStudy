#ifndef _SPRITE_VFX_FX_
#define _SPRITE_VFX_FX_
#include "SpriteRenderer.fx"
#include "MaterialNode.fx"

float4 PS_FireBall(GS_OUT Input) : SV_Target
{
	float2 NewNoiseUV = Panner(Input.UV, gTime, float2(0.2,1.0));

	// T_FireNoise
	float4 FireNoiseColor = Texture0.Sample(DefaultSampler, NewNoiseUV);
	FireNoiseColor *= FireNoiseColor.a;

	// TODO: 필요시 파라미터화
	float NoisePower = 0.15;
	FireNoiseColor *= NoisePower;

	float OffsetX = -0.08866;
	float OffsetY = 0.8;
	float2 SmokeUV = Input.UV + float2(OffsetX, OffsetY);
	SmokeUV = SmokeUV + float2(FireNoiseColor.r,FireNoiseColor.r);

	float4 ParticleColor = gParticle[Input.InstID].Color;
	float3 FireSmokeColor = Texture1.Sample(DefaultSampler, SmokeUV);

	float AlphaCorrect = 13.0f;
	float3 EmissiveColor = ParticleColor.rgb * pow(FireSmokeColor.r, AlphaCorrect);
	
	float Radius = 0.4f;
	float RadialMask = RadialGradientExponential(Input.UV, float2(0.5,0.5), Radius, 8.0f);
	float AlphaMask = FireSmokeColor.b * RadialMask;
	float NewAlpha = ParticleColor.a * AlphaMask;
	
	float4 Color = float4(EmissiveColor.rgb, NewAlpha);
	return Color;
}

float4 PS_FireFlash(GS_OUT Input) : SV_Target
{
	float4 DynamicParam = gParticle[Input.InstID].DynamicParam;
	float Flashing = DynamicParam.x;
	float LightPower = DynamicParam.y;
	float FlashRadiance = clamp(sin(gTime*Flashing) * LightPower, 0.1, 42.195);

	float4 Color = gParticle[Input.InstID].Color * FlashRadiance;

	float TextureAlpha = Texture0.Sample(DefaultSampler, Input.UV).a;
	float Alpha = TextureAlpha * Color.a;

	return float4(Color.rgb, Alpha);
}

#endif
#ifndef _SPRITE_VFX_FX_
#define _SPRITE_VFX_FX_
#include "SpriteRenderer.fx"
#include "MaterialNode.fx"

float4 PS_FireBall(GS_OUT Input) : SV_Target
{
	float2 NoiseUV0 = Panner(Input.UV, gTime, float2(0.35, 1.1));
	float2 NoiseUV1 = Panner(Input.UV, gTime, float2(-0.2, 0.9));

	float Noise0 = Texture0.Sample(DefaultSampler, NoiseUV0).r;
	float Noise1 = Texture0.Sample(DefaultSampler, NoiseUV1).g;
	float Noise = saturate((Noise0 + Noise1) * 0.5);

	float2 SmokeUV = Input.UV + float2(Noise * 0.15, Noise * 0.25);
	float3 Smoke = Texture1.Sample(DefaultSampler, SmokeUV).rgb;

	float4 ParticleColor = gParticle[Input.InstID].Color;

	float Radius = 0.45f;
	float RadialMask = RadialGradientExponential(Input.UV, float2(0.5,0.5), Radius, 5.0f);

	float Heat = saturate(Smoke.r * 1.2 + Noise * 0.25);
	float Core = saturate(pow(Heat, 2.5) * 1.2);
	float Mid = saturate(pow(Heat, 1.4));

	float3 CoreColor = float3(18.0f, 9.5f, 1.8f);
	float3 MidColor = float3(6.0f, 1.4f, 0.1f);
	float3 EdgeColor = float3(1.2f, 0.12f, 0.02f);

	float3 FireColor = lerp(EdgeColor, MidColor, Mid);
	FireColor = lerp(FireColor, CoreColor, Core);

	float Flicker = 0.7f + 0.3f * sin(gTime * 10.0f + Noise * 6.2831f);
	FireColor *= RadialMask * Flicker;
	FireColor *= ParticleColor.rgb;

	float Highlight = Smoke.g * (0.8f + Noise * 0.2f);
	float3 HighlightColor = float3(4.0f, 1.2f, 0.1f) * Highlight * RadialMask;

	float Alpha = Smoke.b * RadialMask * (0.7f + Noise * 0.3f) * ParticleColor.a;

	return float4(FireColor + HighlightColor, Alpha);
}

float4 PS_FireFlash(GS_OUT Input) : SV_Target
{
	float4 DynamicParam = gParticle[Input.InstID].DynamicParam;
	float Flashing = DynamicParam.x;
	float LightPower = DynamicParam.y;
	float FlashRadiance = clamp(sin(gTime*Flashing) * LightPower, 0.1, 42.195);

	float4 Color = gParticle[Input.InstID].Color * FlashRadiance;
	Color.rgb *= float3(1.1f, 0.6f, 0.2f);

	float TextureAlpha = Texture0.Sample(DefaultSampler, Input.UV).a;
	float Alpha = TextureAlpha * Color.a;

	return float4(Color.rgb, Alpha);
}



#endif

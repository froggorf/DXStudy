#include "RibbonRenderer.fx"


float4 PS_Lightning(VS_OUTPUT Input) : SV_Target
{
	float TexUCount = 4;
	float ChangeSpeed = 5.0f;
	//float NewV = floor(frac(gTime) * TexVCount) * (1.0f / TexVCount) + UV.y / TexVCount;
	float2 NewUV = float2(Input.Tex.y, Input.Tex.x);
	NewUV.x = floor(frac(gTime * ChangeSpeed) * TexUCount) * (1.0f/TexUCount) + NewUV.x / TexUCount;

	float4 TextureSample = Texture0.Sample(DefaultSampler, NewUV);
	TextureSample.rgb *= Input.ParticleColor;
	return TextureSample;
}

#include "MeshRenderer.fx"
#include "MaterialNode.fx"
float4 MF_TexController(in GS_OUT Input, in Texture2D Tex, in float UTilling, in float VTilling, in float SpeedX, in float SpeedY, 
	in float OffsetX, in float OffsetY, in float TexPower, in float TexMultiply)
{
	float2 NewUV = Input.UV * float2(UTilling, VTilling);
	
	NewUV = NewUV + float2(SpeedX, SpeedY) * gTime + float2(OffsetX, OffsetY);
	
	float4 TexColor = Tex.Sample(DefaultSampler, NewUV);	
	TexColor.rgb = pow(TexColor.rgb, TexPower);
	TexColor.rgb = TexColor.rgb * TexMultiply;
	return TexColor;
}

float4 MF_TexController_Primitive(in GS_OUT Input, in Texture2D Tex, in float UTilling, in float VTilling, in float SpeedX, in float SpeedY, in float ExtendValue)
{
	float2 NewUV = Input.UV * float2(UTilling, VTilling);
	NewUV += float2(SpeedX, SpeedY) * gTime;
	NewUV += ExtendValue;

	return Tex.Sample(DefaultSampler, NewUV);
}

float MF_MaskToTopUnder(in float2 UV, in float TopMaskPower, in float TopMaskMultiply, in float LowerMaskPower, in float LowerMaskMultiply)
{
	float TopMask = pow(UV.y, TopMaskPower) * TopMaskMultiply;
	float UnderMask = clamp(pow(1 - UV.y, LowerMaskPower) * LowerMaskMultiply, 0.0f, 1.0f);
	return clamp(TopMask * UnderMask, 0.0f, 1.0f);
}

float4 PS_Storm(GS_OUT Input) : SV_Target
{
	float4 ReturnColor = float4(1,0,1,1);
	float4 ParticleColor = gParticle[Input.InstID].Color;

	// TileU, TileV, SpeedX, SpeedY
	float4 NoiseControl = float4(2.0f,1.0f,1.5f,1.0f);
	float NoiseDensity = 0.5f;
	float NoisePower = 0.6f;

	float4 OutNoise = MF_TexController(Input, Texture0, NoiseControl.r, NoiseControl.g, NoiseControl.b, NoiseControl.a,
					0.0f, 0.0f, NoiseDensity, NoisePower);

	// TileU, TileV, SpeedX, SpeedY
	float4 BaseTexControl = float4(2.0f, 2.0f, 0.3f, 2.0f);
	float4 OutBaseTexControl = MF_TexController_Primitive(Input, Texture0, BaseTexControl.r, BaseTexControl.g, BaseTexControl.b, BaseTexControl.a, OutNoise.g);
	float BaseColorDecay = 5.0f;
	float BaseColorAlpha = pow(OutBaseTexControl.r, BaseColorDecay);
	float BaseColorDensity = 3.0f;
	float BaseColorDensityValue = pow(OutBaseTexControl.r, BaseColorDensity);
	// Main Emissive Color
	{
		// TileU, TileV, SpeedX, SpeedY
		float4 SubColorControl = float4(1.0f, 2.0f, 0.0f, 3.0f);
		float4 SubColorTexControl = MF_TexController_Primitive(Input, Texture0, SubColorControl.r, SubColorControl.g, SubColorControl.b, SubColorControl.a, 0.0f);
		float SubColorDensity = 1.3f;

		// Param화 시키기
		float4 SubColor = float4(0.0f, 16.0f, 25.0f, 1.0f);
		ReturnColor.rgb = ParticleColor.rgb * BaseColorDensityValue + SubColor.rgb * pow(SubColorTexControl, SubColorDensity);
	}

	// New Alpha
	{
		ReturnColor.a = BaseColorDensityValue;
	}
	// 기존 Alpha 무시
	//// Alpha
	//{
	//	float MaskTopDensity = 1.2f;
	//	float MaskTopPower = 4.0f;
	//	float MaskLowerDensity = 0.0f;
	//	float MaskLowerPower = 1.0f;
	//	float MaskTopAndLower = clamp(MF_MaskToTopUnder(Input.UV, MaskTopDensity, MaskTopPower, MaskLowerDensity,MaskLowerPower), 0.0f, 1.0f);
	//	ReturnColor.a = ParticleColor.a * (BaseColorAlpha + 1.0f) * MaskTopAndLower;
	//	ReturnColor.a *= 0.7f;
	//}
	
	return ReturnColor;
}


float4 PS_Storm_Sub(GS_OUT Input) : SV_Target
{
	float4 ReturnColor = float4(1, 0, 1, 1);
	float4 ParticleColor = gParticle[Input.InstID].Color;

	// TileU, TileV, SpeedX, SpeedY
	float4 NoiseControl = float4(2.0f, 1.0f, 1.5f, 1.0f);
	float NoiseDensity = 0.5f;
	float NoisePower = 0.6f;

	float4 OutNoise = MF_TexController(Input, Texture0, NoiseControl.r, NoiseControl.g, NoiseControl.b, NoiseControl.a,
					0.0f, 0.0f, NoiseDensity, NoisePower);

	// TileU, TileV, SpeedX, SpeedY
	float4 BaseTexControl = float4(2.0f, 2.0f, 0.3f, 2.0f);
	float4 OutBaseTexControl = MF_TexController_Primitive(Input, Texture0, BaseTexControl.r, BaseTexControl.g, BaseTexControl.b, BaseTexControl.a, OutNoise.g);
	float BaseColorDecay = 5.0f;
	float BaseColorAlpha = pow(OutBaseTexControl.r, BaseColorDecay);
	// Main Emissive Color
	{
		// TileU, TileV, SpeedX, SpeedY
		float4 SubColorControl = float4(1.0f, 2.0f, 0.0f, 3.0f);
		float4 SubColorTexControl = MF_TexController_Primitive(Input, Texture0, SubColorControl.r, SubColorControl.g, SubColorControl.b, SubColorControl.a, 0.0f);
		float SubColorDensity = 1.3f;

		// Param화 시키기
		float4 SubColor = float4(0.0f, 8.0f, 50.0f, 1.0f);
		float3 EmissiveColorA = float3(0.0f, 0.0f, 0.0f);
		float BaseColorDensity = 3.0f;
		float3 EmissiveColorB = ParticleColor.rgb * pow(OutBaseTexControl.r, BaseColorDensity) + SubColor.rgb * pow(SubColorTexControl, SubColorDensity);
		ReturnColor.rgb = lerp(EmissiveColorA, EmissiveColorB, BaseColorAlpha);
		ReturnColor.a = BaseColorAlpha;
	}
	
	return ReturnColor;
}
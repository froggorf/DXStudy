#ifndef __GLOBAL_FX__
#define __GLOBAL_FX__

#define PI 3.141592
cbuffer cbPerFrame : register(b0)
{
	row_major matrix gView;
	row_major matrix gViewInv;
	row_major matrix gProjection;

	float2			 gResolution;
	float            gTime;
	float            gDeltaTime;
	int				 gLightCount;

	float3           Padding;
}

cbuffer cbPerObject : register(b1)
{
	matrix   World;
	row_major matrix WorldInv;
	row_major float4x4	gMatWV;
	float4x4 WorldInvTranspose;
};

// 필요시 사용하는 값
cbuffer SystemParam : register(b6)
{
	int4 bTexBind_0_3;		// bTexBind[0~3]
	int4 bTexBind_4_7;		// bTexBind[4~7]
	int4 bTexBind_8_11;		// bTexBind[8~11]
	int4 bTexBind_12_15;	// bTexBind[12~15]

	float4 Float4_1;
	float4 Float4_2;
	float4 Float4_3;
	float4 Float4_4;

	int Bool_1;
	int Bool_2;
	int Bool_3;
	int Bool_4;

	int Int_1;
	int Int_2;
	int Int_3;
	int Int_4;

	float Float_1;
	float Float_2;
	float Float_3;
	float Float_4;
}

// 톤매핑 함수 - ACES Film
float3 ACESFilm(float3 x)
{
	float a = 2.51;
	float b = 0.03;
	float c = 2.43;
	float d = 0.59;
	float e = 0.14;
	return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

#endif
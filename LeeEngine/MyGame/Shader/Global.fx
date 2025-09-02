#include "LightHelper.hlsl"

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



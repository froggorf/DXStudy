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




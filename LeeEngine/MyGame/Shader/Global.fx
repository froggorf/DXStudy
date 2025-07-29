#include "LightHelper.hlsl"

cbuffer cbPerFrame : register(b0)
{
	row_major matrix gView;
	row_major matrix gProjection;

	float            gTime;
	float            gDeltaTime;
	int				 gLightCount;
	float           Padding;
}

cbuffer cbPerObject : register(b1)
{
	matrix   World;
	float4x4 WorldInvTranspose;
	Material ObjectMaterial;
};

cbuffer cbLight : register(b2)
{
	DirectionalLight gDirLight;
	PointLight       gPointLight;
	float3           gEyePosW;
	float            pad;
}


StructuredBuffer<FLightInfo> g_Light3DBuffer : register(t14);

#define PI 3.141592

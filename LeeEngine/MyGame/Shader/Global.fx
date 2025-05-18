#include "LightHelper.hlsl"

cbuffer cbPerFrame : register(b0)
{
	row_major matrix gView;
	row_major matrix gProjection;
	matrix           gLightView;
	matrix           gLightProj;
	float            gTime;
	float            gDeltaTime;
	float2           Padding;
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

#define PI 3.141592

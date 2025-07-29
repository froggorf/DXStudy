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
	row_major float4x4	gMatWV;
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



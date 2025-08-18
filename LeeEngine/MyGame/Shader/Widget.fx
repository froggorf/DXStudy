#include "Global.fx"

Texture2D    UITexture : register( t0 );
SamplerState UISampler : register( s0 );

cbuffer cbWidgetRenderData : register( b2 )
{
	float4 Tint;
	//NDC 좌표계 기준
	float Left;
	float Top;
	float Width;
	float Height;
};

struct VS_IN
{
	float3 Pos : POSITION;
	float2 UV : TEXCOORD;
};

struct VS_OUT
{
	float4 Position : SV_Position;
	float2 UV : TEXCOORD;
};

VS_OUT VS_Widget(VS_IN Input)
{
	VS_OUT output = (VS_OUT) 0.f;

	float3 scaledPos;
	scaledPos.x = Left + (Input.Pos.x + 0.5) * abs(Width);

	float actualTop = Top;
	if (Height < 0.0f)
	{
		actualTop = Top + Height; 
	}

	scaledPos.y = actualTop + (Input.Pos.y + 0.5) * abs(Height);
	scaledPos.z = 0.0f;

	output.Position = float4(scaledPos, 1.0f);

	output.UV = Input.UV;

	return output;
}

float4 PS_Widget(VS_OUT _in) : SV_Target
{
	float4 OutColor = (float4) 0.f;
	float4 TextureColor = UITexture.Sample(UISampler, _in.UV);
	OutColor = TextureColor * Tint;
	return OutColor;
}
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
	scaledPos.y = Top - (Input.Pos.y + 0.5) * abs(Height);
	scaledPos.z = 0.0f;

	output.Position = float4(scaledPos, 1.0f);

	output.UV = Input.UV;
	output.UV.y = 1 - output.UV.y;

	return output;
}

float4 PS_Widget(VS_OUT _in) : SV_Target
{
	float4 OutColor = (float4) 0.f;
	float4 TextureColor = UITexture.Sample(UISampler, _in.UV);
	
	OutColor = TextureColor * Tint;
	return OutColor;
}

#define Radial_LeftToRight 0
#define Radial_RightToLeft 1
#define FillMode Int_1
#define FillTint Float4_1
#define FillValue Float_1

// radians 함수가 있음,
// Left->Right 기준으로
// FillValue == 0 (0도) / 0.25 (90도) / 0.5 (180도) / 0.75 (270도) / 1 (360도) 가 나오니까
// radians(FillValue * 360) 으로 각도를 파악한 후에
// cos와 sin 을 통해 각도를 알아낼 수 있을 것 같음.
float4 PS_RadialPBWidget(VS_OUT Input) : SV_Target
{
    float2 UV = Input.UV;
    float2 Center = float2(0.5f, 0.5f);

    // 현재 UV와 중앙과의 관계
    float2 Diff = UV - Center;

	// 그걸 가지고 Angle을 구함 ( -PI ~ PI )
    float Angle = atan2(-Diff.x, -Diff.y);
	// 0~2PI 범위로 변환 (if문 없이) // AI 조언
	Angle = fmod(Angle + 2 * PI, 2 * PI);

    // 진행률에 따라 채워질 각도
	// Left->Right 는 0~360 / Right->Left 는 360~0을 만들어야함
	// Left->Right 는 1-FillValue 를 곱해주고,
	// Right->Left 는 그대로 FillValue를 곱해주기
	float NewFillValue = lerp(1-FillValue, FillValue, FillMode);
    float FillAngle = NewFillValue * 2 * PI;

	// Angle < FillAngle -> 0  || else -> 1
	float Mask = step(Angle, FillAngle);

	// FillMode가 0(LeftToRight) 이면 1-Mask를 사용해서 Angle<FillAngle 인 곳에 FillTint 를 채우고
	//			  1(RightToLeft) 이면 Mask 를 사용해서 Angle > FillAngle 인 곳에 FillTint 를 채우기
	float FinalMask = lerp(1 - Mask, Mask, FillMode);

    float4 PixelTint = lerp(Tint, FillTint, FinalMask);

    float4 TextureColor = UITexture.Sample(UISampler, UV);
    return TextureColor * PixelTint;
}

#define Number Int_1

float4 PS_NumberWidget(VS_OUT Input) : SV_Target
{
	float2 NumberTextureUV =  Input.UV;
	// x-> 0~1 을 0.0~0.2 로 만들고서 offset을 더해줘야함
	NumberTextureUV.x = NumberTextureUV.x / 5 + 0.2 * (Number % 5);
	// y-> 0~1을 0.0~0.5 로 만들고서 Offset을 더해줘야함
	NumberTextureUV.y = NumberTextureUV.y / 2 + 0.5*(Number/5);

	float4 OutColor = (float4) 0.f;
	float4 TextureColor = UITexture.Sample(UISampler, NumberTextureUV);

	OutColor = TextureColor * Tint;
	return OutColor;
}
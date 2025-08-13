#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "Global.fx"
#include "TransformHelpers.hlsl"

TextureCube  CubeTexture : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 TexCoord : TEXCOORD;

};

struct VS_OUTPUT
{
	float4 PosScreen : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 LocalPos : POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT Input)
{
	VS_OUTPUT output = (VS_OUTPUT) 0.f;

	// Sphere Mesh Local 반지름을 1로 설정 (NDC 에서 1이 최대 깊이)
	Input.Pos *= 2.f;

	// View 변환의 회전변환만 적용시켜서 카메라가 바라보는 방향쪽에 있는 면이 Z 축 앞에 배치되게 함
	float4 ProjPos = mul(float4(Input.Pos.xyz, 0.f), gView);

	// 시야각에 맞게 투영시킴
	ProjPos = mul(ProjPos, gProjection);

	// 투영과정에서 Z 값이 NDC 공간상에서 최대가 될 수 있게 일부러 z 위치에 w 를 넣어둠
	// 레스터라이저가 xyz 를 w 로 나누기 때문
	ProjPos.z = ProjPos.w - 0.00005f;

	output.PosScreen = ProjPos;
	output.Tex = Input.TexCoord;    
	output.LocalPos = Input.Pos;    

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	float3 UV = normalize(input.LocalPos);
	float4 ObjectColor = CubeTexture.Sample(samLinear, UV);
	return ObjectColor;
}

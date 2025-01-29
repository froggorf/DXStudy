
Texture2D gTexture : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_OUTPUT
{
    float4 PosScreen : SV_POSITION;
    float2 Tex : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos: POSITION, float3 Normal:NORMAL, float2 Tex : TEXCOORD )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.PosScreen = float4(Pos.xyz,1.0f);

    output.Tex = Tex;

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    float4 color = gTexture.Sample( samLinear, input.Tex );

    return color;
}
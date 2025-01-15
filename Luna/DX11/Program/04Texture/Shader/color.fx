Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer cbPerFrame : register(b0)
{
	matrix View;
	matrix Projection;
}

cbuffer cbPerObject : register(b1)
{
	matrix World; 
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex.x = TexCoord.x;
    output.Tex.y = TexCoord.y;
    
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    float4 color = txDiffuse.Sample( samLinear, input.Tex );
    return color;
}

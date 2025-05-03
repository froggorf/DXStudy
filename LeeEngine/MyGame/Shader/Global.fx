
cbuffer cbPerFrame : register(b0)
{
    matrix gView;
    matrix gProjection;
    matrix gLightView;
    matrix gLightProj;
    float gTime;
    float3 Padding;
}

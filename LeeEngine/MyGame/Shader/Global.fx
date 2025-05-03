
#include "LightHelper.hlsl"
cbuffer cbPerFrame : register(b0)
{
    matrix View;
    matrix Projection;
    matrix LightView;
    matrix LightProj;
    float Time;
    float3 Padding;
}


cbuffer cbPerObject : register(b1)
{
    matrix World;
    float4x4 WorldInvTranspose;
    Material ObjectMaterial;
};

cbuffer cbLight : register(b2)
{
    DirectionalLight gDirLight;
    PointLight gPointLight;
    float3 gEyePosW;
    float pad;
}
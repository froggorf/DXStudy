#ifndef __TRANSFORMHELPER_FX_
#define __TRANSFORMHELPER_FX_
float4 CalculateScreenPosition(float4 position, float4x4 modelMat, float4x4 viewMat, float4x4 projMat)
{
	return mul(mul(mul(float4(position.xyz, 1.0f), modelMat),viewMat),projMat);
}
#endif
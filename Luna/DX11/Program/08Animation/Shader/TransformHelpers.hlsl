
float4 CalculateScreenPosition(float4 position, float4x4 modelMat, float4x4 viewMat, float4x4 projMat)
{
	float4 posScreen = float4(0.0f,0.0f,0.0f,0.0f);
	posScreen = mul(position, modelMat);
	posScreen = mul(posScreen, viewMat);
	posScreen = mul(posScreen, projMat);

	return posScreen;
}
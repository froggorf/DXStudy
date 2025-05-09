#define MAX_BONES 100
#define MAX_BONE_INFLUENCE 4

void CalculateSkinnedPosition(const in float4 Pos, const in float3 Normal, const in int4 boneIDs, const in float4 boneWeights, const in float4x4 BoneFinalTransforms[MAX_BONES], out float4 skinnedPosition, out float3 skinnedNormal)
{
	skinnedPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
	skinnedNormal = float3(0.0f, 0.0f, 0.0f);

	// bone에 따른 최종 위치 계산
	{
		for (int index = 0; index < MAX_BONE_INFLUENCE; ++index)
		{
			if(boneIDs[index] <0 || boneIDs[index] >= MAX_BONES)
			{
				continue;
			}

			float4x4 test = BoneFinalTransforms[boneIDs[index]];
			float weight = boneWeights[index];

			float4 localPosition = mul(Pos, BoneFinalTransforms[boneIDs[index]]);
			skinnedPosition += localPosition * weight;

			float3 localNormal = mul(Normal, BoneFinalTransforms[boneIDs[index]]);
			skinnedNormal += localNormal * weight;

		}
	}
}
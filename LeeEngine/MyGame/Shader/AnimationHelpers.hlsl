#define MAX_BONES 200
#define MAX_BONE_INFLUENCE 4

cbuffer cbSkeletalMeshBoneFinalTransforms : register(b3)
{
	matrix gBoneFinalTransforms[MAX_BONES];
}

void CalculateSkinnedPosition(const in float4 Pos, const in float3 Normal, const in float3 Tangent, const in float3 Binormal, const in int4 boneIDs, const in float4 boneWeights, const in float4x4 BoneFinalTransforms[MAX_BONES], out float4 skinnedPosition, out float3 skinnedNormal, out float3 skinnedTangentNormal, out float3 skinnedTangentBinormal)
{
	skinnedPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
	skinnedNormal = float3(0.0f, 0.0f, 0.0f);
	skinnedTangentNormal = float3(0.0f, 0.0f, 0.0f);
	skinnedTangentBinormal = float3(0.0f, 0.0f, 0.0f);

	for (int index = 0; index < MAX_BONE_INFLUENCE; ++index)
	{
		if (boneIDs[index] < 0 || boneIDs[index] >= MAX_BONES)
		{
			continue;
		}

		float weight = boneWeights[index];

		float4 localPosition = mul(Pos, BoneFinalTransforms[boneIDs[index]]);
		skinnedPosition += localPosition * weight;

		float3x3 mat3 = (float3x3) BoneFinalTransforms[boneIDs[index]];

		skinnedNormal += mul(Normal, mat3) * weight;
		skinnedTangentNormal += mul(Tangent, mat3) * weight;
		skinnedTangentBinormal += mul(Binormal, mat3) * weight;
	}

	skinnedNormal = normalize(skinnedNormal);
	skinnedTangentNormal = normalize(skinnedTangentNormal);
	skinnedTangentBinormal = normalize(skinnedTangentBinormal);
}
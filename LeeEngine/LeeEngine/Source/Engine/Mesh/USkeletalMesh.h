// 03.24
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/MyEngineUtils.h"
#include "Engine/Class/UTexture.h"
#include "Engine/Material/UMaterial.h"
#include "Engine/UObject/UObject.h"

class FSkeletalMeshRenderData
{
public:
	FSkeletalMeshRenderData(const nlohmann::json& SkeletalMeshFilePathData);

	unsigned int MeshCount;

	// Buffer[MeshCount]
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> VertexBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> IndexBuffer;

	std::vector<std::shared_ptr<UMaterialInterface>> MaterialInterfaces;

	// 본이름 - BoneInfo 맵
	std::map<std::string, BoneInfo> ModelBoneInfoMap;

	float BoundSphereRadius = 0;
};

class USkeletalMesh : public UObject
{
	MY_GENERATE_BODY(USkeletalMesh)
	USkeletalMesh();
	~USkeletalMesh() override;

	unsigned int GetSkeletalMeshMeshCount() const
	{
		return RenderData.get()->MeshCount;
	}

	// 로우 포인터로 반환
	FSkeletalMeshRenderData* GetSkeletalMeshRenderData() const
	{
		return RenderData.get();
	}

	static std::shared_ptr<USkeletalMesh> GetSkeletalMesh(const std::string& SkeletalMeshName);

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

private:
	// TODO: LOD 데이터가 필요한 경우 std::map<UINT, std::unique_ptr<FStaticMeshRenderData> LODRenderData; 로 변경 예정
	// StaticMesh의 버텍스, 인덱스 버퍼 등 렌더링에 필요한 데이터를 관리
	std::shared_ptr<FSkeletalMeshRenderData> RenderData;

};

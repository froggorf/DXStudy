// 03.24
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/MyEngineUtils.h"
#include "Engine/Class/UTexture.h"
#include "Engine/UObject/UObject.h"

class FSkeletalMeshRenderData
{
public:
	FSkeletalMeshRenderData(const nlohmann::json& SkeletalMeshFilePathData);



protected:
private:
public:
	unsigned int MeshCount;

	// Buffer[MeshCount]
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> VertexBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> IndexBuffer;

	// TODO: 임시 텍스쳐 데이터,
	// UMaterial 구현 이전으로 텍스쳐 SRV 데이터를 해당 클래스에서 관리
	//std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> TextureSRV;
	std::vector<std::shared_ptr<UTexture>> Textures;

	// 본이름 - BoneInfo 맵
	std::map<std::string, BoneInfo> ModelBoneInfoMap;
};


class USkeletalMesh : public UObject, public std::enable_shared_from_this<USkeletalMesh>
{
	MY_GENERATED_BODY(USkeletalMesh)
public:
	USkeletalMesh();
	~USkeletalMesh();

	unsigned int GetSkeletalMeshMeshCount() const {return RenderData.get()->MeshCount; }

	// unique_ptr 로 관리되는 RenderData
	// Render 시 잠깐 사용하므로 로우 포인터를 반환
	FSkeletalMeshRenderData* GetSkeletalMeshRenderData() const {return RenderData.get();}


	static const std::map<std::string, std::shared_ptr<USkeletalMesh>>& GetStaticMeshCache() {return GetSkeletalMeshCacheMap();}
	static const std::shared_ptr<USkeletalMesh>& GetSkeletalMesh(const std::string& SkeletalMeshName) { return GetSkeletalMeshCacheMap()[SkeletalMeshName]; }

	virtual void LoadDataFromFileData(const nlohmann::json& AssetData) override;
protected:
private:

public:
protected:
private:
	static std::map<std::string, std::shared_ptr<USkeletalMesh>>& GetSkeletalMeshCacheMap()
	{
		static std::map<std::string, std::shared_ptr<USkeletalMesh>> SkeletalMeshCache;
		return SkeletalMeshCache;
	}


	// TODO: LOD 데이터가 필요한 경우 std::map<UINT, std::unique_ptr<FStaticMeshRenderData> LODRenderData; 로 변경 예정
	// StaticMesh의 버텍스, 인덱스 버퍼 등 렌더링에 필요한 데이터를 관리
	std::unique_ptr<FSkeletalMeshRenderData> RenderData;

};

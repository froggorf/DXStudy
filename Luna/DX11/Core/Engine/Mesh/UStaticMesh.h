// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#ifndef USTATICMESH_H
#define USTATICMESH_H

#include "StaticMeshResources.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/UEngine.h"
#include "Engine/UObject/UObject.h"

class UStaticMesh : public UObject, public std::enable_shared_from_this<UStaticMesh>
{
public:
	UStaticMesh();
	~UStaticMesh();

	unsigned int GetStaticMeshMeshCount() const {return RenderData.get()->MeshCount; }

	// unique_ptr 로 관리되는 RenderData
	// Render 시 잠깐 사용하므로 로우 포인터를 반환
	const FStaticMeshRenderData* GetStaticMeshRenderData() const {return RenderData.get();}

	static const std::map<std::string, std::shared_ptr<UStaticMesh>>& GetStaticMeshCache() {return StaticMeshCache;}
	static const std::shared_ptr<UStaticMesh>& GetStaticMesh(const std::string& StaticMeshName) { return StaticMeshCache[StaticMeshName]; }


	virtual void LoadDataFromFileData(std::vector<std::string>& StaticMeshAssetData) override;
protected:
private:

public:
protected:
private:
	static std::map<std::string, std::shared_ptr<UStaticMesh>> StaticMeshCache;


	// TODO: LOD 데이터가 필요한 경우 std::map<UINT, std::unique_ptr<FStaticMeshRenderData> LODRenderData; 로 변경 예정
	// StaticMesh의 버텍스, 인덱스 버퍼 등 렌더링에 필요한 데이터를 관리
	std::unique_ptr<FStaticMeshRenderData> RenderData;
	
};
#endif
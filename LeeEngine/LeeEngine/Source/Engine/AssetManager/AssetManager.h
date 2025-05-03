// assimp 라이브러리와 텍스쳐 로드 라이브러리를 활용하여
// 모델, 텍스쳐 등의 오브젝트 (에셋)을 관리하는 매니저 클래스
#pragma once
#include "DirectX/d3dUtil.h"

#include <type_traits>
#include <fstream>

#include "Engine/MyEngineUtils.h"
#include "Engine/UEditorEngine.h"
#include "Engine/RenderCore/EditorScene.h"
#include "nlohmann/json.hpp"
class UObject;
struct aiScene;
struct aiMesh;


enum class EAssetDataType
{
	EADT_StaticMesh = 0,
	EADT_Level,
	EADT_SkeletalMesh,
};

template <typename T>
concept DerivedFromUObject = std::is_base_of_v<UObject, T>;

class AssetManager
{
public:
	// === 파일 데이터를 읽어 모델 정보를 로드하는 함수 ===
	// 한 메쉬가 여러 오브젝트로 존재시 vector 데이터
	static void LoadModelData(const std::string& path, const Microsoft::WRL::ComPtr<ID3D11Device> pDevice, std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pVertexBuffer,		std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pIndexBuffer);
	// 한 메쉬가 하나의 오브젝트로 존재시
	static void LoadModelData(const std::string& path, Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11Buffer>& pVertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>&  pIndexBuffer);
	// SkeletalMesh 모델 데이터 로드
	static void LoadSkeletalModelData(const std::string& path, const Microsoft::WRL::ComPtr<ID3D11Device> pDevice, std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pVertexBuffer, std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>&pIndexBuffer, std::map<std::string, BoneInfo>& modelBoneInfoMap);

	//static void LoadTextureFromTGA(const std::wstring& szFile, const Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	//std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& vTextureShaderResourceView);
	static void LoadTextureFromFile(const std::wstring& szFile, const Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& vTextureShaderResourceView);
	static void LoadTextureFromFile(const std::wstring& szFile, const Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& vTextureShaderResourceView);
	static void LoadTexture(class UTexture* Texture, const nlohmann::json& AssetData);

	// UINT BindFlag -> D3D11_BIND_FLAG
	static std::shared_ptr<UTexture> CreateTexture(const std::string& Name, UINT Width, UINT Height, DXGI_FORMAT PixelFormat, UINT BindFlag, D3D11_USAGE Usage = D3D11_USAGE_DEFAULT);


	// myasset 파일 읽기
	// myasset을 통해 읽은 UObject 데이터를 unique_ptr를 통해 관리하고 반환
	// UObject에 계승된 클래스만 템플릿의 타입으로 들어갈 수 있도록 컨셉 사용
	//template <DerivedFromUObject T>
	static UObject* ReadMyAsset(const std::string& FilePath);

private:
	// 파일 데이터로 얻은 aiScene 클래스로부터 메쉬 정보 얻는 함수
	static void ProcessScene(const aiScene* scene, std::vector<std::vector<MyVertexData>>& allVertices,
	std::vector<std::vector<UINT>>& allIndices);

	// 메쉬 정보를 분석하여 VertexType에 맞게 정보 수집
	static void ProcessMesh(aiMesh* mesh, std::vector<MyVertexData>& vertices, std::vector<UINT>& indices);

	// SkeletalMesh 모델의 버텍스 데이터 내 본 데이터를 설정하는 함수
	static void SetVertexBoneData(MyVertexData& vertexData, int boneID, float weight);
	// 버텍스의 BoneWeight 정보를 추출하는 함수
	static void ExtractBoneWeightForVertices(std::vector<MyVertexData>& vVertexData, aiMesh* mesh, std::map<std::string, BoneInfo>& modelBoneInfoMap);


public:
	static std::unordered_map<std::string,std::string>& GetAssetNameAndAssetPathCacheMap() {return AssetNameAndAssetPathCacheMap;}
protected:
private:
	static std::unordered_map<std::string, std::shared_ptr<UObject>> AssetCache;
	static std::unordered_map<std::string, std::string> AssetNameAndAssetPathCacheMap;
};




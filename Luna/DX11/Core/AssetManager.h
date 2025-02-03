// assimp 라이브러리와 텍스쳐 로드 라이브러리를 활용하여
// 모델, 텍스쳐 등의 오브젝트 (에셋)을 관리하는 매니저 클래스
#pragma once

#include "d3dUtil.h"

struct aiScene;
struct aiMesh;

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



private:
	// 파일 데이터로 얻은 aiScene 클래스로부터 메쉬 정보 얻는 함수
	static void ProcessScene(const aiScene* scene, std::vector<std::vector<MyVertexData>>& allVertices,
	std::vector<std::vector<UINT>>& allIndices);

	// 메쉬 정보를 분석하여 VertexType에 맞게 정보 수집
	static void ProcessMesh(aiMesh* mesh, std::vector<MyVertexData>& vertices, std::vector<UINT>& indices);

	// SkeletalMesh 모델의 버텍스 데이터 내 본 데이터를 설정하는 함수
	static void SetVertexBoneData(MySkeletalMeshVertexData& vertexData, int boneID, float weight);
	// 버텍스의 BoneWeight 정보를 추출하는 함수
	static void ExtractBoneWeightForVertices(std::vector<MySkeletalMeshVertexData>& vVertexData, aiMesh* mesh, std::map<std::string, BoneInfo>& modelBoneInfoMap);
};


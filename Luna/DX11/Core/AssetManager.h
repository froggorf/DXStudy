// assimp 라이브러리와 텍스쳐 로드 라이브러리를 활용하여
// 모델, 텍스쳐 등의 오브젝트 (에셋)을 관리하는 매니저 클래스
#pragma once


#include <string>
#include <vector>
#include "d3dUtil.h"
#include <assimp/Importer.hpp>

//class aiScene;
class aiMesh;

class AssetManager
{
public:
	// 파일 데이터를 읽어 aiScene 클래스 얻는 함수
	static void LoadModelData(const std::string& path, const Microsoft::WRL::ComPtr<ID3D11Device> pDevice, std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pVertexBuffer,
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>&pIndexBuffer);

	static void LoadTextureFromTGA(const std::wstring& szFile, const Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& vTextureShaderResourceView);

private:
	// 파일 데이터로 얻은 aiScene 클래스로부터 메쉬 정보 얻는 함수
	static void ProcessScene(const aiScene* scene, std::vector<std::vector<MyVertexData>>& allVertices,
	std::vector<std::vector<UINT>>& allIndices);

	// 메쉬 정보를 분석하여 VertexType에 맞게 정보 수집
	static void ProcessMesh(aiMesh* mesh, std::vector<MyVertexData>& vertices, std::vector<UINT>& indices);
};


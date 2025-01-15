#include "AssetManager.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace Microsoft::WRL;

#define ModelFolderDirectory "../../Resource/"
void AssetManager::LoadModelData(const std::string& path, const ComPtr<ID3D11Device> pDevice, std::vector<ComPtr<ID3D11Buffer>>& pVertexBuffer,
	std::vector<ComPtr<ID3D11Buffer>>&pIndexBuffer)
{
    std::string filePath = ModelFolderDirectory + path;

    Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, 
	    aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded );
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
	    std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
	    return;
	}

	std::vector<std::vector<MyVertexData>> allVertices;
	std::vector<std::vector<UINT>> allIndices;

	ProcessScene(scene, allVertices, allIndices);
	
	// 결과 출력
    for (size_t i = 0; i < allVertices.size(); i++) {
        std::cout << "Mesh " << i << ":\n";
        std::cout << "  Vertices: " << allVertices[i].size() << "\n";
        std::cout << "  Indices: " << allIndices[i].size() << "\n";
    }

    // 모델 로드 성공
    std::cout << "Model loaded successfully: " << filePath << std::endl;

	for(int i = 0; i < scene->mNumMeshes; ++i)
	{
		// 버텍스 버퍼
        ComPtr<ID3D11Buffer> vertexBuffer;
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.ByteWidth = sizeof(MyVertexData) * allVertices[i].size();
		D3D11_SUBRESOURCE_DATA initVertexData = {};
		initVertexData.pSysMem = allVertices[i].data();
		HR(pDevice->CreateBuffer(&vertexBufferDesc,&initVertexData, vertexBuffer.GetAddressOf()));
        pVertexBuffer.push_back(vertexBuffer);

		// 인덱스 버퍼
        ComPtr<ID3D11Buffer> indexBuffer;
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.ByteWidth = sizeof(UINT) * allIndices[i].size();
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		D3D11_SUBRESOURCE_DATA initIndexData = {};
		initIndexData.pSysMem = allIndices[i].data();
		HR(pDevice->CreateBuffer(&indexBufferDesc, &initIndexData, indexBuffer.GetAddressOf()));
        pIndexBuffer.push_back(indexBuffer);
		
	}
}


void AssetManager::ProcessScene(const aiScene* scene, std::vector<std::vector<MyVertexData>>& allVertices,
                                std::vector<std::vector<UINT>>& allIndices)
{
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        // 각 메쉬의 버텍스와 인덱스 데이터를 저장할 벡터
        std::vector<MyVertexData> vertices;
        std::vector<UINT> indices;

        // 메쉬 데이터 처리
        ProcessMesh(mesh, vertices, indices);

        // 결과를 전체 리스트에 추가
        allVertices.push_back(vertices);
        allIndices.push_back(indices);
    }
}

void AssetManager::ProcessMesh(aiMesh* mesh, std::vector<MyVertexData>& vertices, std::vector<UINT>& indices)
{
// 1. 버텍스 데이터 추출
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        MyVertexData vertex;
		
        // 정점 위치
        vertex.Pos.x = mesh->mVertices[i].x;
        vertex.Pos.y = mesh->mVertices[i].y;
        vertex.Pos.z = mesh->mVertices[i].z;

        // 법선 벡터 (존재 여부 확인)
        if (mesh->HasNormals()) {
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;
        }

        // 텍스처 좌표 (첫 번째 텍스처 채널만 사용)
        if (mesh->mTextureCoords[0]) { // 텍스처 좌표가 존재하는지 확인
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.TexCoords.x = 0.0f;
            vertex.TexCoords.y = 0.0f;
        }

        vertices.push_back(vertex);
    }

    // 2. 인덱스 데이터 추출
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }	
}

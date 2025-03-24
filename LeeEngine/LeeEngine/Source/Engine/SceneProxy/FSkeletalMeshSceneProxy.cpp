// 03.10
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "FSkeletalMeshSceneProxy.h"

#include "Animation/Animator.h"
#include "Engine/UEditorEngine.h"
#include "Engine/Mesh/USkeletalMesh.h"
#include "Engine/RenderCore/EditorScene.h"

FSkeletalMeshSceneProxy::FSkeletalMeshSceneProxy(UINT PrimitiveID, const std::shared_ptr<USkeletalMesh>& InSkeletalMesh)
	: FPrimitiveSceneProxy(PrimitiveID)
{
	this->SkeletalMesh = InSkeletalMesh;
	std::string Text = "FSkeletalMeshSceneProxy Create StaticMeshSceneProxy - " + std::to_string(PrimitiveID);
	BoneFinalMatrices.resize(MAX_BONES);
	for(int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
	{
		BoneFinalMatrices[BoneIndex] = XMMatrixIdentity(); 
	}
	
	MY_LOG(Text, EDebugLogLevel::DLL_Display, "");
}

FSkeletalMeshSceneProxy::~FSkeletalMeshSceneProxy()
{
}

void FSkeletalMeshSceneProxy::Draw()
{
	if(!SkeletalMesh || !SkeletalMesh->GetSkeletalMeshRenderData())
	{
		return;
	}

	FPrimitiveSceneProxy::Draw();

	// 셰이더 설정
	GDirectXDevice->GetDeviceContext()->IASetInputLayout(GDirectXDevice->GetSkeletalMeshInputLayout().Get());
	GDirectXDevice->GetDeviceContext()->VSSetShader(GDirectXDevice->GetSkeletalMeshVertexShader().Get(), nullptr, 0);


	{
		SkeletalMeshBoneTransformConstantBuffer cb;
		
		//auto boneFinalTransforms = m_PaladinAnimator->GetFinalBoneMatrices();
		for(int i = 0; i < MAX_BONES; ++i)
		{
			cb.BoneFinalTransforms[i] = XMMatrixTranspose(BoneFinalMatrices[i]);
		}
		GDirectXDevice->GetDeviceContext()->UpdateSubresource(GDirectXDevice->GetSkeletalMeshConstantBuffer().Get(),0,nullptr,&cb, 0,0);
		
	}

	const FTest* RenderData = SkeletalMesh->GetSkeletalMeshRenderData();
	unsigned int MeshCount = RenderData->MeshCount;
	for(int MeshIndex= 0; MeshIndex < MeshCount; ++MeshIndex)
	{
		ID3D11DeviceContext* DeviceContext = GDirectXDevice->GetDeviceContext().Get();
		// SRV 설정(텍스쳐)
		{
			int TextureIndex = 0;
			if(RenderData->TextureSRV.size() > MeshIndex)
			{
				TextureIndex = MeshIndex;
			}
			DeviceContext->PSSetShaderResources(0,1, RenderData->TextureSRV[TextureIndex].GetAddressOf());	
		}
		UINT stride = sizeof(MySkeletalMeshVertexData);
		UINT offset = 0;
		DeviceContext->IASetVertexBuffers(0, 1, RenderData->VertexBuffer[MeshIndex].GetAddressOf(), &stride, &offset);
		DeviceContext->IASetIndexBuffer(RenderData->IndexBuffer[MeshIndex].Get(), DXGI_FORMAT_R32_UINT, 0);

		D3D11_BUFFER_DESC indexBufferDesc;
		RenderData->IndexBuffer[MeshIndex]->GetDesc(&indexBufferDesc);
		UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
		DeviceContext->DrawIndexed(indexSize, 0, 0);
	}
	
}


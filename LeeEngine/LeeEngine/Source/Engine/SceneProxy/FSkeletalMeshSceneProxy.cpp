// 03.10
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "FSkeletalMeshSceneProxy.h"

#include "Engine/RenderCore/EditorScene.h"

FSkeletalMeshSceneProxy::FSkeletalMeshSceneProxy(UINT PrimitiveID, UINT InMeshIndex, const std::shared_ptr<USkeletalMesh>& InSkeletalMesh)
	: FPrimitiveSceneProxy(PrimitiveID)
{
	MeshIndex         = InMeshIndex;
	RenderData        = InSkeletalMesh->GetSkeletalMeshRenderData();
	MaterialInterface = RenderData->MaterialInterfaces[MeshIndex];

	if (!MaterialInterface)
	{
		assert(nullptr && "존재하지 않는 머테리얼");
	}

	BoneFinalMatrices.resize(MAX_BONES);
	for (int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
	{
		BoneFinalMatrices[BoneIndex] = XMMatrixIdentity();
	}

	std::string Text = "FSkeletalMeshSceneProxy Create SkeletalMeshSceneProxy - " + std::to_string(PrimitiveID);
	MY_LOG(Text, EDebugLogLevel::DLL_Display, "");
}

FSkeletalMeshSceneProxy::~FSkeletalMeshSceneProxy()
{
}

void FSkeletalMeshSceneProxy::Draw()
{
	if (!RenderData)
	{
		return;
	}

	FPrimitiveSceneProxy::Draw();

	{
		SkeletalMeshBoneTransformConstantBuffer cb;
		for (int i = 0; i < MAX_BONES; ++i)
		{
			cb.BoneFinalTransforms[i] = XMMatrixTranspose(BoneFinalMatrices[i]);
		}

		GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_SkeletalData, &cb, sizeof(cb));
	}

	unsigned int MeshCount = RenderData->MeshCount;

	ID3D11DeviceContext* DeviceContext = GDirectXDevice->GetDeviceContext().Get();

	UINT stride = sizeof(MyVertexData);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, RenderData->VertexBuffer[MeshIndex].GetAddressOf(), &stride, &offset);
	DeviceContext->IASetIndexBuffer(RenderData->IndexBuffer[MeshIndex].Get(), DXGI_FORMAT_R32_UINT, 0);

	D3D11_BUFFER_DESC indexBufferDesc;
	RenderData->IndexBuffer[MeshIndex]->GetDesc(&indexBufferDesc);
	UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
	DeviceContext->DrawIndexed(indexSize, 0, 0);
}

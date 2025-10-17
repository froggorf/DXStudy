#include "CoreMinimal.h"
#include "IDodgeInterface.h"

#include "Engine/AssetManager/AssetManager.h"
#include "Engine/Class/Framework/APlayerController.h"
#include "Engine/RenderCore/RenderingThread.h"
#include "Engine/World/UWorld.h"
#include "MyGame/Core/AMyGamePlayerController.h"

void IDodgeInterface::LoadAnimMontages()
{
	// Dodge
	{
		AssetManager::GetAsyncAssetCache(DodgeMontageName[static_cast<int>(EDodgeDirection::Forward)], [this](std::shared_ptr<UObject> Object)
			{
				AM_Dodge[static_cast<int>(EDodgeDirection::Forward)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
			});
		AssetManager::GetAsyncAssetCache(DodgeMontageName[static_cast<int>(EDodgeDirection::Backward)], [this](std::shared_ptr<UObject> Object)
			{
				AM_Dodge[static_cast<int>(EDodgeDirection::Backward)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
			});
	}

	// Roll
	{
		AssetManager::GetAsyncAssetCache(RollMontageName[static_cast<int>(EDodgeDirection::Forward)], [this](std::shared_ptr<UObject> Object)
			{
				AM_Roll[static_cast<int>(EDodgeDirection::Forward)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
			});
		AssetManager::GetAsyncAssetCache(RollMontageName[static_cast<int>(EDodgeDirection::Backward)], [this](std::shared_ptr<UObject> Object)
			{
				AM_Roll[static_cast<int>(EDodgeDirection::Backward)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
			});
	}


}

void IDodgeInterface::DodgeEnd()
{
	bIsDodging = false;
}

void IDodgeInterface::RollEnd()
{
	RemoveMonochromePostprocess();
}

void IDodgeInterface::AddMonochromePostprocess()
{
#ifdef WITH_EDITOR
	FPostProcessRenderData MonoPP = FPostProcessRenderData{0, "Monochrome",
		UMaterial::GetMaterialCache("M_Monochrome"),
		EMultiRenderTargetType::Editor_HDR};
#else
	FPostProcessRenderData MonoPP = FPostProcessRenderData{0, "Monochrome",
		UMaterial::GetMaterialCache("M_Monochrome"),
		EMultiRenderTargetType::SwapChain_HDR};
#endif
	MonoPP.SetClearDepthStencilTexture(false);
	MonoPP.SetClearRenderTexture(false);
	MonoPP.SetFuncBeforeRendering({[]()
		{
			// 거리 비례를 바인딩 해주기 위해서 해당 함수 적용
			const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& DeviceContext = GDirectXDevice->GetDeviceContext();
			const std::shared_ptr<UTexture>& ViewPositionTex = GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Deferred)->GetRenderTargetTexture(2);
			DeviceContext->PSSetShaderResources(11, 1, ViewPositionTex->GetSRV().GetAddressOf());
			
			APlayerController* PC = GEngine->GetCurrentWorld()->GetPlayerController();
			AMyGamePlayerController* MyGamePC = dynamic_cast<AMyGamePlayerController*>(PC);
			if (!MyGamePC)
			{
				return;
			}
			
			XMFLOAT3 Pos = MyGamePC->GetMonochromeCenterPos();
			XMMATRIX ViewMat = FRenderCommandExecutor::CurrentSceneData->GetViewMatrix();

			XMVECTOR WorldPos4 = XMVectorSet(Pos.x, Pos.y, Pos.z, 1.0f);
			XMVECTOR ViewPosVec = XMVector4Transform(WorldPos4, ViewMat);
			XMFLOAT4 ViewPos;
			XMStoreFloat4(&ViewPos, ViewPosVec);

			FMonochromeDataConstantBuffer Data;
			Data.Distance = MyGamePC->GetMonochromeDistance();
			Data.CenterPos = XMFLOAT3{ViewPos.x, ViewPos.y, ViewPos.z};


			GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_BloomBlur, &Data, sizeof(Data));
		}});
	FScene::AddPostProcess_GameThread(MonoPP);


#ifdef WITH_EDITOR
	FPostProcessRenderData NonMonoPP = FPostProcessRenderData{1, "NotMonochrome",
		UMaterial::GetMaterialCache("M_NotMonochrome"),
		EMultiRenderTargetType::Editor_HDR};
	NonMonoPP.SetClearRenderTexture(false);
	NonMonoPP.SetClearDepthStencilTexture(false);
	FScene::AddPostProcess_GameThread(NonMonoPP);
#else
	FPostProcessRenderData NonMonoPP = FPostProcessRenderData{1, "NotMonochrome",
		UMaterial::GetMaterialCache("M_NotMonochrome"),
		EMultiRenderTargetType::SwapChain_HDR};
	NonMonoPP.SetClearRenderTexture(false);
	NonMonoPP.SetClearDepthStencilTexture(false);
	FScene::AddPostProcess_GameThread(NonMonoPP);
#endif

}

void IDodgeInterface::RemoveMonochromePostprocess()
{
	FScene::RemovePostProcess_GameThread(0, "Monochrome");
	FScene::RemovePostProcess_GameThread(1, "NotMonochrome");
}


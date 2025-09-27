#include "CoreMinimal.h"
#include "PBRTestComponent.h"

UPBRTestComponent* UPBRTestComponent::Instance = nullptr;

UPBRTestComponent::UPBRTestComponent()
{
	Rename("Monochrome Test Comp");
}

void UPBRTestComponent::TurnOnMono()
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

			APlayerController* PC = GEngine->GetWorld()->GetPlayerController();


			XMFLOAT3 Pos = PC->GetMonochromeCenterPos();
			XMMATRIX ViewMat = FRenderCommandExecutor::CurrentSceneData->GetViewMatrix();

			XMVECTOR WorldPos4 = XMVectorSet(Pos.x, Pos.y, Pos.z, 1.0f);
			XMVECTOR ViewPosVec = XMVector4Transform(WorldPos4, ViewMat);
			XMFLOAT4 ViewPos;
			XMStoreFloat4(&ViewPos, ViewPosVec);

			FMonochromeDataConstantBuffer Data;
			Data.Distance = PC->GetMonochromeDistance();
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


void UPBRTestComponent::TurnOffMono()
{
	FScene::RemovePostProcess_GameThread(0, "Monochrome");
	FScene::RemovePostProcess_GameThread(1, "NotMonochrome");
}

void UPBRTestComponent::ObjectSelect(int index)
{
	for (int i = 0; i < 3; ++i)
	{
		FScene::SetComponentMonochrome_GameThread(TargetComp[i]->GetPrimitiveID(), i!=index);
	}
	GEngine->GetWorld()->GetPlayerController()->SetMonochromeCenterComp(TargetComp[index]);
}

#ifdef WITH_EDITOR
void UPBRTestComponent::DrawDetailPanel(UINT ComponentDepth)
{
	USceneComponent::DrawDetailPanel(ComponentDepth);
	static bool Mono = false;
	if (ImGui::Checkbox("Monochrome", &Mono))
	{
		if (Mono)
		{
			TurnOnMono();
		}
		else
		{
			TurnOffMono();
		}
	}

	static float Distance = 0.0f;
	if (ImGui::SliderFloat("Monochrome Distance", &Distance, 0.0f, 2000, "%.2f"))
	{
		GEngine->GetWorld()->GetPlayerController()->SetMonochromeDistance(Distance);
	}

	ImGui::Text("TargetObject");
	if (ImGui::Button("Chair"))
	{
		ObjectSelect(0);
	}
	if (ImGui::Button("Couch"))
	{
		ObjectSelect(1);
	}
	if (ImGui::Button("Character"))
	{
		ObjectSelect(2);
	}
}
#endif
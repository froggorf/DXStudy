#include "CoreMinimal.h"
#include "ASkyBox.h"
#include "Engine/World/UWorld.h"

/*
 Note: SkyBox 이미지 구하는법
 https://mstone8370.tistory.com/28
 텍스쳐 -> 스카이박스 -> .dds
 */

ASkyBox::ASkyBox()
{
	SM_SkyBox = std::make_shared<UStaticMeshComponent>();
	SM_SkyBox->SetupAttachment(GetRootComponent());
	std::shared_ptr<UMaterialInterface> SkyBoxMaterial = UMaterial::GetMaterialCache("M_SkyBox");
	if (SkyBoxMaterial)
	{
		SkyBoxMaterial->SetRasterizerType(ERasterizerType::RT_CullFront);
	}
	AssetManager::GetAsyncAssetCache("SM_SkyBox", [this](std::shared_ptr<UObject> LoadedStaticMesh)
	{
		SM_SkyBox->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(LoadedStaticMesh));
	});
	SM_SkyBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SM_SkyBox->SetDoFrustumCulling(false);
	SM_SkyBox->SetWorldLocation({-100000,-100000,-100000});

	FScene::SetSkyBoxTexture_GameThread("T_SkyBox");

	DirectionalLight= std::make_shared<ULightComponent>();
	DirectionalLight->SetupAttachment(GetRootComponent());
	DirectionalLight->SetWorldLocation(XMFLOAT3{-3000, 6000,-3000});
	DirectionalLight->SetLightType(ELightType::Directional);
	XMVECTOR RotQuat = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(45.0f),
		XMConvertToRadians(45.0f),
		XMConvertToRadians(0.0f)
	);
	DirectionalLight->SetWorldRotation(RotQuat);
	constexpr float Radiance = 2.0f;
	DirectionalLight->SetLightColor({Radiance,Radiance,Radiance});


	if (!GDirectXDevice) return;

	
				
		


	
}

void ASkyBox::Register()
{
	AActor::Register();

//	// Emissive Down/Up Sampling
//	{
//		// DownSampling
//		{
//			const std::shared_ptr<UMaterialInterface>& DownSamplingMat = UMaterial::GetMaterialCache("M_BlurDownSam");
//			for (UINT i = 0; i < BloomCount; ++i)
//			{
//				EMultiRenderTargetType OutRenderType = static_cast<EMultiRenderTargetType>(static_cast<UINT>(EMultiRenderTargetType::Bloom_Blur_0) + i);
//				FPostProcessRenderData DownSampling = FPostProcessRenderData{static_cast<UINT>(i), "EBlur_Down", DownSamplingMat, OutRenderType};
//				if (i == 0)
//				{
//					// 0번째에게는 EmissiveTargetTex를 텍스쳐로
//					DownSampling.SetSRVNames({"EmissiveTargetTex"});
//					DownSampling.SetFuncBeforeRendering({
//						[]()
//						{
//							const std::shared_ptr<FMultiRenderTarget>& DeferredMRT = GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Deferred);
//							if (!DeferredMRT) return;
//
//							const std::shared_ptr<UTexture>& RT = DeferredMRT->GetRenderTargetTexture(0);
//							const D3D11_TEXTURE2D_DESC& Desc = RT->GetDesc();
//							FBloomDataConstantBuffer BindingData;
//							BindingData.TexelSize = XMFLOAT2{static_cast<float>(Desc.Width), static_cast<float>(Desc.Height)};
//							GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_BloomBlur, &BindingData, sizeof(BindingData));
//						}
//						});
//				}
//				else
//				{
//					// 이 외에는 이전 결과물의 텍스쳐를
//					DownSampling.SetSRVNames({ "Blur_RT" + std::to_string(i-1) });
//					DownSampling.SetFuncBeforeRendering({
//						[i]()
//						{
//							//해당 MRT가 존재하면 이전 MRT 는 반드시 존재
//							const std::shared_ptr<FMultiRenderTarget>& LastDownSamplingMRT = GDirectXDevice->GetBloomMRT(i-1);
//							const std::shared_ptr<UTexture>& RT = LastDownSamplingMRT->GetRenderTargetTexture(0);
//							const D3D11_TEXTURE2D_DESC& Desc =RT->GetDesc();
//							FBloomDataConstantBuffer BindingData;
//							BindingData.TexelSize = XMFLOAT2{static_cast<float>(Desc.Width), static_cast<float>(Desc.Height)};
//							GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_BloomBlur, &BindingData, sizeof(BindingData));
//						}
//						});
//
//				}
//				FScene::AddPostProcess_GameThread(DownSampling);
//			}
//		}
//
//		// UpSampling
//		{
//			const std::shared_ptr<UMaterialInterface>& UpSamplingMat = UMaterial::GetMaterialCache("M_BlurUpSam");
//			for (int i = static_cast<int>(BloomCount) - 2; i >= 0; --i)
//			{
//				EMultiRenderTargetType OutRenderType = static_cast<EMultiRenderTargetType>(static_cast<UINT>(EMultiRenderTargetType::Bloom_Blur_0) + i);
//				UINT Priority = static_cast<UINT>(BloomCount + (BloomCount - 2 - i) + 1);
//				FPostProcessRenderData UpSamplingPP = FPostProcessRenderData{Priority, "EBlur_Up", UpSamplingMat, OutRenderType};
//				// 이 외에는 이전 결과물의 텍스쳐를
//
//				// i+1 번째 다운샘플링 결과를 바인딩 해주어야함
//				// 추가로 내 PostProcess 는 모든 렌더타겟을 지우고 하기떄문에
//				// ONE_ONE 블렌드 방식이 아니라 이전 텍스쳐 결과를 따로 바인딩해준다음에 적용해줘야함
//				UpSamplingPP.SetSRVNames({ "Blur_RT" + std::to_string(i+1) });
//				UpSamplingPP.SetFuncBeforeRendering({
//					[i]()
//					{
//						// Additive하기 위한 다운샘플링 텍스쳐를 CopyResource 하고서 바인딩해줘야함
//						// 이 기능은 별도로 해당 함수에서 적용해줘야함
//						const std::shared_ptr<FMultiRenderTarget>& BloomMRT = GDirectXDevice->GetBloomMRT(i);
//						std::shared_ptr<UTexture> LastRT = BloomMRT->GetRenderTargetTexture(0);
//						std::shared_ptr<UTexture> T_BloomPP = GDirectXDevice->GetBloomPPTexture(i);
//
//						GDirectXDevice->GetDeviceContext()->CopyResource(T_BloomPP->GetTexture().Get(), LastRT->GetTexture().Get());
//						GDirectXDevice->GetDeviceContext()->PSSetShaderResources(2,1, T_BloomPP->GetSRV().GetAddressOf());
//
//						const D3D11_TEXTURE2D_DESC& Desc = LastRT->GetDesc();
//						FBloomDataConstantBuffer BindingData;
//						BindingData.TexelSize = XMFLOAT2{static_cast<float>(Desc.Width), static_cast<float>(Desc.Height)};
//						GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_BloomBlur, &BindingData, sizeof(BindingData));
//					}
//					});
//				FScene::AddPostProcess_GameThread(UpSamplingPP);
//			}
//		}
//	}
//
//
//	// Bloom
//	{
//#ifdef WITH_EDITOR
//		FPostProcessRenderData BloomPP = 
//			FPostProcessRenderData{20, "Bloom",
//			UMaterial::GetMaterialCache("M_Bloom"),
//			EMultiRenderTargetType::Editor_HDR};
//#else
//		FPostProcessRenderData BloomPP = FPostProcessRenderData{1, "Bloom", UMaterial::GetMaterialCache("M_Bloom"), EMultiRenderTargetType::SwapChain_HDR};
//#endif
//		BloomPP.SetSRVNames({"Blur_RT0"});
//		BloomPP.SetFuncBeforeRendering({
//			[]()
//			{
//				FBloomDataConstantBuffer BindingData;
//				BindingData.BloomIntensity = 10.f;
//				GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_BloomBlur, &BindingData, sizeof(BindingData));
//			}
//			});
//		FScene::AddPostProcess_GameThread(BloomPP);
//	}



#ifdef WITH_EDITOR
	FScene::AddPostProcess_GameThread(FPostProcessRenderData{100, "ToneMap", UMaterial::GetMaterialCache("M_PostProcessTest"), EMultiRenderTargetType::Editor_Main});
#else
	FScene::AddPostProcess_GameThread(FPostProcessRenderData{100, "ToneMap", UMaterial::GetMaterialCache("M_PostProcessTest"), EMultiRenderTargetType::SwapChain_Main});
#endif
}


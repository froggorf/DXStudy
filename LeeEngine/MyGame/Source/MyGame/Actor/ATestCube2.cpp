#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Components/UStaticMeshComponent.h"


ATestCube2::ATestCube2()
{
	if (!GDirectXDevice) return;

	PBRTestComp = std::make_shared<UPBRTestComponent>();
	PBRTestComp->SetupAttachment(GetRootComponent());
	{
		SM_Chair = std::make_shared<UStaticMeshComponent>();
		SM_Chair->SetupAttachment(GetRootComponent());
		AssetManager::GetAsyncAssetCache("SM_UE_Chair",[this](std::shared_ptr<UObject> Object)
			{
				SM_Chair->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			});
		SM_Chair->SetRelativeLocation({-300,500,0});
	}

	{
		SM_Couch = std::make_shared<UStaticMeshComponent>();
		SM_Couch->SetupAttachment(GetRootComponent());
		AssetManager::GetAsyncAssetCache("SM_UE_Couch",[this](std::shared_ptr<UObject> Object)
			{
				SM_Couch->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			});
		SM_Couch->SetRelativeLocation({-300,500,-250});
	}

	{
		Ground= std::make_shared<UStaticMeshComponent>();
		Ground->SetupAttachment(GetRootComponent());
		AssetManager::GetAsyncAssetCache("SM_Brick",[this](std::shared_ptr<UObject> Object)
			{
				Ground->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			});
		Ground->SetRelativeLocation({0,490,0});
		Ground->SetRelativeScale3D({500,10,500});
	}

	SM_Well = std::make_shared<UStaticMeshComponent>();
	SM_Well->SetupAttachment(GetRootComponent());
	AssetManager::GetAsyncAssetCache("SM_Well_PBR",[this](std::shared_ptr<UObject> Object)
		{
			SM_Well->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});
	SM_Well->SetRelativeLocation({0,500,0});

	SM_Barrel1 = std::make_shared<UStaticMeshComponent>();
	SM_Barrel1->SetupAttachment(GetRootComponent());
	AssetManager::GetAsyncAssetCache("SM_Barrel1",[this](std::shared_ptr<UObject> Object)
		{
			SM_Barrel1->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});
	SM_Barrel1->SetRelativeLocation({300,500,0});

	SM_Barrel2 = std::make_shared<UStaticMeshComponent>();
	SM_Barrel2->SetupAttachment(GetRootComponent());
	AssetManager::GetAsyncAssetCache("SM_Barrel2",[this](std::shared_ptr<UObject> Object)
		{
			SM_Barrel2->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});
	SM_Barrel2->SetRelativeLocation({300,500,300});
	SM_Barrel2->SetRelativeRotation(XMFLOAT3{0,180,0});

	
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0;  j < 6; ++j)
		{
			PBRTestSphere[i][j] = std::make_shared<UStaticMeshComponent>();
			PBRTestSphere[i][j]->SetupAttachment(GetRootComponent());
			PBRTestSphere[i][j]->SetRelativeLocation({-500.0f + 200*i, 100.0f, 1500.0f + 200 * j});
			AssetManager::GetAsyncAssetCache("SM_PBRTestSphere",[this, i,j](std::shared_ptr<UObject> Object)
				{
					PBRTestSphere[i][j]->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
					
				});
			constexpr float Size = 30.0f;
			PBRTestSphere[i][j]->SetRelativeScale3D({Size,Size,Size});
			
		}
	}


	
}

void ATestCube2::Register()
{
	// Emissive Down/Up Sampling
	{

		const std::shared_ptr<UMaterialInterface>& DownSamplingMat = UMaterial::GetMaterialCache("M_BlurDownSam");
		for (UINT i = 0; i < BloomCount; ++i)
		{
			EMultiRenderTargetType OutRenderType = static_cast<EMultiRenderTargetType>(static_cast<UINT>(EMultiRenderTargetType::Bloom_Blur_0) + i);
			FPostProcessRenderData DownSampling = FPostProcessRenderData{static_cast<UINT>(i), "EBlur_Down", DownSamplingMat, OutRenderType};
			if (i == 0)
			{
				// 0번째에게는 EmissiveTargetTex를 텍스쳐로
				DownSampling.SetSRVNames({"EmissiveTargetTex"});
				DownSampling.SetFuncBeforeRendering({
					[]()
					{
						const std::shared_ptr<FMultiRenderTarget>& DeferredMRT = GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Deferred);
						if (!DeferredMRT) return;

						const std::shared_ptr<UTexture>& RT = DeferredMRT->GetRenderTargetTexture(0);
						const D3D11_TEXTURE2D_DESC& Desc = RT->GetDesc();
						FBloomDataConstantBuffer BindingData;
						BindingData.TexelSize = XMFLOAT2{static_cast<float>(Desc.Width), static_cast<float>(Desc.Height)};
						GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_BloomBlur, &BindingData, sizeof(BindingData));
					}
				});
			}
			else
			{
				// 이 외에는 이전 결과물의 텍스쳐를
				DownSampling.SetSRVNames({ "Blur_RT" + std::to_string(i-1) });
				DownSampling.SetFuncBeforeRendering({
					[i]()
					{
						// 현재 MRT가 존재하지 않다면 그냥 리턴
						if (nullptr == GDirectXDevice->GetBloomMRT(i)) return;

						//해당 MRT가 존재하면 이전 MRT 는 반드시 존재
						const std::shared_ptr<FMultiRenderTarget>& LastDownSamplingMRT = GDirectXDevice->GetBloomMRT(i-1);
						const std::shared_ptr<UTexture>& RT = LastDownSamplingMRT->GetRenderTargetTexture(0);
						const D3D11_TEXTURE2D_DESC& Desc =RT->GetDesc();
						FBloomDataConstantBuffer BindingData;
						BindingData.TexelSize = XMFLOAT2{static_cast<float>(Desc.Width), static_cast<float>(Desc.Height)};
						GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_BloomBlur, &BindingData, sizeof(BindingData));
					}
					});
				
			}
			FScene::AddPostProcess_GameThread(DownSampling);
		}
	}
	

	// Bloom
//	{
//#ifdef WITH_EDITOR
//		FPostProcessRenderData BloomPP = FPostProcessRenderData{6, "Bloom", UMaterial::GetMaterialCache("M_Bloom"), EMultiRenderTargetType::Editor_Main};
//#else
//		FPostProcessRenderData BloomPP = FPostProcessRenderData{1, "Bloom", UMaterial::GetMaterialCache("M_Bloom"), EMultiRenderTargetType::SwapChain_HDR};
//#endif
//		BloomPP.SetSRVNames({"EBlur_Hor"});
//		FScene::AddPostProcess_GameThread(BloomPP);
//	}


#ifdef WITH_EDITOR
	//FScene::AddPostProcess_GameThread(FPostProcessRenderData{100, "ToneMap", UMaterial::GetMaterialCache("M_PostProcessTest"), EMultiRenderTargetType::Editor_Main});
#else
	FScene::AddPostProcess_GameThread(FPostProcessRenderData{100, "ToneMap", UMaterial::GetMaterialCache("M_PostProcessTest"), EMultiRenderTargetType::SwapChain_Main});
#endif
	AActor::Register();

	PBRTestComp->OnRChange.Add([this](float Value)
	{
			for (int i = 0; i < 6; ++i)
			{
				for (int j = 0; j < 6; ++j)
				{
					PBRTestSphere[i][j]->SetScalarParam(0,"Albedo_R", Value);
				}
			}	
	});
	PBRTestComp->OnBChange.Add([this](float Value)
		{
			for (int i = 0; i < 6; ++i)
			{
				for (int j = 0; j < 6; ++j)
				{
					PBRTestSphere[i][j]->SetScalarParam(0,"Albedo_B", Value);
				}
			}	
		});
	PBRTestComp->OnGChange.Add([this](float Value)
		{
			for (int i = 0; i < 6; ++i)
			{
				for (int j = 0; j < 6; ++j)
				{
					PBRTestSphere[i][j]->SetScalarParam(0,"Albedo_G", Value);
				}
			}	
		});
	PBRTestComp->OnButtonDelegate.Add([this]()
	{
			for (int i = 0; i < 6; ++i)
			{
				for (int j = 0; j < 6; ++j)
				{
					float NewMetallic = i * 0.2f;
					PBRTestSphere[i][j]->SetScalarParam(0,"Metallic", NewMetallic);
					float NewRoughness = j * 0.2f;
					PBRTestSphere[i][j]->SetScalarParam(0,"Roughness", NewRoughness);
				}
			}	
	});
	
}

void ATestCube2::BeginPlay()
{
	AActor::BeginPlay();
	

	
	
}

void ATestCube2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}

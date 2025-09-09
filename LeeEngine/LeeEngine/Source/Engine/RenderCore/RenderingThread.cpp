// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"

#include "EditorScene.h"
#include "renderingthread.h"
#include "Engine/Physics/UShapeComponent.h"
#include "Engine/SceneProxy/FNiagaraSceneProxy.h"
#include "Engine/SceneProxy/FSkeletalMeshSceneProxy.h"
#include "Engine/SceneProxy/FStaticMeshSceneProxy.h"



std::shared_ptr<FScene> FRenderCommandExecutor::CurrentSceneData = nullptr;

FScene::FScene()
{
	DeferredMergeRenderData.MaterialInterface = UMaterial::GetMaterialCache("M_DeferredMergeRect");
	DeferredMergeRenderData.MaterialInterface->SetRasterizerType(ERasterizerType::RT_TwoSided);
	DeferredMergeRenderData.MaterialInterface->SetDepthStencilState(EDepthStencilStateType::DST_NO_TEST_NO_WRITE);
	DeferredMergeRenderData.MaterialInterface->SetBlendStateType(EBlendStateType::BST_Default);
	std::static_pointer_cast<UMaterial>(DeferredMergeRenderData.MaterialInterface)->SetTexture(0, UTexture::GetTextureCache("DiffuseTargetTex")); 
	
	std::shared_ptr<UStaticMesh> StaticMesh = UStaticMesh::GetStaticMesh("SM_DeferredMergeRect");
	if (!StaticMesh)
	{
		AssetManager::ReadMyAsset(AssetManager::GetAssetNameAndAssetPathMap()["SM_DeferredMergeRect"]);	
		StaticMesh = UStaticMesh::GetStaticMesh("SM_DeferredMergeRect");
	}
	DeferredMergeRenderData.SceneProxy = std::make_shared<FStaticMeshSceneProxy>(-1,0,StaticMesh);

	LightBuffer = std::make_shared<FStructuredBuffer>();

	M_LightShadow[static_cast<UINT>(ELightType::Directional)][0] = UMaterial::GetMaterialCache("M_DirShadow_Static");
	M_LightShadow[static_cast<UINT>(ELightType::Directional)][1] = UMaterial::GetMaterialCache("M_DirShadow_Skeletal");
	M_LightShadow[static_cast<UINT>(ELightType::Point)][0] = UMaterial::GetMaterialCache("M_PointShadow_Static");
	M_LightShadow[static_cast<UINT>(ELightType::Point)][1] = UMaterial::GetMaterialCache("M_PointShadow_Skeletal");

	M_Widget = UMaterial::GetMaterialCache("M_Widget");
	M_Widget->SetRasterizerType(ERasterizerType::RT_TwoSided);
	M_Widget->SetDepthStencilState(EDepthStencilStateType::DST_NO_TEST_NO_WRITE);
	// 동일한 메쉬를 사용하므로 재사용
	WidgetStaticMeshSceneProxy = DeferredMergeRenderData.SceneProxy;
}

void FScene::BeginRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData, UINT GameThreadFrameCount)
{
	RenderingThreadFrameCount = GameThreadFrameCount;
	SceneData->BeginRenderFrame();
}
static void KillProxies(std::unordered_map<UINT, std::vector<FPrimitiveRenderData>>& SceneProxyRenderData, const std::vector<UINT>& PendingKillPrimitiveIDs)
{
	// Opaque {머테리얼 ID, 씬프록시 벡터}
	for (auto MaterialIter = SceneProxyRenderData.begin(); MaterialIter != SceneProxyRenderData.end(); ++MaterialIter)
	{
		// 씬 프록시 벡터
		std::vector<FPrimitiveRenderData>& Proxies = MaterialIter->second;

		for (UINT KillPrimitiveID : PendingKillPrimitiveIDs)
		{
			Proxies.erase(std::remove_if(Proxies.begin(),Proxies.end(), [KillPrimitiveID](const FPrimitiveRenderData& RenderData)
				{
					return RenderData.PrimitiveID == KillPrimitiveID;
				}), Proxies.end());
		}
	}	
}

void FScene::BeginRenderFrame()
{
	if (bIsFrameStart)
	{
		return;
	}

	bIsFrameStart = true;

	// Pending Kill
	if(!PendingKillPrimitiveIDs.empty())
	{	
		KillProxies(OpaqueSceneProxyRenderData, PendingKillPrimitiveIDs);
		KillProxies(MaskedSceneProxyRenderData, PendingKillPrimitiveIDs);
		KillProxies(TranslucentSceneProxyRenderData, PendingKillPrimitiveIDs);
		KillProxies(DeferredSceneProxyRenderData, PendingKillPrimitiveIDs);
		PendingKillPrimitiveIDs.clear();
	}
	
	

	// Pending Add
	for (const auto& NewPrimitiveProxies : PendingAddSceneProxies)
	{
		for (int i = 0; i < NewPrimitiveProxies.second.size(); ++i)
		{
			const auto& NewPrimitiveProxy = NewPrimitiveProxies;

			FPrimitiveRenderData RenderData;
			RenderData.MeshIndex         = NewPrimitiveProxy.second[i]->GetMeshIndex();
			RenderData.PrimitiveID       = NewPrimitiveProxy.first;
			RenderData.SceneProxy        = NewPrimitiveProxy.second[i];
			RenderData.MaterialInterface = NewPrimitiveProxy.second[i]->GetMaterialInterface();

			UINT MaterialID = RenderData.SceneProxy->GetMaterialID();

			bool bUseMaterialInstance = RenderData.SceneProxy->GetMaterialInterface()->IsMaterialInstance();
			// 만약 머테리얼 인스턴스를 쓸 경우 새로 인스턴싱 해줘야함
			if (bUseMaterialInstance)
			{
				RenderData.MaterialInterface = std::dynamic_pointer_cast<UMaterialInstance>(NewPrimitiveProxy.second[i]->GetMaterialInterface())->GetInstance();
			}

			// 머테리얼 ID 로 관리하는데, 머테리얼 -> 머테리얼인스턴스 순으로 배열에 배치되도록 설정
			switch (RenderData.MaterialInterface->GetBlendModeType())
			{
			case EBlendMode::BM_Opaque:
				if (bUseMaterialInstance)
				{
					OpaqueSceneProxyRenderData[MaterialID].emplace_back(RenderData);
				}
				else
				{
					OpaqueSceneProxyRenderData[MaterialID].insert(OpaqueSceneProxyRenderData[MaterialID].begin(), RenderData);
				}
				break;
			case EBlendMode::BM_Masked:
				if (bUseMaterialInstance)
				{
					MaskedSceneProxyRenderData[MaterialID].emplace_back(RenderData);
				}
				else
				{
					MaskedSceneProxyRenderData[MaterialID].insert(MaskedSceneProxyRenderData[MaterialID].begin(), RenderData);
				}
				break;
			case EBlendMode::BM_Translucent:
				if (bUseMaterialInstance)
				{
					TranslucentSceneProxyRenderData[MaterialID].emplace_back(RenderData);
				}
				else
				{
					TranslucentSceneProxyRenderData[MaterialID].insert(TranslucentSceneProxyRenderData[MaterialID].begin(), RenderData);
				}
				break;
			case EBlendMode::BM_Deferred:
				if (bUseMaterialInstance)
				{
					DeferredSceneProxyRenderData[MaterialID].emplace_back(RenderData);
				}
				else
				{
					DeferredSceneProxyRenderData[MaterialID].insert(DeferredSceneProxyRenderData[MaterialID].begin(), RenderData);
				}
			break;
			default:
				// 잘못된 데이터
				assert(0);
				break;
			}
		}
	}
	PendingAddSceneProxies.clear();

	// 새로운 위치정보 갱신
	for (const auto& NewTransform : PendingNewTransformProxies)
	{
		UINT FindPrimitiveID = NewTransform.first;

		// Opaque
		for (auto Iter = OpaqueSceneProxyRenderData.begin(); Iter != OpaqueSceneProxyRenderData.end(); ++Iter)
		{
			for (auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end();)
			{
				PrimitiveIter = std::find_if(PrimitiveIter,
											Iter->second.end(),
											[FindPrimitiveID](const FPrimitiveRenderData& A)
											{
												return A.PrimitiveID == FindPrimitiveID;
											});
				if (PrimitiveIter != Iter->second.end())
				{
					PrimitiveIter->SceneProxy->SetSceneProxyWorldTransform(NewTransform.second);
					++PrimitiveIter;
				}
			}
		}

		// Masked
		for (auto Iter = MaskedSceneProxyRenderData.begin(); Iter != MaskedSceneProxyRenderData.end(); ++Iter)
		{
			for (auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end();)
			{
				PrimitiveIter = std::find_if(PrimitiveIter,
											Iter->second.end(),
											[FindPrimitiveID](const FPrimitiveRenderData& A)
											{
												return A.PrimitiveID == FindPrimitiveID;
											});
				if (PrimitiveIter != Iter->second.end())
				{
					PrimitiveIter->SceneProxy->SetSceneProxyWorldTransform(NewTransform.second);
					++PrimitiveIter;
				}
			}
		}

		// Translucent
		for (auto Iter = TranslucentSceneProxyRenderData.begin(); Iter != TranslucentSceneProxyRenderData.end(); ++Iter)
		{
			for (auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end();)
			{
				PrimitiveIter = std::find_if(PrimitiveIter,
											Iter->second.end(),
											[FindPrimitiveID](const FPrimitiveRenderData& A)
											{
												return A.PrimitiveID == FindPrimitiveID;
											});
				if (PrimitiveIter != Iter->second.end())
				{
					PrimitiveIter->SceneProxy->SetSceneProxyWorldTransform(NewTransform.second);
					++PrimitiveIter;
				}
			}
		}

		// Deferred
		for (auto Iter = DeferredSceneProxyRenderData.begin(); Iter != DeferredSceneProxyRenderData.end(); ++Iter)
		{
			for (auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end();)
			{
				PrimitiveIter = std::find_if(PrimitiveIter,
					Iter->second.end(),
					[FindPrimitiveID](const FPrimitiveRenderData& A)
					{
						return A.PrimitiveID == FindPrimitiveID;
					});
				if (PrimitiveIter != Iter->second.end())
				{
					PrimitiveIter->SceneProxy->SetSceneProxyWorldTransform(NewTransform.second);
					++PrimitiveIter;
				}
			}
		}
	}
	PendingNewTransformProxies.clear();
}


void FScene::UpdateSkeletalMeshAnimation_GameThread(UINT PrimitiveID, const std::vector<XMMATRIX>& FinalMatrices)
{
	if (PrimitiveID > 0)
	{
		auto Lambda = [PrimitiveID, FinalMatrices](std::shared_ptr<FScene>& SceneData)
		{
			FSkeletalMeshSceneProxy* SkeletalMeshSceneProxy = nullptr;

			// Opaque
			auto& OpaqueRenderData = SceneData->OpaqueSceneProxyRenderData;
			for (auto Iter = OpaqueRenderData.begin(); Iter != OpaqueRenderData.end(); ++Iter)
			{
				for (auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end();)
				{
					PrimitiveIter = std::find_if(PrimitiveIter,
												Iter->second.end(),
												[PrimitiveID](const FPrimitiveRenderData& A)
												{
													return A.PrimitiveID == PrimitiveID;
												});
					if (PrimitiveIter != Iter->second.end())
					{
						SkeletalMeshSceneProxy = dynamic_cast<FSkeletalMeshSceneProxy*>(PrimitiveIter->SceneProxy.get());
						if (SkeletalMeshSceneProxy)
						{
							for (int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
							{
								SkeletalMeshSceneProxy->BoneFinalMatrices[BoneIndex] = FinalMatrices[BoneIndex];
							}
						}
						++PrimitiveIter;
					}
				}
			}

			// Masked
			auto& MaskedRenderData = SceneData->MaskedSceneProxyRenderData;
			for (auto Iter = MaskedRenderData.begin(); Iter != MaskedRenderData.end(); ++Iter)
			{
				for (auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end();)
				{
					PrimitiveIter = std::find_if(PrimitiveIter,
												Iter->second.end(),
												[PrimitiveID](const FPrimitiveRenderData& A)
												{
													return A.PrimitiveID == PrimitiveID;
												});
					if (PrimitiveIter != Iter->second.end())
					{
						SkeletalMeshSceneProxy = dynamic_cast<FSkeletalMeshSceneProxy*>(PrimitiveIter->SceneProxy.get());
						if (SkeletalMeshSceneProxy)
						{
							for (int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
							{
								SkeletalMeshSceneProxy->BoneFinalMatrices[BoneIndex] = FinalMatrices[BoneIndex];
							}
						}
						++PrimitiveIter;
					}
				}
			}

			//Translucent
			auto& TranslucentRenderData = SceneData->TranslucentSceneProxyRenderData;
			for (auto Iter = TranslucentRenderData.begin(); Iter != TranslucentRenderData.end(); ++Iter)
			{
				for (auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end();)
				{
					PrimitiveIter = std::find_if(PrimitiveIter,
												Iter->second.end(),
												[PrimitiveID](const FPrimitiveRenderData& A)
												{
													return A.PrimitiveID == PrimitiveID;
												});
					if (PrimitiveIter != Iter->second.end())
					{
						SkeletalMeshSceneProxy = dynamic_cast<FSkeletalMeshSceneProxy*>(PrimitiveIter->SceneProxy.get());
						if (SkeletalMeshSceneProxy)
						{
							for (int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
							{
								SkeletalMeshSceneProxy->BoneFinalMatrices[BoneIndex] = FinalMatrices[BoneIndex];
							}
						}
						++PrimitiveIter;
					}
				}
			}

			// Deferred
			auto& DeferredRenderData = SceneData->DeferredSceneProxyRenderData;
			for (auto Iter = DeferredRenderData.begin(); Iter != DeferredRenderData.end(); ++Iter)
			{
				for (auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end();)
				{
					PrimitiveIter = std::find_if(PrimitiveIter,
						Iter->second.end(),
						[PrimitiveID](const FPrimitiveRenderData& A)
						{
							return A.PrimitiveID == PrimitiveID;
						});
					if (PrimitiveIter != Iter->second.end())
					{
						SkeletalMeshSceneProxy = dynamic_cast<FSkeletalMeshSceneProxy*>(PrimitiveIter->SceneProxy.get());
						if (SkeletalMeshSceneProxy)
						{
							for (int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
							{
								SkeletalMeshSceneProxy->BoneFinalMatrices[BoneIndex] = FinalMatrices[BoneIndex];
							}
						}
						++PrimitiveIter;
					}
				}
			}


			//Pending Add
			auto p = SceneData->PendingAddSceneProxies.find(PrimitiveID);
			if (p != SceneData->PendingAddSceneProxies.end())
			{
				for (int i = 0; i < p->second.size(); ++i)
				{
					SkeletalMeshSceneProxy = dynamic_cast<FSkeletalMeshSceneProxy*>(p->second[i].get());
					if (SkeletalMeshSceneProxy)
					{
						for (int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
						{
							SkeletalMeshSceneProxy->BoneFinalMatrices[BoneIndex] = FinalMatrices[BoneIndex];
						}
					}
				}
			}
		};
		ENQUEUE_RENDER_COMMAND(Lambda);
	}
}

void FScene::SetMaterialScalarParam_RenderThread(UINT PrimitiveID, UINT MeshIndex, const std::string& ParamName, float Value)
{
	// Opaque
	for (const auto& RenderData : OpaqueSceneProxyRenderData)
	{
		auto TargetRenderData = std::ranges::find_if(RenderData.second,[PrimitiveID, MeshIndex](const FPrimitiveRenderData& A){
														return A.PrimitiveID == PrimitiveID && A.MeshIndex == MeshIndex;
													});

		if (TargetRenderData != RenderData.second.end())
		{
			std::shared_ptr<UMaterialInstance> MaterialInstance = std::dynamic_pointer_cast<UMaterialInstance>(TargetRenderData->MaterialInterface);
			if (MaterialInstance)
			{
				MaterialInstance->SetScalarParam(ParamName, Value);
				return;
			}
		}
	}
	// Masked
	for (const auto& RenderData : MaskedSceneProxyRenderData)
	{
		auto TargetRenderData = std::ranges::find_if(RenderData.second,
													[PrimitiveID, MeshIndex](const FPrimitiveRenderData& A)
													{
														return A.PrimitiveID == PrimitiveID && A.MeshIndex == MeshIndex;
													});

		if (TargetRenderData != RenderData.second.end())
		{
			std::shared_ptr<UMaterialInstance> MaterialInstance = std::dynamic_pointer_cast<UMaterialInstance>(TargetRenderData->MaterialInterface);
			if (MaterialInstance)
			{
				MaterialInstance->SetScalarParam(ParamName, Value);
				return;
			}
		}
	}
	// Translucent
	for (const auto& RenderData : TranslucentSceneProxyRenderData)
	{
		auto TargetRenderData = std::ranges::find_if(RenderData.second,
													[PrimitiveID, MeshIndex](const FPrimitiveRenderData& A)
													{
														return A.PrimitiveID == PrimitiveID && A.MeshIndex == MeshIndex;
													});

		if (TargetRenderData != RenderData.second.end())
		{
			std::shared_ptr<UMaterialInstance> MaterialInstance = std::dynamic_pointer_cast<UMaterialInstance>(TargetRenderData->MaterialInterface);
			if (MaterialInstance)
			{
				MaterialInstance->SetScalarParam(ParamName, Value);
				return;
			}
		}
	}
	// Deferred
	for (const auto& RenderData : DeferredSceneProxyRenderData)
	{
		auto TargetRenderData = std::ranges::find_if(RenderData.second,
			[PrimitiveID, MeshIndex](const FPrimitiveRenderData& A)
			{
				return A.PrimitiveID == PrimitiveID && A.MeshIndex == MeshIndex;
			});

		if (TargetRenderData != RenderData.second.end())
		{
			std::shared_ptr<UMaterialInstance> MaterialInstance = std::dynamic_pointer_cast<UMaterialInstance>(TargetRenderData->MaterialInterface);
			if (MaterialInstance)
			{
				MaterialInstance->SetScalarParam(ParamName, Value);
				return;
			}
		}
	}
}

void FScene::SetTextureParam_RenderThread(UINT PrimitiveID, UINT MeshIndex, UINT TextureSlot, std::shared_ptr<UTexture> Texture)
{
	// Opaque
	for (const auto& RenderData : OpaqueSceneProxyRenderData)
	{
		auto TargetRenderData = std::ranges::find_if(RenderData.second,
													[PrimitiveID, MeshIndex](const FPrimitiveRenderData& A)
													{
														return A.PrimitiveID == PrimitiveID && A.MeshIndex == MeshIndex;
													});

		if (TargetRenderData != RenderData.second.end())
		{
			std::shared_ptr<UMaterialInstance> MaterialInstance = std::dynamic_pointer_cast<UMaterialInstance>(TargetRenderData->MaterialInterface);
			if (MaterialInstance)
			{
				MaterialInstance->SetTextureParam(TextureSlot, Texture);
				return;
			}
		}
	}
	// Masked
	for (const auto& RenderData : MaskedSceneProxyRenderData)
	{
		auto TargetRenderData = std::ranges::find_if(RenderData.second,
													[PrimitiveID, MeshIndex](const FPrimitiveRenderData& A)
													{
														return A.PrimitiveID == PrimitiveID && A.MeshIndex == MeshIndex;
													});

		if (TargetRenderData != RenderData.second.end())
		{
			std::shared_ptr<UMaterialInstance> MaterialInstance = std::dynamic_pointer_cast<UMaterialInstance>(TargetRenderData->MaterialInterface);
			if (MaterialInstance)
			{
				MaterialInstance->SetTextureParam(TextureSlot, Texture);
				return;
			}
		}
	}
	// Translucent
	for (const auto& RenderData : TranslucentSceneProxyRenderData)
	{
		auto TargetRenderData = std::ranges::find_if(RenderData.second,
													[PrimitiveID, MeshIndex](const FPrimitiveRenderData& A)
													{
														return A.PrimitiveID == PrimitiveID && A.MeshIndex == MeshIndex;
													});

		if (TargetRenderData != RenderData.second.end())
		{
			std::shared_ptr<UMaterialInstance> MaterialInstance = std::dynamic_pointer_cast<UMaterialInstance>(TargetRenderData->MaterialInterface);
			if (MaterialInstance)
			{
				MaterialInstance->SetTextureParam(TextureSlot, Texture);
				return;
			}
		}
	}
	// deferred
	for (const auto& RenderData : DeferredSceneProxyRenderData)
	{
		auto TargetRenderData = std::ranges::find_if(RenderData.second,
			[PrimitiveID, MeshIndex](const FPrimitiveRenderData& A)
			{
				return A.PrimitiveID == PrimitiveID && A.MeshIndex == MeshIndex;
			});

		if (TargetRenderData != RenderData.second.end())
		{
			std::shared_ptr<UMaterialInstance> MaterialInstance = std::dynamic_pointer_cast<UMaterialInstance>(TargetRenderData->MaterialInterface);
			if (MaterialInstance)
			{
				MaterialInstance->SetTextureParam(TextureSlot, Texture);
				return;
			}
		}
	}
}

void FScene::SetNiagaraEffectActivate_GameThread(std::vector<std::shared_ptr<FNiagaraSceneProxy>>& TargetSceneProxies, bool bNewActivate)
{
	// 벡터의 크기가 클 경우를 대비하여 참조 캡쳐를 전달
	auto Lambda = [&TargetSceneProxies, bNewActivate](std::shared_ptr<FScene>& SceneData)
	{
		for (auto& Target : TargetSceneProxies)
		{
			bNewActivate ? Target->Activate() : Target->Deactivate();
		}
	};
	ENQUEUE_RENDER_COMMAND(Lambda);
}

static void DrawShadowMapSceneProxies(ELightType Type, const std::unordered_map<UINT, std::vector<FPrimitiveRenderData>>& SceneProxies, const std::array<std::array<std::shared_ptr<UMaterialInterface>,2>,static_cast<UINT>(ELightType::Count)>& ShadowMaterials)
{
	for (const std::vector<FPrimitiveRenderData>& Proxies : SceneProxies | std::views::values)
	{
		for (const FPrimitiveRenderData& RenderData : Proxies)
		{
			int SkeletalMesh = static_cast<int>(RenderData.SceneProxy->IsSkeletalMesh());
			ShadowMaterials[static_cast<int>(Type)][SkeletalMesh]->Binding();
			RenderData.SceneProxy->Draw();
		}
	}
}

void FScene::DrawShadowMap(ELightType Type) const
{
	GDirectXDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DrawShadowMapSceneProxies(Type, DeferredSceneProxyRenderData, M_LightShadow);
	DrawShadowMapSceneProxies(Type, OpaqueSceneProxyRenderData, M_LightShadow);
	DrawShadowMapSceneProxies(Type, MaskedSceneProxyRenderData, M_LightShadow);
	//DrawShadowMapSceneProxies(TranslucentSceneProxyRenderData);
}



static void DrawSceneProxies(const std::shared_ptr<FScene>& SceneData, const std::unordered_map<UINT, std::vector<FPrimitiveRenderData>>& RenderDataSceneProxies)
{
	GDirectXDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for (const auto& SceneProxies : RenderDataSceneProxies | std::views::values)
	{
		bool bIsBinding = false;
		for (const auto& SceneProxy : SceneProxies)
		{
			if (SceneProxy.SceneProxy->IsSphereInCameraFrustum(&SceneData->GetViewMatrices().Frustum))
			{
				if (!bIsBinding)
				{
					SceneProxy.MaterialInterface->Binding();
					bIsBinding = true;
				}
				// 머테리얼 파라미터 설정 (Material::Binding 내에서 기본 디폴트값이 매핑되며,
				// MaterialInstance에서 오버라이드 한 파라미터만 세팅됨
				SceneProxy.MaterialInterface->BindingMaterialInstanceUserParam();
				SceneProxy.SceneProxy->Draw();	
			}
			
		}
	}
}

void FScene::DrawScene_RenderThread(std::shared_ptr<FScene> SceneData)
{
	//RenderFPS 측정
	{
		using clock        = std::chrono::high_resolution_clock;
		using microseconds = std::chrono::microseconds;

		static auto prevTime       = clock::now();
		static auto prevUpdateTime = clock::now();
		static int  frameCount     = 0;

		// 현재 시간
		auto currentTime = clock::now();
		auto duration    = std::chrono::duration_cast<microseconds>(currentTime - prevTime);
		SceneData->DeltaSeconds = static_cast<float>( static_cast<double>(duration.count()) / 1'000'000.0);
		prevTime         = currentTime;

		++frameCount;

		auto elapsed = std::chrono::duration_cast<microseconds>(currentTime - prevUpdateTime).count();
	
		if (elapsed >= 1'000'000)
		{
			RenderFPS = static_cast<float>(frameCount) * 1'000'000.0f / elapsed;

			frameCount     = 0;
			prevUpdateTime = currentTime;
		}
	}
	// 프레임 단위 세팅
	{
		{
			// Frame 상수 버퍼 설정
			{
				// 뷰, 프로젝션 행렬
				{
					FrameConstantBuffer fcb;
					// TODO: 카메라 구현 시 수정

					fcb.View = (SceneData->GetViewMatrix());
					fcb.ViewInv = XMMatrixInverse(nullptr, fcb.View);
					// TODO: 카메라 구현 시 수정
					fcb.Projection = (SceneData->GetProjectionMatrix());

					fcb.Time      = GEngine->GetTimeSeconds();
					fcb.DeltaTime = GEngine->GetDeltaSeconds();
#ifdef WITH_EDITOR
					fcb.Resolution = GDirectXDevice->GetEditorResolution();
#else
					fcb.Resolution = GDirectXDevice->GetResolution();
#endif
					fcb.LightCount = SceneData->CurrentFrameLightInfo.size();
					GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_PerFrame, &fcb, sizeof(fcb));
				}

				// 라이팅 관련
				{
					// 버퍼크기가 모자라면 재할당
					if (SceneData->LightBuffer->GetElementCount() < SceneData->CurrentFrameLightInfo.size())
					{
						SceneData->LightBuffer->Create(sizeof(FLightInfo_ToGPU), (UINT)SceneData->CurrentFrameLightInfo.size(), SB_TYPE::SRV_ONLY, true);
					}
					
					// LightInfo 정보를 구조화버퍼로 보내고 레지스터 바인딩
					if (!SceneData->CurrentFrameLightInfo.empty())
					{
						std::vector<FLightInfo_ToGPU> Data(SceneData->CurrentFrameLightInfo.size());
						for (size_t i = 0; i < SceneData->CurrentFrameLightInfo.size(); ++i)
						{
							const FLightInfo& LightInfo = SceneData->CurrentFrameLightInfo[i];
							Data[i] = FLightInfo_ToGPU{LightInfo.LightType,LightInfo.WorldPos,LightInfo.WorldDir,LightInfo.LightColor,LightInfo.LightAmbient,LightInfo.Radius,LightInfo.Angle};
						}
						SceneData->LightBuffer->SetData(Data.data(), (UINT)Data.size());
						SceneData->LightBuffer->Binding(14);
					}
				}
			}

			GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::SwapChain)->ClearRenderTarget();	
			GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::SwapChain)->ClearDepthStencilTarget();
			GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Deferred)->ClearRenderTarget();
			GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Deferred)->ClearDepthStencilTarget();
			GDirectXDevice->SetDSState(EDepthStencilStateType::DST_LESS);
			SceneData->SetRSViewport();

			// Deferred 렌더링
			{
				GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Deferred)->OMSet();
				// Deferred 오브젝트 렌더링
				DrawSceneProxies(SceneData, SceneData->DeferredSceneProxyRenderData);

				// Decal 오브젝트 렌더링
				GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Decal)->OMSet();
				for (FDecalInfo& DecalInfo : SceneData->CurrentFrameDecalInfo)
				{
					// 같은 구조체를 가지는 FLightIndex를 활용해 데칼의 bIsLight 정보를 전달
					FLightInfoConstantBuffer DecalData;
					DecalData.LightIndex = DecalInfo.bIsLight;
					GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_LightIndex, &DecalData, sizeof(DecalData));
					DecalInfo.Render();
				}


				// Deferred 라이팅 렌더링
				GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Light)->ClearRenderTarget();

				const std::shared_ptr<UTexture>& PositionTexture = UTexture::GetTextureCache("PositionTargetTex");
				GDirectXDevice->GetDeviceContext()->PSSetShaderResources(0,1, PositionTexture->GetSRV().GetAddressOf());
				const std::shared_ptr<UTexture>& NormalTexture = UTexture::GetTextureCache("NormalTargetTex");
				GDirectXDevice->GetDeviceContext()->PSSetShaderResources(1,1, NormalTexture->GetSRV().GetAddressOf());
				
				int LightSize = static_cast<int>(SceneData->CurrentFrameLightInfo.size());
				for (int i = 0; i < LightSize; ++i)
				{
					// 라이트 정보를 상수버퍼에 바인딩해준 뒤
					FLightInfoConstantBuffer LightInfo;
					LightInfo.LightIndex = i;
					LightInfo.LightVP = SceneData->CurrentFrameLightInfo[i].LightVP;
					GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_LightIndex, &LightInfo, sizeof(LightInfo));

					// 그림자 맵을 그리고
					SceneData->CurrentFrameLightInfo[i].ShadowMultiRenderTarget->OMSet();
					SceneData->CurrentFrameLightInfo[i].ShadowMultiRenderTarget->ClearRenderTarget();
					SceneData->CurrentFrameLightInfo[i].ShadowMultiRenderTarget->ClearDepthStencilTarget();
					// 하드코딩
					D3D11_VIEWPORT ViewPort;
					if (SceneData->CurrentFrameLightInfo[i].LightType == static_cast<int>(ELightType::Directional))
					{
						ViewPort = D3D11_VIEWPORT{0,0,8192,8192,0,1};	
					}
					else
					{
						ViewPort = D3D11_VIEWPORT{0,0,512,512,0,1};
					}
					GDirectXDevice->GetDeviceContext()->RSSetViewports(1, &ViewPort);
					SceneData->DrawShadowMap(static_cast<ELightType>(SceneData->CurrentFrameLightInfo[i].LightType));

					

					// 렌더링을 진행한다.
					GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Light)->OMSet();
					SceneData->SetRSViewport();
					// 그린 그림자 맵을 바인딩 한 다음에
					const std::shared_ptr<UTexture>& ShadowMap = SceneData->CurrentFrameLightInfo[i].ShadowMultiRenderTarget->GetRenderTargetTexture(0);
					GDirectXDevice->GetDeviceContext()->PSSetShaderResources(2,1, ShadowMap->GetSRV().GetAddressOf());

					SceneData->CurrentFrameLightInfo[i].Render();
				}

			}
			

			

			// Deferred로 그린 gbuffer 텍스쳐를 렌더타겟에 머지
			{
				// SwapChain MRT로 OMSet
				constexpr float ClearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
				SceneData->SetDrawScenePipeline(ClearColor);
				SceneData->DeferredMergeRenderData.MaterialInterface->Binding();
				const std::shared_ptr<UTexture>& ColorTexture = UTexture::GetTextureCache("ColorTargetTex");
				GDirectXDevice->GetDeviceContext()->PSSetShaderResources(0,1,ColorTexture->GetSRV().GetAddressOf());
				const std::shared_ptr<UTexture>& DiffuseTexture = UTexture::GetTextureCache("DiffuseTargetTex");
				GDirectXDevice->GetDeviceContext()->PSSetShaderResources(1,1,DiffuseTexture->GetSRV().GetAddressOf());
				const std::shared_ptr<UTexture>& SpecularTexture = UTexture::GetTextureCache("SpecularTargetTex");
				GDirectXDevice->GetDeviceContext()->PSSetShaderResources(2,1,SpecularTexture->GetSRV().GetAddressOf());

				SceneData->DeferredMergeRenderData.SceneProxy->Draw();
			}

			// 포워드 렌더링 진행
			{
				GDirectXDevice->SetDSState(EDepthStencilStateType::DST_LESS);
				DrawSceneProxies(SceneData, SceneData->OpaqueSceneProxyRenderData);
				GDirectXDevice->SetBSState(EBlendStateType::BST_AlphaBlend);
				DrawSceneProxies(SceneData, SceneData->MaskedSceneProxyRenderData);
				DrawSceneProxies(SceneData, SceneData->TranslucentSceneProxyRenderData);
			}
			
#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
			// 디버그 드로우
			static std::shared_ptr<UMaterial> DebugMaterial = nullptr;
			if (!DebugMaterial)
			{
				DebugMaterial = std::dynamic_pointer_cast<UMaterial>(UMaterial::GetMaterialCache("M_Debug"));
			}

			if (DebugMaterial)
			{
				DebugMaterial->Binding();
				GDirectXDevice->SetRSState(ERasterizerType::RT_WireFrame);
				for (FDebugRenderData& DebugData : SceneData->DebugRenderData)
				{
					ObjConstantBuffer ocb;
					DirectX::XMMATRIX world = DebugData.Transform.ToMatrixWithScale();
					// 조명 - 노말벡터의 변환을 위해 역전치 행렬 추가
					ocb.InvTransposeMatrix = (XMMatrixInverse(nullptr, world));
					ocb.World              = XMMatrixTranspose(world);
					GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_PerObject, &ocb, sizeof(ocb));

					FSystemParamConstantBuffer ColorBuffer;
					ColorBuffer.Float4_1 = DebugData.DebugColor;
					GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_SystemParam, &ColorBuffer, sizeof(ColorBuffer));


					DebugData.ShapeComp->DebugDraw_RenderThread();
					DebugData.RemainTime -= SceneData->DeltaSeconds;
					
				}
				std::erase_if(SceneData->DebugRenderData, [](const FDebugRenderData& Data)
				{
					return Data.RemainTime <= 0;
				}); 
			}
#endif


			
		}

		// Draw UI
		{
			// 머테리얼 바인딩
			SceneData->M_Widget->Binding();
			for (const FWidgetRenderData& RenderData : SceneData->CurrentFrameWidgetRenderData)
			{
				// 텍스트 렌더링
				if(!RenderData.TextData.empty())
				{
					const Microsoft::WRL::ComPtr<ID2D1DeviceContext> Text2DDeviceContext = GDirectXDevice->Get2DDeviceContext();
					Text2DDeviceContext->BeginDraw();

					// 텍스트 포맷 생성
					Microsoft::WRL::ComPtr<IDWriteTextFormat> TextFormat;
					GDirectXDevice->Get2DDevice()->CreateTextFormat(RenderData.FontName, RenderData.FontSize, TextFormat);
					// 텍스트 포맷 Alignment 설정
					TextFormat->SetTextAlignment(RenderData.TextHorizontalAlignment);
					TextFormat->SetParagraphAlignment(RenderData.TextVerticalAlignment);

					// Brush 생성
					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> ColorBrush;
					Text2DDeviceContext->CreateSolidColorBrush(
						D2D1::ColorF(RenderData.Tint.x,RenderData.Tint.y,RenderData.Tint.z,RenderData.Tint.w),
						ColorBrush.GetAddressOf()
					);

					
					Text2DDeviceContext->DrawTextW(
						RenderData.TextData.c_str(),
						static_cast<UINT32>(RenderData.TextData.size()),
						TextFormat.Get(),
						D2D1::RectF(RenderData.Left,RenderData.Top,RenderData.Left+RenderData.Width,RenderData.Top+RenderData.Height),
						ColorBrush.Get()
					);

					if (FAILED(GDirectXDevice->Get2DDeviceContext()->EndDraw()))
					{
						assert(nullptr && "TextRenderingError");
					}
				}
				else
				{
					// ConstantBuffer 바인딩
					FWidgetConstantBuffer WCB{RenderData.Tint,RenderData.Left, RenderData.Top, RenderData.Width,RenderData.Height};
					GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_Widget, &WCB, sizeof(WCB));

					// 텍스쳐 바인딩
					GDirectXDevice->GetDeviceContext()->PSSetShaderResources(0,1, RenderData.Texture->GetSRV().GetAddressOf());

					// 드로우
					SceneData->WidgetStaticMeshSceneProxy->Draw();	
				}
				
			}
		}
	}


	SceneData->AfterDrawSceneAction(SceneData);

	HR(GDirectXDevice->GetSwapChain()->Present(0, 0));

	EndRenderFrame_RenderThread(SceneData);
}

void FScene::SetDrawScenePipeline(const float* ClearColor)
{
	GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::SwapChain)->OMSet();
}
void FScene::SetRSViewport()
{
	GDirectXDevice->GetDeviceContext()->RSSetViewports(1, GDirectXDevice->GetScreenViewport());
}


DirectX::XMMATRIX FScene::GetViewMatrix()
{
	return ViewMatrices.GetViewMatrix();
}

DirectX::XMMATRIX FScene::GetProjectionMatrix()
{
	return ViewMatrices.GetProjectionMatrix();
}

void FScene::SetSkyBoxTexture_RenderThread(const std::string& NewEnvironmentTextureName)
{
	if (GDirectXDevice)
	{
		const std::shared_ptr<UTexture>& Texture = UTexture::GetTextureCache(NewEnvironmentTextureName);
		const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& DeviceContext = GDirectXDevice->GetDeviceContext();
		if (Texture)
		{
			if (Texture->GetDesc().MipLevels == 1)
			{
				DeviceContext->GenerateMips(Texture->GetSRV().Get());
			}
			DeviceContext->PSSetShaderResources(50,1, Texture->GetSRV().GetAddressOf());		
		}
		
	}
}

void FScene::SetFrameLightInfo(const std::vector<FLightInfo>& LightInfo)
{
	
	CurrentFrameLightInfo.clear();
	CurrentFrameLightInfo.resize(LightInfo.size());
	std::ranges::copy(LightInfo, CurrentFrameLightInfo.begin());
}

void FScene::SetFrameDecalInfo(const std::vector<FDecalInfo>& DecalInfo)
{
	CurrentFrameDecalInfo.clear();
	CurrentFrameDecalInfo.resize(DecalInfo.size());
	std::ranges::copy(DecalInfo, CurrentFrameDecalInfo.begin());
}

void FScene::SetFrameWidgetRenderData(const std::vector<FWidgetRenderData>& WidgetRenderData)
{
	CurrentFrameWidgetRenderData.clear();
	CurrentFrameWidgetRenderData.resize(WidgetRenderData.size());
	std::ranges::copy(WidgetRenderData, CurrentFrameWidgetRenderData.begin());
}


void FScene::EndRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData)
{
	SceneData->bIsFrameStart = false;
}

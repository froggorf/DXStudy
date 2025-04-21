// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"

#include "EditorScene.h"
#include "renderingthread.h"
#include "Engine/SceneProxy/FSkeletalMeshSceneProxy.h"

std::shared_ptr<FScene> FRenderCommandExecutor::CurrentSceneData = nullptr;


void FScene::InitLevelData()
{
	OpaqueSceneProxyRenderData.clear();
	MaskedSceneProxyRenderData.clear();
	TranslucentSceneProxyRenderData.clear();
	
	PendingAddSceneProxies.clear();
	PendingDeleteSceneProxies.clear();

	PendingNewTransformProxies.clear();
	bMustResetLevelDataAtEndFrame = false;
}

void FScene::BeginRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData, UINT GameThreadFrameCount)
{
	RenderingThreadFrameCount = GameThreadFrameCount;
	SceneData->BeginRenderFrame();
}

void FScene::BeginRenderFrame()
{
	if(bMustResetLevelDataAtEndFrame) bMustResetLevelDataAtEndFrame=false;

	if(bIsFrameStart)
	{
		//MY_LOG("RenderCommand", EDebugLogLevel::DLL_Error,"RenderThread is already start");
		return;
	}

	
	bIsFrameStart = true;

	for(const auto& NewPrimitiveProxy : PendingAddSceneProxies)
	{
		PrimitiveRenderData RenderData;
		RenderData.MeshIndex = NewPrimitiveProxy.second->GetMeshIndex();
		RenderData.PrimitiveID = NewPrimitiveProxy.first;
		RenderData.SceneProxy = NewPrimitiveProxy.second;
		UINT MaterialID = RenderData.SceneProxy->GetMaterialID();

		switch(NewPrimitiveProxy.second->GetBlendMode())
		{
		case EBlendMode::BM_Opaque:
		OpaqueSceneProxyRenderData[MaterialID].emplace_back(RenderData);
		break;
		case EBlendMode::BM_Masked:
			MaskedSceneProxyRenderData[MaterialID].emplace_back(RenderData);
		break;
		case EBlendMode::BM_Translucent:
			TranslucentSceneProxyRenderData[MaterialID].emplace_back(RenderData);
		break;
		default:
		// 잘못된 데이터
			assert(0);
			break;
		}
	}
	PendingAddSceneProxies.clear();

	// 새로운 위치정보 갱신
	for(const auto& NewTransform : PendingNewTransformProxies)
	{
		UINT FindPrimitiveID = NewTransform.first;

		// Opaque
		for(auto Iter = OpaqueSceneProxyRenderData.begin(); Iter != OpaqueSceneProxyRenderData.end() ; ++Iter)
		{
			for(auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end(); )
			{
				PrimitiveIter = std::find_if(PrimitiveIter, Iter->second.end(), [FindPrimitiveID](const PrimitiveRenderData& A){ return A.PrimitiveID == FindPrimitiveID;});
				if(PrimitiveIter != Iter->second.end())
				{
					PrimitiveIter->SceneProxy->SetSceneProxyWorldTransform(NewTransform.second);
					++PrimitiveIter;
				}
			}
		}


		// Masked
		for(auto Iter = MaskedSceneProxyRenderData.begin(); Iter != MaskedSceneProxyRenderData.end() ; ++Iter)
		{
			for(auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end(); )
			{
				PrimitiveIter = std::find_if(PrimitiveIter, Iter->second.end(), [FindPrimitiveID](const PrimitiveRenderData& A){ return A.PrimitiveID == FindPrimitiveID;});
				if(PrimitiveIter != Iter->second.end())
				{
					PrimitiveIter->SceneProxy->SetSceneProxyWorldTransform(NewTransform.second);
					++PrimitiveIter;
				}
			}
		}

		// Translucent
		for(auto Iter = TranslucentSceneProxyRenderData.begin(); Iter != TranslucentSceneProxyRenderData.end() ; ++Iter)
		{
			for(auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end(); )
			{
				PrimitiveIter = std::find_if(PrimitiveIter, Iter->second.end(), [FindPrimitiveID](const PrimitiveRenderData& A){ return A.PrimitiveID == FindPrimitiveID;});
				if(PrimitiveIter != Iter->second.end())
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
	if(PrimitiveID > 0)
	{
		auto Lambda = [PrimitiveID, FinalMatrices](std::shared_ptr<FScene>& SceneData)
			{
				FSkeletalMeshSceneProxy* SkeletalMeshSceneProxy = nullptr;

				// Opaque
				auto OpaqueRenderData = SceneData->OpaqueSceneProxyRenderData;
				for(auto Iter = OpaqueRenderData.begin(); Iter != OpaqueRenderData.end() ; ++Iter)
				{

					for(auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end(); )
					{
						PrimitiveIter = std::find_if(PrimitiveIter, Iter->second.end(), [PrimitiveID](const PrimitiveRenderData& A){ return A.PrimitiveID == PrimitiveID;});
						if(PrimitiveIter != Iter->second.end())
						{
							SkeletalMeshSceneProxy = dynamic_cast<FSkeletalMeshSceneProxy*>(PrimitiveIter->SceneProxy.get());
							if(SkeletalMeshSceneProxy)
							{
								for(int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
								{
									SkeletalMeshSceneProxy->BoneFinalMatrices[BoneIndex] = FinalMatrices[BoneIndex];	
								}

							}	
							++PrimitiveIter;
						}
					}
				}

				// Masked
				auto MaskedRenderData = SceneData->MaskedSceneProxyRenderData;
				for(auto Iter = MaskedRenderData.begin(); Iter != MaskedRenderData.end() ; ++Iter)
				{

					for(auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end(); )
					{
						PrimitiveIter = std::find_if(PrimitiveIter, Iter->second.end(), [PrimitiveID](const PrimitiveRenderData& A){ return A.PrimitiveID == PrimitiveID;});
						if(PrimitiveIter != Iter->second.end())
						{
							SkeletalMeshSceneProxy = dynamic_cast<FSkeletalMeshSceneProxy*>(PrimitiveIter->SceneProxy.get());
							if(SkeletalMeshSceneProxy)
							{
								for(int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
								{
									SkeletalMeshSceneProxy->BoneFinalMatrices[BoneIndex] = FinalMatrices[BoneIndex];	
								}

							}	
							++PrimitiveIter;
						}
					}
				}

				//Translucent
				auto TranslucentRenderData = SceneData->TranslucentSceneProxyRenderData;
				for(auto Iter = TranslucentRenderData.begin(); Iter != TranslucentRenderData.end() ; ++Iter)
				{

					for(auto PrimitiveIter = Iter->second.begin(); PrimitiveIter != Iter->second.end(); )
					{
						PrimitiveIter = std::find_if(PrimitiveIter, Iter->second.end(), [PrimitiveID](const PrimitiveRenderData& A){ return A.PrimitiveID == PrimitiveID;});
						if(PrimitiveIter != Iter->second.end())
						{
							SkeletalMeshSceneProxy = dynamic_cast<FSkeletalMeshSceneProxy*>(PrimitiveIter->SceneProxy.get());
							if(SkeletalMeshSceneProxy)
							{
								for(int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
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
				if(p != SceneData->PendingAddSceneProxies.end())
				{
					SkeletalMeshSceneProxy = dynamic_cast<FSkeletalMeshSceneProxy*>(p->second.get());

				}
				if(SkeletalMeshSceneProxy)
				{
					for(int BoneIndex = 0; BoneIndex < MAX_BONES; ++BoneIndex)
					{
						SkeletalMeshSceneProxy->BoneFinalMatrices[BoneIndex] = FinalMatrices[BoneIndex];	
					}

				}	

			};
		ENQUEUE_RENDER_COMMAND(Lambda);
	}
}

void FScene::DrawScene_RenderThread(std::shared_ptr<FScene> SceneData)
{
	//RenderFPS 측정
	{
		using clock = std::chrono::high_resolution_clock;
		using microseconds = std::chrono::microseconds;

		static auto prevTime = clock::now();
		static auto prevUpdateTime = clock::now();
		static int frameCount = 0;

		// 현재 시간
		auto currentTime = clock::now();
		auto duration = std::chrono::duration_cast<microseconds>(currentTime - prevTime);
		prevTime = currentTime;

		++frameCount;

		auto elapsed = std::chrono::duration_cast<microseconds>(currentTime - prevUpdateTime).count();
		if (elapsed >= 1'000'000) {
			RenderFPS = static_cast<float>(frameCount) * 1'000'000.0f / elapsed;
			
			frameCount = 0;
			prevUpdateTime = currentTime;
		}
	}



	// 프레임 단위 세팅
	{
		const float ClearColor[] = {0.0f, 0.0f, 0.0f,1.0f};
		SceneData->SetDrawScenePipeline(ClearColor);
		
		GDirectXDevice->GetDeviceContext()->ClearRenderTargetView( GDirectXDevice->GetRenderTargetView().Get(), ClearColor);
		GDirectXDevice->GetDeviceContext()->ClearDepthStencilView( GDirectXDevice->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);


		{

			// Frame 상수 버퍼 설정
			{
				// 뷰, 프로젝션 행렬
				{
					FrameConstantBuffer fcb;
					// TODO: 카메라 구현 시 수정

					fcb.View = XMMatrixTranspose(SceneData->GetViewMatrix());
					// TODO: 카메라 구현 시 수정
					//XMMATRIX ProjMat = XMMatrixPerspectiveFovLH(0.5*XM_PI, 1600.0f/1200.0f, 1.0f, 1000.0f);
					fcb.Projection = XMMatrixTranspose(SceneData->GetProjectionMatrix());
					fcb.LightView = XMMatrixTranspose(XMMatrixIdentity());//m_LightView
					fcb.LightProj = XMMatrixTranspose(XMMatrixIdentity()); //m_LightProj
					GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_PerFrame, &fcb, sizeof(fcb));
					

				}

				// 라이팅 관련
				{
					LightFrameConstantBuffer lfcb;
					DirectionalLight TempDirectionalLight;
					TempDirectionalLight.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
					TempDirectionalLight.Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
					TempDirectionalLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
					XMStoreFloat3(&TempDirectionalLight.Direction, XMVector3Normalize(XMVectorSet(0.57735f, -0.57735f, 0.57735f,0.0f)));
					lfcb.gDirLight = TempDirectionalLight;

					PointLight TempPointLight;
					TempPointLight.Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
					TempPointLight.Diffuse  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
					TempPointLight.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
					TempPointLight.Att      = XMFLOAT3(0.0f, 0.1f, 0.0f);
					TempPointLight.Position = XMFLOAT3(0.0f,-2.5f,0.0f);
					TempPointLight.Range    = 0.0f;
					lfcb.gPointLight = TempPointLight;
					// Convert Spherical to Cartesian coordinates.
					lfcb.gEyePosW = XMFLOAT3{GEngine->Test_DeleteLater_GetCameraPosition()};
					GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_Light, &lfcb, sizeof(lfcb));
				}
			}



			



			// Sampler State 설정
			GDirectXDevice->GetDeviceContext()->PSSetSamplers(0, 1, GDirectXDevice->GetSamplerState().GetAddressOf());

			for(const auto& SceneProxies : SceneData->OpaqueSceneProxyRenderData | std::views::values)
			{
				bool bIsBinding = false;
				for(const auto& SceneProxy : SceneProxies)
				{
					if(!bIsBinding)
					{
						SceneProxy.SceneProxy->GetMaterialInterface()->Binding();
						bIsBinding = true;
					}
					SceneProxy.SceneProxy->Draw();
				}
			}
			for(const auto& SceneProxies : SceneData->MaskedSceneProxyRenderData | std::views::values)
			{
				bool bIsBinding = false;
				for(const auto& SceneProxy : SceneProxies)
				{
					if(!bIsBinding)
					{
						SceneProxy.SceneProxy->GetMaterialInterface()->Binding();
						bIsBinding = true;
					}
					SceneProxy.SceneProxy->Draw();
				}
			}
			for(const auto& SceneProxies : SceneData->TranslucentSceneProxyRenderData | std::views::values)
			{
				bool bIsBinding = false;
				for(const auto& SceneProxy : SceneProxies)
				{
					if(!bIsBinding)
					{
						SceneProxy.SceneProxy->GetMaterialInterface()->Binding();
						bIsBinding = true;
					}
					SceneProxy.SceneProxy->Draw();
				}
			}
			//if(CurrentWorld)
			//{
			//	CurrentWorld->TestDrawWorld();
			//}
			//DrawImGui();

		}
	}

	SceneData->AfterDrawSceneAction(SceneData);

	HR(GDirectXDevice->GetSwapChain()->Present(0, 0));

	// SceneProxy Render
	EndRenderFrame_RenderThread(SceneData);
}

void FScene::SetDrawScenePipeline(const float* ClearColor)
{
	GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(1, GDirectXDevice->GetRenderTargetView().GetAddressOf(),  GDirectXDevice->GetDepthStencilView().Get());
	GDirectXDevice->GetDeviceContext()->RSSetViewports(1, GDirectXDevice->GetScreenViewport());
}

XMMATRIX FScene::GetViewMatrix()
{
	return GEngine->Test_DeleteLater_GetViewMatrix();
}

XMMATRIX FScene::GetProjectionMatrix()
{
	return GEngine->Test_DeleteLater_GetProjectionMatrix();
}


void FScene::EndRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData)
{
	if(SceneData->bMustResetLevelDataAtEndFrame)
	{
		SceneData->InitLevelData();
	}

	SceneData->bIsFrameStart = false;
}



















// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "renderingthread.h"
#include "Engine/SceneProxy/FSkeletalMeshSceneProxy.h"
#include "ThirdParty/ImGui/imgui_internal.h"

std::shared_ptr<FScene> FRenderCommandExecutor::CurrentSceneData = nullptr;


void FScene::InitLevelData()
{
	PrimitiveSceneProxies.clear();
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
		PrimitiveSceneProxies[NewPrimitiveProxy.first] = NewPrimitiveProxy.second;

		/*MY_LOG("SceneProxy Add New Proxy - PrimitiveID = " + std::to_string(NewPrimitiveProxy.first), EDebugLogLevel::DLL_Display, "");*/
	}
	PendingAddSceneProxies.clear();

}


void FScene::UpdateSkeletalMeshAnimation_GameThread(UINT PrimitiveID, const std::vector<XMMATRIX>& FinalMatrices)
{
	if(PrimitiveID > 0)
	{
		auto Lambda = [PrimitiveID, FinalMatrices](std::shared_ptr<FScene>& SceneData)
			{
				auto p = SceneData->PrimitiveSceneProxies.find(PrimitiveID);\
				FSkeletalMeshSceneProxy* SkeletalMeshSceneProxy = nullptr;
				if(p != SceneData->PrimitiveSceneProxies.end())
				{
					SkeletalMeshSceneProxy = dynamic_cast<FSkeletalMeshSceneProxy*>(p->second.get());
					
				}
				else
				{
					p = SceneData->PendingAddSceneProxies.find(PrimitiveID);
					if(p != SceneData->PendingAddSceneProxies.end())
					{
						SkeletalMeshSceneProxy = dynamic_cast<FSkeletalMeshSceneProxy*>(p->second.get());	
					}
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

{	// 임시 렌더링 쓰레드 FPS 측정
	//{
	//	static GameTimer test;
	//	static bool startbool = false;
	//	if(!startbool)
	//	{
	//		test.Reset();
	//		startbool = true;
	//	}
	//	test.Tick();
	//	static int frameCnt = 0;
	//	static float timeElapsed = 0.0f;

	//	frameCnt++;

	//	// Compute averages over one second period.
	//	if( (test.TotalTime() - timeElapsed) >= 1.0f )
	//	{
	//		float fps = (float)frameCnt; // fps = frameCnt / 1
	//		float mspf = 1000.0f / fps;

	//		std::wostringstream outs;
	//		outs.precision(6);
	//		outs  << L"Rendering Thread  "
	//			<< L"FPS: " << fps << L"    " 
	//			<< L"Frame Time: " << mspf << L" (ms)";
	//		//SetWindowText(m_hMainWnd, outs.str().c_str());
	//		if(GEngine)
	//		{
	//			GEngine->GetApplication()->TestSetWindowBarName(outs);
	//		}

	//		// Reset for next average.
	//		frameCnt = 0;
	//		timeElapsed += 1.0f;
	//	}
	//}
}

	// 프레임 단위 세팅
	{
		const float ClearColor[] = {0.0f, 0.0f, 0.0f,1.0f};
		SceneData->SetDrawScenePipeline(ClearColor);
		
		GDirectXDevice->GetDeviceContext()->ClearRenderTargetView( GDirectXDevice->GetRenderTargetView().Get(), ClearColor);
		GDirectXDevice->GetDeviceContext()->ClearDepthStencilView( GDirectXDevice->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);


		{
			// 셰이더 설정
			GDirectXDevice->GetDeviceContext()->PSSetShader(GDirectXDevice->GetTexturePixelShader().Get(), nullptr, 0);
			// 상수버퍼 설정
			GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(0, 1, GDirectXDevice->GetFrameConstantBuffer().GetAddressOf());
			GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(0,1,GDirectXDevice->GetFrameConstantBuffer().GetAddressOf());
			GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(1, 1, GDirectXDevice->GetObjConstantBuffer().GetAddressOf());
			GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(1,1, GDirectXDevice->GetObjConstantBuffer().GetAddressOf());
			GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(2,1, GDirectXDevice->GetLightConstantBuffer().GetAddressOf());
			GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(3, 1, GDirectXDevice->GetSkeletalMeshConstantBuffer().GetAddressOf());

			// 인풋 레이아웃
			GDirectXDevice->GetDeviceContext()->IASetInputLayout(GDirectXDevice->GetStaticMeshInputLayout().Get());
			GDirectXDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
					GDirectXDevice->GetDeviceContext()->UpdateSubresource(GDirectXDevice->GetFrameConstantBuffer().Get(), 0, nullptr, &fcb, 0, 0);

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
					//m_PointLight.Position = XMFLOAT3(796.5f, 700.0f, 0.7549f);
					TempPointLight.Position = XMFLOAT3(0.0f,-2.5f,0.0f);
					TempPointLight.Range    = 0.0f;
					lfcb.gPointLight = TempPointLight;
					// Convert Spherical to Cartesian coordinates.
					lfcb.gEyePosW = XMFLOAT3{GEngine->Test_DeleteLater_GetCameraPosition()};
					GDirectXDevice->GetDeviceContext()->UpdateSubresource(GDirectXDevice->GetLightConstantBuffer().Get(),0,nullptr,&lfcb, 0,0);
				}
			}



			



			// Sampler State 설정
			GDirectXDevice->GetDeviceContext()->PSSetSamplers(0, 1, GDirectXDevice->GetSamplerState().GetAddressOf());

			for(const auto& SceneProxy : SceneData->PrimitiveSceneProxies)
			{
				SceneProxy.second->Draw();
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


	for(const auto& NewPrimitiveProxy : SceneData->PendingAddSceneProxies)
	{
		SceneData->PrimitiveSceneProxies[NewPrimitiveProxy.first] = NewPrimitiveProxy.second;

	}
	SceneData->PendingAddSceneProxies.clear();

	for(const auto& NewTransform : SceneData->PendingNewTransformProxies)
	{
		const auto& SceneProxy = SceneData->PrimitiveSceneProxies.find(NewTransform.first);
		if(SceneProxy != SceneData->PrimitiveSceneProxies.end())
		{
			SceneProxy->second->SetSceneProxyWorldTransform(NewTransform.second);
		}	
	}
	SceneData->PendingNewTransformProxies.clear();
	

	SceneData->bIsFrameStart = false;
}



















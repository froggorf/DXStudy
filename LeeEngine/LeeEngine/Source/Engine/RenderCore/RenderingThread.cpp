// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "renderingthread.h"
#include "ThirdParty/ImGui/imgui_internal.h"

std::shared_ptr<FScene> FRenderCommandExecutor::CurrentSceneData = nullptr;



std::unordered_map<std::string,std::function<void()>> FScene::ImGuiRenderFunctions;
std::unordered_map<std::string,std::function<void()>> FScene::ImGuizmoRenderFunctions;


// 디버깅 콘솔
std::vector<DebugText> FScene::DebugConsoleText;
std::vector<DebugText> FScene::PendingAddDebugConsoleText;
std::vector<DebugText> FScene::SearchingDebugConsoleText;
std::string FScene::DebugConsoleSearchText;

// 월드 아웃라이너
std::vector<std::shared_ptr<AActor>> FScene::WorldOutlinerActors;
std::vector<std::shared_ptr<AActor>> FScene::PendingAddWorldOutlinerActors;
std::shared_ptr<AActor> FScene::CurrentSelectedActor = nullptr;
std::vector<std::shared_ptr<USceneComponent>> FScene::SelectActorComponents;
std::vector<std::string> FScene:: SelectActorComponentNames;

// 에디터 ViewPort
bool FScene::bResizeEditorRenderTargetAtEndFrame=false;
ImVec2 FScene::ResizeEditorRenderTargetSize = {};

int FScene::CurrentSelectedComponentIndex = -1;

void FScene::BeginRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData, UINT GameThreadFrameCount)
{

	if(SceneData->bIsFrameStart)
	{
		//MY_LOG("RenderCommand", EDebugLogLevel::DLL_Error,"RenderThread is already start");
		return;
	}

	RenderingThreadFrameCount = GameThreadFrameCount;
	SceneData->bIsFrameStart = true;

#ifdef MYENGINE_BUILD_DEBUG || MYENGINE_BUILD_DEVELOPMENT
	for(const auto& Text : PendingAddDebugConsoleText)
	{
		DebugConsoleText.push_back(Text);
	}
	PendingAddDebugConsoleText.clear();
	for(const auto& NewOutlinerActor : PendingAddWorldOutlinerActors)
	{
		WorldOutlinerActors.push_back(NewOutlinerActor);
	}
	PendingAddWorldOutlinerActors.clear();
#endif

	for(const auto& NewPrimitiveProxy : SceneData->PendingAddSceneProxies)
	{
		SceneData->PrimitiveSceneProxies[NewPrimitiveProxy.first] = NewPrimitiveProxy.second;

		MY_LOG("SceneProxy Add New Proxy - PrimitiveID = " + std::to_string(NewPrimitiveProxy.first), EDebugLogLevel::DLL_Display, "");
	}
	SceneData->PendingAddSceneProxies.clear();


	
}


void FScene::DrawIMGUI_RenderThread(std::shared_ptr<FScene> SceneData)
{
	if(bIsGameKill)
	{
		return;
	}
	//========== IMGUI ==========
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	//ImGuizmo::BeginFrame();

	
	// ImGui
	for(const auto& Func : ImGuiRenderFunctions)
	{
		Func.second();
	}


	//// ImGuizmo
	/*ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0,0,io.DisplaySize.x,io.DisplaySize.y);
	for(const auto& Func : ImGuizmoRenderFunctions)
	{
		Func.second();
	}*/

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::EndFrame();

}


void FScene::DrawScene_RenderThread(std::shared_ptr<FScene> SceneData)
{

	// 임시 렌더링 쓰레드 FPS 측정
	{
		static GameTimer test;
		static bool startbool = false;
		if(!startbool)
		{
			test.Reset();
			startbool = true;
		}
		test.Tick();
		static int frameCnt = 0;
		static float timeElapsed = 0.0f;

		frameCnt++;

		// Compute averages over one second period.
		if( (test.TotalTime() - timeElapsed) >= 1.0f )
		{
			float fps = (float)frameCnt; // fps = frameCnt / 1
			float mspf = 1000.0f / fps;

			std::wostringstream outs;
			outs.precision(6);
			outs  << L"Rendering Thread  "
				<< L"FPS: " << fps << L"    " 
				<< L"Frame Time: " << mspf << L" (ms)";
			//SetWindowText(m_hMainWnd, outs.str().c_str());
			if(GEngine)
			{
				GEngine->GetApplication()->TestSetWindowBarName(outs);
			}

			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;
		}
	}
	// 프레임 단위 세팅
	{
		const float clearColor[] = {0.0f, 0.0f, 0.0f,1.0f};
#ifdef WITH_EDITOR
		GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(1, GDirectXDevice->GetEditorRenderTargetView().GetAddressOf(), GDirectXDevice->GetDepthStencilView().Get());
		GDirectXDevice->GetDeviceContext()->ClearRenderTargetView(GDirectXDevice->GetEditorRenderTargetView().Get(),clearColor);
		GDirectXDevice->SetViewPortSize(ResizeEditorRenderTargetSize.x,ResizeEditorRenderTargetSize.y);
#else
		GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(1, GDirectXDevice->GetRenderTargetView().GetAddressOf(),  GDirectXDevice->GetDepthStencilView().Get());
		GDirectXDevice->GetDeviceContext()->RSSetViewports(1, GDirectXDevice->GetScreenViewport());
#endif

		
		GDirectXDevice->GetDeviceContext()->ClearRenderTargetView( GDirectXDevice->GetRenderTargetView().Get(), clearColor);
		GDirectXDevice->GetDeviceContext()->ClearDepthStencilView( GDirectXDevice->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);


		GDirectXDevice->GetDeviceContext()->IASetInputLayout(GDirectXDevice->GetInputLayout().Get());
		{
			// 셰이더 설정
			GDirectXDevice->GetDeviceContext()->VSSetShader(GDirectXDevice->GetStaticMeshVertexShader().Get(), nullptr, 0);
			GDirectXDevice->GetDeviceContext()->PSSetShader(GDirectXDevice->GetTexturePixelShader().Get(), nullptr, 0);
			// 상수버퍼 설정
			GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(0, 1, GDirectXDevice->GetFrameConstantBuffer().GetAddressOf());
			GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(0,1,GDirectXDevice->GetFrameConstantBuffer().GetAddressOf());
			GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(1, 1, GDirectXDevice->GetObjConstantBuffer().GetAddressOf());
			GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(1,1, GDirectXDevice->GetObjConstantBuffer().GetAddressOf());
			GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(2,1, GDirectXDevice->GetLightConstantBuffer().GetAddressOf());


			// 인풋 레이아웃
			GDirectXDevice->GetDeviceContext()->IASetInputLayout(GDirectXDevice->GetInputLayout().Get());
			GDirectXDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// Frame 상수 버퍼 설정
			{
				// 뷰, 프로젝션 행렬
				{
					FrameConstantBuffer fcb;
					// TODO: 카메라 구현 시 수정

					fcb.View = XMMatrixTranspose(GEngine->Test_DeleteLater_GetViewMatrix());
					// TODO: 카메라 구현 시 수정
					//XMMATRIX ProjMat = XMMatrixPerspectiveFovLH(0.5*XM_PI, 1600.0f/1200.0f, 1.0f, 1000.0f);
					fcb.Projection = XMMatrixTranspose(GEngine->Test_DeleteLater_GetProjectionMatrix());
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

#ifdef WITH_EDITOR

	GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(1, GDirectXDevice->GetRenderTargetView().GetAddressOf(),  GDirectXDevice->GetDepthStencilView().Get());
	GDirectXDevice->GetDeviceContext()->RSSetViewports(1, GDirectXDevice->GetScreenViewport());
	GDirectXDevice->SetDefaultViewPort();

	DrawIMGUI_RenderThread(SceneData);

#endif

	HR(GDirectXDevice->GetSwapChain()->Present(0, 0));

	// SceneProxy Render
	EndRenderFrame_RenderThread(SceneData);
}

void FScene::DrawImGuiScene_RenderThread()
{
#ifdef WITH_EDITOR
	static ImVec2 PreviousViewPortSize = ImVec2(0.0f,0.0f);
	ImVec2 CurrentViewPortSize{};
	ImVec2 ScreenPos;

	if(ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove|
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse))
	{

		CurrentViewPortSize = ImGui::GetWindowSize();


		
		ImGui::Dummy(ImVec2(CurrentViewPortSize.x,20.0f));
		//CurrentViewPortSize.y -= YMargin;
		ImGui::SetCursorPos(ImVec2(0,20.0f));
		ScreenPos = ImGui::GetCursorScreenPos();
		if(GDirectXDevice->GetSRVEditorRenderTarget())
		{
			ImGui::Image((void*)GDirectXDevice->GetSRVEditorRenderTarget().Get(), ImVec2(CurrentViewPortSize.x,CurrentViewPortSize.y),ImVec2(0.0f,0.0f),ImVec2(1.0f,1.0f));

		}
		ImVec2 test =  ImGui::GetWindowPos();
		//CurrentViewPortPos = ImGui::GetWindowPos();
		
		/*if(ImGui::BeginTabBar(" "))
		{

			
			if(ImGui::BeginTabItem("ViewPort"))
			{
				

				ImGui::EndTabItem();

			}
			ImGui::EndTabBar();
		}*/
		if(PreviousViewPortSize != CurrentViewPortSize)
		{
			PreviousViewPortSize = CurrentViewPortSize;
			ResizeEditorRenderTargetSize = CurrentViewPortSize;
			bResizeEditorRenderTargetAtEndFrame = true;

		}


#ifdef WITH_EDITOR
		if(bResizeEditorRenderTargetAtEndFrame)
		{
			bResizeEditorRenderTargetAtEndFrame = false;
			GDirectXDevice->ResizeEditorRenderTarget(ResizeEditorRenderTargetSize.x, ResizeEditorRenderTargetSize.y);
		}
#endif


		ImGuizmo::Enable(true);
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
		//ImGuizmo::SetRect(ScreenPos.x, ScreenPos.y, CurrentViewPortSize.x,CurrentViewPortSize.y+YMargin);
		
		ImGuizmo::SetRect(ScreenPos.x,ScreenPos.y,CurrentViewPortSize.x,CurrentViewPortSize.y);
		

		//ImGui::PushClipRect(ImVec2(ScreenPos.x,ScreenPos.y),ImVec2(CurrentViewPortSize.x,CurrentViewPortSize.y+YMargin),false);
		
		DrawImguizmoSelectedActor_RenderThread(CurrentViewPortSize.x/(CurrentViewPortSize.y));
		//ImGui::PopClipRect();
		ImGui::End();
	}
	


	
#endif

}

void FScene::DrawDebugConsole_RenderThread()
{

	{
		ImGui::Begin("Debug Console");

		// EditBox
		char* CurrentText = DebugConsoleSearchText.data();
		ImGui::Text("Search: ");
		ImGui::SameLine();
		if(ImGui::InputText(" ",CurrentText,100))
		{
			DebugConsoleSearchText = CurrentText;
			SearchDebugConsole_RenderThread();
		}

		ImGui::SameLine();
		ImVec2 MousePos = ImGui::GetMousePos();
		ImGui::Text("x = %.2f , y = %.2f", MousePos.x,MousePos.y);

		if(ImGui::BeginListBox(" ", ImVec2(-FLT_MIN, -FLT_MIN)))
		{
			// 디버그 리스트 박스의 맨 아래를 볼 시 맨 아래로 고정
			bool bIsFixListBox = ImGui::GetScrollMaxY() == ImGui::GetScrollY();


			// 검색 중 체크
			bool bWhileSearching = DebugConsoleSearchText.size() != 0;
			if(bWhileSearching)
			{
				for(const DebugText& Text : SearchingDebugConsoleText)
				{
					ImGui::TextColored(DebugText::Color[Text.Level], Text.Text.data());
				}	
			}
			// 검색 아닐 시
			else
			{
				for(const DebugText& Text : DebugConsoleText)
				{
					ImGui::TextColored(DebugText::Color[Text.Level], Text.Text.data());

				}	
			}

			// 리스트 맨 아래였을 시 고정
			if(bIsFixListBox)
			{
				ImGui::SetScrollHereY(1.0f);
			}

			ImGui::EndListBox();

		}


		ImGui::End();
	}
}


void FScene::DrawImguizmoSelectedActor_RenderThread(float AspectRatio)
{
	if(!CurrentSelectedActor)
	{
		return;
	}

	static ImGuizmo::OPERATION CurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE CurrentGizmoMode(ImGuizmo::WORLD);
	if(ImGui::IsKeyPressed(ImGuiKey_Q))
	{
		CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	if(ImGui::IsKeyPressed(ImGuiKey_W))
	{
		CurrentGizmoOperation = ImGuizmo::ROTATE;
	}
	if(ImGui::IsKeyPressed(ImGuiKey_E))
	{
		CurrentGizmoOperation = ImGuizmo::SCALE;
	}

	if(ImGui::IsKeyPressed(ImGuiKey_1))
	{
		CurrentGizmoMode = ImGuizmo::WORLD;
	}
	if(ImGui::IsKeyPressed(ImGuiKey_2))
	{
		CurrentGizmoMode = ImGuizmo::LOCAL;
	}

	const std::shared_ptr<USceneComponent>& CurrentSelectedComponent = SelectActorComponents[CurrentSelectedComponentIndex];
	FTransform ComponentTransform = CurrentSelectedComponent->GetComponentTransform();

	XMMATRIX ComponentMatrix;
	ComponentMatrix = ComponentTransform.ToMatrixWithScale();

	XMMATRIX DeltaMatrixTemp = XMMatrixIdentity();
	float* DeltaMatrix = reinterpret_cast<float*>(&DeltaMatrixTemp);

	XMMATRIX ViewMat = GEngine->Test_DeleteLater_GetViewMatrix();
	XMMATRIX ProjMat = XMMatrixPerspectiveFovLH(0.5*XM_PI, AspectRatio, 1.0f, 1000.0f);

	//ProjMat = XMMatrixPerspectiveFovRH(0.5*XM_PI, 1600.0f/1200.0f, 1.0f, 1000.0f);;
	//ImGuizmo::AllowAxisFlip(true);
	ImGuizmo::Manipulate(reinterpret_cast<float*>(&ViewMat), reinterpret_cast<float*>(&ProjMat),CurrentGizmoOperation,CurrentGizmoMode,reinterpret_cast<float*>(&ComponentMatrix),DeltaMatrix);

	XMFLOAT3 DeltaTranslation;
	XMFLOAT3 DeltaRot;
	XMFLOAT3 DeltaScale;
	ImGuizmo::DecomposeMatrixToComponents(DeltaMatrix, reinterpret_cast<float*>(&DeltaTranslation),reinterpret_cast<float*>(&DeltaRot),reinterpret_cast<float*>(&DeltaScale) );
	if(CurrentGizmoOperation == ImGuizmo::TRANSLATE && XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&DeltaTranslation))) > FLT_EPSILON)
	{
		const auto Lambda = [CurrentSelectedComponent, DeltaTranslation]()
			{
				CurrentSelectedComponent->AddWorldOffset(DeltaTranslation);		
			};
		ENQUEUE_IMGUI_COMMAND(Lambda)

	}

	if ((CurrentGizmoOperation == ImGuizmo::ROTATE) && XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&DeltaRot))) > FLT_EPSILON)
	{
		if(XMVectorGetX(XMVector3Length(XMLoadFloat3(&DeltaRot))) > FLT_EPSILON)
		{
			//CurrentSelectedComponent->AddWorldRotation(DeltaRot);
			const auto Lambda = [CurrentSelectedComponent, DeltaRot]()
				{
					CurrentSelectedComponent->AddWorldRotation(DeltaRot);		
				};
			ENQUEUE_IMGUI_COMMAND(Lambda)
		}

	}

	
}

void FScene::EndRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData)
{
	for(const auto& NewPrimitiveProxy : SceneData->PendingAddSceneProxies)
	{
		SceneData->PrimitiveSceneProxies[NewPrimitiveProxy.first] = NewPrimitiveProxy.second;

	}
	SceneData->PendingAddSceneProxies.clear();
	SceneData->bIsFrameStart = false;
}

// 03.19
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "EditorScene.h"

#ifdef WITH_EDITOR

std::unordered_map<std::string,std::function<void()>> FEditorScene::ImGuiRenderFunctions;
std::unordered_map<std::string,std::function<void()>> FEditorScene::ImGuizmoRenderFunctions;


// 디버깅 콘솔
std::vector<DebugText> FEditorScene::DebugConsoleText;
std::vector<DebugText> FEditorScene::PendingAddDebugConsoleText;
std::vector<DebugText> FEditorScene::SearchingDebugConsoleText;
std::string FEditorScene::DebugConsoleSearchText;

// 월드 아웃라이너
std::vector<std::shared_ptr<AActor>> FEditorScene::WorldOutlinerActors;
std::vector<std::shared_ptr<AActor>> FEditorScene::PendingAddWorldOutlinerActors;
std::shared_ptr<AActor> FEditorScene::CurrentSelectedActor = nullptr;
std::vector<std::shared_ptr<USceneComponent>> FEditorScene::SelectActorComponents;
std::vector<std::string> FEditorScene:: SelectActorComponentNames;

// 에디터 ViewPort
bool FEditorScene::bResizeEditorRenderTargetAtEndFrame=false;
ImVec2 FEditorScene::ResizeEditorRenderTargetSize = {};

int FEditorScene::CurrentSelectedComponentIndex = -1;

FViewMatrices FEditorScene::EditorViewMatrices;


#endif
void FEditorScene::InitLevelData()
{
	FScene::InitLevelData();

	WorldOutlinerActors.clear();
	PendingAddWorldOutlinerActors.clear();
	CurrentSelectedActor = nullptr;
	SelectActorComponents.clear();
	SelectActorComponentNames.clear();
	CurrentSelectedComponentIndex=-1;

	EditorViewMatrices.UpdateViewMatrix(XMFLOAT3(0.0f,0.0f,0.0f), XMQuaternionRotationRollPitchYaw(0.0f,0.0f,0.0f));
}

void FEditorScene::BeginRenderFrame()
{
	FScene::BeginRenderFrame();

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

}

void FEditorScene::SetDrawScenePipeline(const float* ClearColor)
{
	GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(1, GDirectXDevice->GetEditorRenderTargetView().GetAddressOf(), GDirectXDevice->GetDepthStencilView().Get());
	GDirectXDevice->GetDeviceContext()->ClearRenderTargetView(GDirectXDevice->GetEditorRenderTargetView().Get(),ClearColor);
	GDirectXDevice->SetViewPortSize(ResizeEditorRenderTargetSize.x,ResizeEditorRenderTargetSize.y);
}

void FEditorScene::AfterDrawSceneAction(const std::shared_ptr<FScene> SceneData)
{
	FScene::AfterDrawSceneAction(SceneData);

	GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(1, GDirectXDevice->GetRenderTargetView().GetAddressOf(),  GDirectXDevice->GetDepthStencilView().Get());
	GDirectXDevice->GetDeviceContext()->RSSetViewports(1, GDirectXDevice->GetScreenViewport());
	GDirectXDevice->SetDefaultViewPort();

	DrawIMGUI_RenderThread(SceneData);
}

XMMATRIX FEditorScene::GetViewMatrix()
{
	return EditorViewMatrices.GetViewMatrix();
}

XMMATRIX FEditorScene::GetProjectionMatrix()
{
	// TODO: 수정 예정 03/20
	return FScene::GetProjectionMatrix();//ViewMatrices.GetProjectionMatrix();
}

void FEditorScene::DrawIMGUI_RenderThread(std::shared_ptr<FScene> SceneData)
{
	if(bIsGameKill)
	{
		return;
	}
	//========== IMGUI ==========
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();



	static ImFont* RobotoFont = ImGui::GetIO().Fonts->Fonts[0];
	ImGui::PushFont(RobotoFont);




	// ImGui
	for(const auto& Func : ImGuiRenderFunctions)
	{
		Func.second();
	}

	ImGui::PopFont();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::EndFrame();


}

void FEditorScene::DrawDebugConsole_RenderThread()
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

void FEditorScene::DrawImguizmoSelectedActor_RenderThread(float AspectRatio)
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

	// TODO: 03.20 Editor 를 static이 아닌 FEditorScene내에서 작동하도록 변경하기
	XMMATRIX ViewMat = FRenderCommandExecutor::CurrentSceneData->GetViewMatrix();//GEngine->Test_DeleteLater_GetViewMatrix();
	XMMATRIX ProjMat = FRenderCommandExecutor::CurrentSceneData->GetProjectionMatrix();//XMMatrixPerspectiveFovLH(0.5*XM_PI, AspectRatio, 1.0f, 1000.0f);

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

void FEditorScene::DrawImGuiScene_RenderThread()
{
	static ImVec2 PreviousViewPortSize = ImVec2(0.0f,0.0f);
	ImVec2 CurrentViewPortSize{};
	ImVec2 ScreenPos;

	// Actor
	if(ImGui::Begin("Place Actors",nullptr) )
	{
		ImGui::End();
	}


	if(ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoTitleBar|
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse))
	{
		ImVec2 WindowSize = ImGui::GetWindowSize();;
		float XMargin = 5.0f;
		float YMargin = 5.0f;
		float TopMargin = 0.0f;

		ImVec2 ViewPortSize = WindowSize;
		ViewPortSize.x -= XMargin*2;
		ViewPortSize.y = ViewPortSize.y  - YMargin*2;
		CurrentViewPortSize = ViewPortSize;



		ImGui::SetCursorPos(ImVec2(XMargin,YMargin+TopMargin));
		ScreenPos = ImGui::GetCursorScreenPos();
		if(GDirectXDevice->GetSRVEditorRenderTarget())
		{
			ImGui::Image((void*)GDirectXDevice->GetSRVEditorRenderTarget().Get(), ViewPortSize);

		}
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

		// SelectActor ImGuizmo
		ImGuizmo::Enable(true);
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());

		ImGuizmo::SetRect(ScreenPos.x, ScreenPos.y,ViewPortSize.x,ViewPortSize.y);

		ImGui::PushClipRect(ImVec2{0,0},ImVec2{ViewPortSize.x,ViewPortSize.y},true);

		DrawImguizmoSelectedActor_RenderThread(GDirectXDevice->GetAspectRatio());
		ImGui::PopClipRect();

		if(bResizeEditorRenderTargetAtEndFrame)
		{
			bResizeEditorRenderTargetAtEndFrame = false;
			GDirectXDevice->ResizeEditorRenderTarget(ResizeEditorRenderTargetSize.x, ResizeEditorRenderTargetSize.y);
		}

		// 키 입력 처리
		if(ImGui::IsWindowFocused())
		{
			static bool bShowCursor = true;
			static ImVec2 LastMousePos = ImVec2{ -1.0f,-1.0f };
			if(ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			{
				if(!bShowCursor)
				{
					bShowCursor = true;
					MY_LOG("CURSOR", EDebugLogLevel::DLL_Error, "SHOW");
				}
				LastMousePos = ImVec2{ -1.0f,-1.0f };
			}
			if(ImGui::IsMouseDown(ImGuiMouseButton_Right))
			{
				if (bShowCursor) 
				{
					bShowCursor = false;
					MY_LOG("CURSOR", EDebugLogLevel::DLL_Error, "HIDDEN");
				}
				// 커서 델타 구하기
				ImVec2 MouseDelta{ 0.0f,0.0f };
				if (LastMousePos.x >= 0.0f) 
				{
					MouseDelta = ImGui::GetMousePos() - LastMousePos;
				}
				LastMousePos = ImGui::GetMousePos();


				static float MoveSpeed = 0.05f;
				XMFLOAT3 MoveDelta = XMFLOAT3(0.0f,0.0f,0.0f);
				if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_W))
				{
					MoveDelta.z += MoveSpeed;
				}
				if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_S))
				{
					MoveDelta.z -= MoveSpeed;
				}
				if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_A))
				{
					MoveDelta.x-=MoveSpeed;
				}
				if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_D))
				{
					MoveDelta.x +=MoveSpeed;
				}
				if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_E))
				{
					MoveDelta.y +=MoveSpeed;
				}
				if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Q))
				{
					MoveDelta.y -=MoveSpeed;
				}

				XMFLOAT2 XMMouseDelta{ MouseDelta.x, MouseDelta.y };
				if(XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&MoveDelta)))>FLT_EPSILON
					|| XMVectorGetX(XMVector2LengthEst(XMLoadFloat2(&XMMouseDelta))) > FLT_EPSILON)
				{
					EditorCameraMove(MoveDelta, XMMouseDelta);
				}
				
				
			}
		}


		ImGui::End();
	}
}

void FEditorScene::EditorCameraMove(XMFLOAT3 MoveDelta, XMFLOAT2 MouseDelta)
{
	XMFLOAT3 CurrentLocation = EditorViewMatrices.GetViewOrigin();
	XMVECTOR CurrentCameraRotQuat = EditorViewMatrices.GetCameraRotQuat();
	float DeltaSpeed = 90.0f / 50.0f; // 윈도우 좌표 50 이동 시 90도 회전하도록
	// MouseDelta.x -> y축(yaw) 회전 // MouseDelta.y -> x축(pitch) 회전
	XMVECTOR NewCameraRotQuat = XMQuaternionMultiply(CurrentCameraRotQuat, XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(MouseDelta.y * DeltaSpeed),
		XMConvertToRadians(MouseDelta.x * DeltaSpeed),
		XMConvertToRadians(0.0f)));


	// MoveDelta.x -> 좌우 이동
	// MoveDelta.y -> 위 아래 이동
	// MoveDelta.z -> 앞뒤 이동
	XMVECTOR ForwardVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), NewCameraRotQuat));
	XMVECTOR UpVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), NewCameraRotQuat));
	XMVECTOR RightVector = XMVector3Normalize(XMVector3Cross(UpVector, ForwardVector));
	
	XMVECTOR CurLocVec = XMLoadFloat3(&CurrentLocation);
	XMVECTOR MoveDeltaVec = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	// 앞뒤
	MoveDeltaVec = XMVectorAdd(MoveDeltaVec, XMVectorScale(ForwardVector, MoveDelta.z));
	// 좌우
	MoveDeltaVec = XMVectorAdd(MoveDeltaVec, XMVectorScale(RightVector, MoveDelta.x));
	// 위아래
	MoveDeltaVec = XMVectorAdd(MoveDeltaVec, XMVectorScale(UpVector, MoveDelta.y));

	XMFLOAT3 NewLocation{0.0f,0.0f,0.0f};
	XMStoreFloat3(&NewLocation, XMVectorAdd(CurLocVec, MoveDeltaVec));

	EditorViewMatrices.UpdateViewMatrix(NewLocation, NewCameraRotQuat);
}

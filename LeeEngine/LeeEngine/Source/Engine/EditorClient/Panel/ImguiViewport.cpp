#include "ImguiViewport.h"

#include "ImguiActorDetail.h"
#include "ImguiWorldOutliner.h"
#include "Engine/DirectX/Device.h"

FImguiLevelViewport::FImguiLevelViewport(FScene* SceneData)
	:FImguiPanel(SceneData)
{
	ActorDetailPanel = std::make_unique<FImguiActorDetail>(SceneData, this);
	WorldOutlinerPanel = std::make_unique<FImguiWorldOutliner>(SceneData, this);
}

void FImguiLevelViewport::Draw()
{

	// 렌더링 시작

	static ImVec2 PreviousViewPortSize = ImVec2(0.0f,0.0f);
	ImVec2 CurrentViewPortSize{};
	ImVec2 ScreenPos;

	// Actor
	if(ImGui::Begin("Place Actors",nullptr) )
	{
		ImGui::End();
	}

	// World Outliner
	WorldOutlinerPanel->Draw();
	// Actor Detail
	ActorDetailPanel->Draw();


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

		DrawImguizmoSelectedActor(GDirectXDevice->GetAspectRatio());
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
				ImGuiIO& io = ImGui::GetIO();
				if(io.MouseWheel > 0.0f)
				{
					MoveSpeed = min(MoveSpeed + 0.05f, 100.0f);
				}
				else if(io.MouseWheel < -0.0f)
				{
					MoveSpeed = max(MoveSpeed - 0.05f, 0.05f);
				}

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

void FImguiLevelViewport::ExecuteCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData)
{
	if(CommandData->IsOfType(FImguiLevelViewportCommandData::GetClassID()))
	{
		FImguiLevelViewportCommandData* Data = static_cast<FImguiLevelViewportCommandData*>(CommandData.get());
		switch (Data->CommandType)
		{
		case ELevelViewportCommandType::LVCT_AddActorToWorldOutliner:
			WorldOutlinerPanel->PendingAddWorldOutlinerActor(Data->NewPendingAddActor);
			break;

		case ELevelViewportCommandType::LVCT_SetViewportSizeToEditorViewportSize:
			GDirectXDevice->SetViewPortSize(ResizeEditorRenderTargetSize.x, ResizeEditorRenderTargetSize.y);	
			break;

		case ELevelViewportCommandType::LVCT_GetEditorViewMatrices:
			*Data->ViewMatrices = EditorViewMatrices;
			break;

		case ELevelViewportCommandType::LVCT_ClearCurrentLevelData:
			InitLevelData();
			break;

		default:
			MY_LOG("LevelViewPort", EDebugLogLevel::DLL_Error, "Wrong Command Type");
			break;
		}
		
	}
}

void FImguiLevelViewport::InitLevelData()
{
	WorldOutlinerPanel->InitLevelData();
	ActorDetailPanel->InitLevelData();

	EditorViewMatrices.UpdateViewMatrix(XMFLOAT3(0.0f,0.0f,0.0f), XMQuaternionRotationRollPitchYaw(0.0f,0.0f,0.0f));
}

void FImguiLevelViewport::SelectActorFromWorldOutliner(const std::shared_ptr<AActor>& NewSelectedActor)
{
	if(ActorDetailPanel)
	{
		ActorDetailPanel->SelectActorFromWorldOutliner(NewSelectedActor);
	}
	
}


void FImguiLevelViewport::DrawImguizmoSelectedActor(float AspectRatio)
{
	const std::shared_ptr<USceneComponent>& CurrentSelectedComponent = ActorDetailPanel->GetCurrentSelectedComponent();
	if(!ActorDetailPanel || !CurrentSelectedComponent)
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

	FTransform ComponentTransform = CurrentSelectedComponent->GetComponentTransform();

	XMMATRIX ComponentMatrix;
	ComponentMatrix = ComponentTransform.ToMatrixWithScale();

	XMMATRIX DeltaMatrixTemp = XMMatrixIdentity();
	float* DeltaMatrix = reinterpret_cast<float*>(&DeltaMatrixTemp);

	// TODO: 03.20 Editor 를 static이 아닌 FEditorScene내에서 작동하도록 변경하기
	XMMATRIX ViewMat = EditorViewMatrices.GetViewMatrix();//GEngine->Test_DeleteLater_GetViewMatrix();
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

void FImguiLevelViewport::EditorCameraMove(XMFLOAT3 MoveDelta, XMFLOAT2 MouseDelta)
{
	XMFLOAT3 CurrentLocation = EditorViewMatrices.GetViewOrigin();
	XMVECTOR CurrentCameraRotQuat = EditorViewMatrices.GetCameraRotQuat();
	float DeltaSpeed = 30.0f / 50.0f; // 윈도우 좌표 50 이동 시 60도 회전하도록
	// MouseDelta.x -> y축(yaw) 회전 // MouseDelta.y -> x축(pitch) 회전

	XMVECTOR ForwardVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), CurrentCameraRotQuat));
	//XMVECTOR UpVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), CurrentCameraRotQuat));
	XMVECTOR UpVector = XMVectorSet(0.0f,1.0f,0.0f,0.0f);
	XMVECTOR RightVector = XMVector3Normalize(XMVector3Cross(UpVector, ForwardVector));

	XMVECTOR LocalPitchQuat = XMQuaternionRotationAxis(RightVector,
		XMConvertToRadians(MouseDelta.y*DeltaSpeed));
	XMVECTOR LocalYawQuat = XMQuaternionRotationAxis(UpVector,
		XMConvertToRadians(MouseDelta.x*DeltaSpeed));

	XMVECTOR NewCameraRotQuat = XMQuaternionMultiply(CurrentCameraRotQuat,LocalPitchQuat);
	NewCameraRotQuat = XMQuaternionNormalize(XMQuaternionMultiply(NewCameraRotQuat,LocalYawQuat));
	//XMVECTOR NewCameraRotQuat = XMQuaternionMultiply(CurrentCameraRotQuat, XMQuaternionRotationRollPitchYaw(
	//XMConvertToRadians(MouseDelta.y * DeltaSpeed),
	//XMConvertToRadians(MouseDelta.x * DeltaSpeed),
	//XMConvertToRadians(0.0f)));


	// MoveDelta.x -> 좌우 이동
	// MoveDelta.y -> 위 아래 이동
	// MoveDelta.z -> 앞뒤 이동
	ForwardVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), NewCameraRotQuat));
	//UpVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), NewCameraRotQuat));
	RightVector = XMVector3Normalize(XMVector3Cross(UpVector, ForwardVector));
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

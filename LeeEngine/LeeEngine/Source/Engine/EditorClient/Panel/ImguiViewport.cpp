#include "CoreMinimal.h"
#ifdef WITH_EDITOR
#include "Engine/UEditorEngine.h"
#include "Engine/RenderCore/ImGUIActionTask.h"
#include "ImguiViewport.h"

#include "ImguiActorDetail.h"
#include "ImguiWorldOutliner.h"

ImVec2 FImguiLevelViewport::PreviousViewPortSize = ImVec2(0.0f, 0.0f);
XMFLOAT2 FImguiLevelViewport::LevelViewportPos = XMFLOAT2(0.0f,0.0f);


FImguiLevelViewport::FImguiLevelViewport(FScene* SceneData)
	: FImguiPanel(SceneData)
{
	ActorDetailPanel   = std::make_unique<FImguiActorDetail>(SceneData, this);
	WorldOutlinerPanel = std::make_unique<FImguiWorldOutliner>(SceneData, this);
}

void FImguiLevelViewport::Draw()
{
	// 렌더링 시작
	ImVec2 CurrentViewPortSize{};

	// Actor
	if (ImGui::Begin("Place Actors", nullptr))
	{
		ImGui::Text("ERT");
		if (const std::shared_ptr<UTexture>& RenderTexture = GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Editor_Main)->GetRenderTargetTexture(0))
		{
			ImGui::Image(reinterpret_cast<ImTextureID>(RenderTexture->GetSRV().Get()), {300,200});
		}

		ImGui::Text("ERT_HDR");
		if (const std::shared_ptr<UTexture>& RenderTexture = GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Editor_HDR)->GetRenderTargetTexture(0))
		{
			ImGui::Image(reinterpret_cast<ImTextureID>(RenderTexture->GetSRV().Get()), {300,200});
		}

		ImGui::Text("PBRTargetTex");
		if (const std::shared_ptr<UTexture>& RenderTexture = GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Deferred)->GetRenderTargetTexture(4))
		{
			ImGui::Image(reinterpret_cast<ImTextureID>(RenderTexture->GetSRV().Get()), {300,200});
		}

		ImGui::Text("ColorTargetTex");
		if (const std::shared_ptr<UTexture>& RenderTexture =  GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Deferred)->GetRenderTargetTexture(0))
		{
			ImGui::Image(reinterpret_cast<ImTextureID>(RenderTexture->GetSRV().Get()), {300,200});
		}

		ImGui::Text("NormalTargetTex");
		if (const std::shared_ptr<UTexture>& RenderTexture =  GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Deferred)->GetRenderTargetTexture(1))
		{
			ImGui::Image(reinterpret_cast<ImTextureID>(RenderTexture->GetSRV().Get()), {300,200});
		}

		ImGui::Text("PositionTargetTex");
		if (const std::shared_ptr<UTexture>& RenderTexture = GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Deferred)->GetRenderTargetTexture(2))
		{
			ImGui::Image(reinterpret_cast<ImTextureID>(RenderTexture->GetSRV().Get()), {300,200});
		}

		ImGui::Text("EmissiveTargetTex");
		if (const std::shared_ptr<UTexture>& RenderTexture = GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Deferred)->GetRenderTargetTexture(3))
		{
			ImGui::Image(reinterpret_cast<ImTextureID>(RenderTexture->GetSRV().Get()), {300,200});
		}

		ImGui::End();
	}

	// World Outliner
	WorldOutlinerPanel->Draw();
	// Actor Detail
	ActorDetailPanel->Draw();

	if (ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		ImVec2 WindowSize = ImGui::GetWindowSize();
		float  XMargin    = 5.0f;
		float  YMargin    = 5.0f;
		float  TopMargin  = 0.0f;

		

		ImVec2 ViewPortSize = WindowSize;
		ViewPortSize.x -= XMargin * 2;
		ViewPortSize.y      = ViewPortSize.y - YMargin * 2;
		CurrentViewPortSize = ViewPortSize;

		ImGui::SetCursorPos(ImVec2(XMargin, YMargin + TopMargin));
		ImVec2 ScreenPos = ImGui::GetCursorScreenPos();
		LevelViewportPos = {ScreenPos.x, ScreenPos.y};
		
		if (PreviousViewPortSize != CurrentViewPortSize )
		{
			PreviousViewPortSize                = CurrentViewPortSize;
			ResizeEditorRenderTargetSize        = CurrentViewPortSize;
			bResizeEditorRenderTargetAtEndFrame = true;
		}

		if (const std::shared_ptr<UTexture>& RenderTexture = GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Editor_Main)->GetRenderTargetTexture(0))
		{
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd) {
				GDirectXDevice->SetBSStateAlways(EBlendStateType::BST_Default);
				}, nullptr);

			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 size = ViewPortSize;
			ImVec2 pos_min = pos; 
			ImVec2 pos_max = ImVec2(pos.x + size.x, pos.y + size.y);
			draw_list->AddImage(reinterpret_cast<ImTextureID>(RenderTexture->GetSRV().Get()), pos_min, pos_max);

			// 콜백 끝나고 원래 상태로 복귀
			draw_list->AddCallback(ImDrawCallback_ResetRenderState, nullptr);


			//ImGui::Image(reinterpret_cast<ImTextureID>(RenderTexture->GetSRV().Get()), ViewPortSize);	
		}	
		

		// SelectActor ImGuizmo
		ImGuizmo::Enable(true);
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
		ImGuizmo::SetRect(ScreenPos.x, ScreenPos.y, ViewPortSize.x, ViewPortSize.y);

		ImGui::PushClipRect(ImVec2{0, 0}, ImVec2{ViewPortSize.x, ViewPortSize.y}, true);

		DrawImguizmoSelectedActor(GDirectXDevice->GetAspectRatio());
		ImGui::PopClipRect();

		// 키 입력 처리
		if (ImGui::IsWindowFocused())
		{
			static bool bShowCursor  = true;
			static auto LastMousePos = ImVec2{-1.0f, -1.0f};
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			{
				if (!bShowCursor)
				{
					bShowCursor = true;
					MY_LOG("CURSOR", EDebugLogLevel::DLL_Error, "SHOW");
				}
				LastMousePos = ImVec2{-1.0f, -1.0f};
			}
			if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
			{
				if (bShowCursor)
				{
					bShowCursor = false;
					MY_LOG("CURSOR", EDebugLogLevel::DLL_Error, "HIDDEN");
				}
				// 커서 델타 구하기
				ImVec2 MouseDelta{0.0f, 0.0f};
				if (LastMousePos.x >= 0.0f)
				{
					
					MouseDelta = ImGui::GetMousePos() - LastMousePos;
				}
				LastMousePos = ImGui::GetMousePos();

				static float MoveSpeed = 0.05f;
				ImGuiIO&     io        = ImGui::GetIO();
				if (io.MouseWheel > 0.0f)
				{
					MoveSpeed = std::min(MoveSpeed + 0.05f, 10000.0f);
				}
				else if (io.MouseWheel < -0.0f)
				{
					MoveSpeed = max(MoveSpeed - 0.05f, 0.05f);
				}

				auto MoveDelta = XMFLOAT3(0.0f, 0.0f, 0.0f);
				if (ImGui::IsKeyDown(ImGuiKey_W))
				{
					MoveDelta.z += MoveSpeed;
				}
				if (ImGui::IsKeyDown(ImGuiKey_S))
				{
					MoveDelta.z -= MoveSpeed;
				}
				if (ImGui::IsKeyDown(ImGuiKey_A))
				{
					MoveDelta.x -= MoveSpeed;
				}
				if (ImGui::IsKeyDown(ImGuiKey_D))
				{
					MoveDelta.x += MoveSpeed;
				}

				if (ImGui::IsKeyDown(ImGuiKey_E))
				{
					MoveDelta.y += MoveSpeed;
				}
				if (ImGui::IsKeyDown(ImGuiKey_Q))
				{
					MoveDelta.y -= MoveSpeed;
				}

				XMFLOAT2 XMMouseDelta{MouseDelta.x, MouseDelta.y};
				if (XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&MoveDelta))) > FLT_EPSILON || XMVectorGetX(XMVector2LengthEst(XMLoadFloat2(&XMMouseDelta))) > FLT_EPSILON)
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
	if (CommandData->IsOfType(FImguiLevelViewportCommandData::GetClassID()))
	{
		auto Data = static_cast<FImguiLevelViewportCommandData*>(CommandData.get());
		switch (Data->CommandType)
		{
		case ELevelViewportCommandType::LVCT_AddActorToWorldOutliner:
			WorldOutlinerPanel->PendingAddWorldOutlinerActor(Data->NewPendingActor);
			break;
		case ELevelViewportCommandType::LVCT_RemoveActorWorldOutliner:
			WorldOutlinerPanel->PendingKillWorldOutlinerActor(Data->NewPendingActor);
			break;

		case ELevelViewportCommandType::LVCT_SetViewportSizeToEditorViewportSize:
			GDirectXDevice->SetViewPortSize(ResizeEditorRenderTargetSize.x, ResizeEditorRenderTargetSize.y);
			Data->ViewMatrices->ProjectionMatrix = XMMatrixPerspectiveFovLH(0.5*XM_PI, ResizeEditorRenderTargetSize.x/ResizeEditorRenderTargetSize.y,1.0f, 200000.0f);
			break;

		case ELevelViewportCommandType::LVCT_GetEditorViewMatrices:
			Data->ViewMatrices->ViewMatrix = EditorViewMatrices.ViewMatrix;
			break;

		case ELevelViewportCommandType::LVCT_ChangeLevelInitialize:
			PreviousViewPortSize = ImVec2(0.0f, 0.0f);
			break;

		default: MY_LOG("LevelViewPort", EDebugLogLevel::DLL_Error, "Wrong Command Type");
			break;
		}
	}
}

void FImguiLevelViewport::SelectActorFromWorldOutliner(const std::shared_ptr<AActor>& NewSelectedActor)
{
	if (ActorDetailPanel)
	{
		ActorDetailPanel->SelectActorFromWorldOutliner(NewSelectedActor);
	}
}

void FImguiLevelViewport::DrawImguizmoSelectedActor(float AspectRatio)
{
	const std::shared_ptr<USceneComponent>& CurrentSelectedComponent = ActorDetailPanel->GetCurrentSelectedComponent();
	if (!ActorDetailPanel || !CurrentSelectedComponent)
	{
		return;
	}

	static ImGuizmo::OPERATION CurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE      CurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::IsKeyPressed(ImGuiKey_Q))
	{
		CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	if (ImGui::IsKeyPressed(ImGuiKey_W))
	{
		CurrentGizmoOperation = ImGuizmo::ROTATE;
	}
	if (ImGui::IsKeyPressed(ImGuiKey_E))
	{
		CurrentGizmoOperation = ImGuizmo::SCALE;
	}

	if (ImGui::IsKeyPressed(ImGuiKey_1))
	{
		CurrentGizmoMode = ImGuizmo::WORLD;
	}
	if (ImGui::IsKeyPressed(ImGuiKey_2))
	{
		CurrentGizmoMode = ImGuizmo::LOCAL;
	}

	FTransform ComponentTransform = CurrentSelectedComponent->GetComponentTransform();

	XMMATRIX ComponentMatrix;
	ComponentMatrix = ComponentTransform.ToMatrixWithScale();

	XMMATRIX DeltaMatrixTemp = XMMatrixIdentity();
	auto     DeltaMatrix     = reinterpret_cast<float*>(&DeltaMatrixTemp);

	// TODO: 03.20 Editor 를 static이 아닌 FEditorScene내에서 작동하도록 변경하기
	XMMATRIX ViewMat = EditorViewMatrices.GetViewMatrix(); //GEngine->Test_DeleteLater_GetViewMatrix();
	XMMATRIX ProjMat = FRenderCommandExecutor::CurrentSceneData->GetProjectionMatrix();

	//ProjMat = XMMatrixPerspectiveFovRH(0.5*XM_PI, 1600.0f/1200.0f, 1.0f, 1000.0f);;
	//ImGuizmo::AllowAxisFlip(true);
	Manipulate(reinterpret_cast<float*>(&ViewMat), reinterpret_cast<float*>(&ProjMat), CurrentGizmoOperation, CurrentGizmoMode, reinterpret_cast<float*>(&ComponentMatrix), DeltaMatrix);

	XMFLOAT3 DeltaTranslation;
	XMFLOAT3 DeltaRot;
	XMFLOAT3 DeltaScale;
	ImGuizmo::DecomposeMatrixToComponents(DeltaMatrix, reinterpret_cast<float*>(&DeltaTranslation), reinterpret_cast<float*>(&DeltaRot), reinterpret_cast<float*>(&DeltaScale));

	bool bHasChange = false;
	if (CurrentGizmoOperation == ImGuizmo::TRANSLATE && XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&DeltaTranslation))) > FLT_EPSILON)
	{
		const auto Lambda = [CurrentSelectedComponent, DeltaTranslation]()
		{
			CurrentSelectedComponent->AddWorldOffset(DeltaTranslation);
		};
		ENQUEUE_IMGUI_COMMAND(Lambda);
		bHasChange = true;
	}

	if ((CurrentGizmoOperation == ImGuizmo::ROTATE) && XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&DeltaRot))) > FLT_EPSILON)
	{
		if (XMVectorGetX(XMVector3Length(XMLoadFloat3(&DeltaRot))) > FLT_EPSILON)
		{
			//CurrentSelectedComponent->AddWorldRotation(DeltaRot);
			const auto Lambda = [CurrentSelectedComponent, DeltaRot]()
			{
				CurrentSelectedComponent->AddWorldRotation(DeltaRot);
			};
			ENQUEUE_IMGUI_COMMAND(Lambda)
		}
		bHasChange = true;
	}

	if (bHasChange)
	{
		GEditorEngine->EditorModify(EEditorModificationType::EMT_Level,
									[](bool bIsFirstChange)
									{
										GEditorEngine->DrawEngineTitleBar();
									});
	}
}

void FImguiLevelViewport::EditorCameraMove(XMFLOAT3 MoveDelta, XMFLOAT2 MouseDelta)
{
	XMFLOAT3 CurrentLocation      = EditorViewMatrices.GetViewOrigin();
	XMVECTOR CurrentCameraRotQuat = EditorViewMatrices.GetCameraRotQuat();
	float    DeltaSpeed           = 30.0f / 50.0f; // 윈도우 좌표 50 이동 시 60도 회전하도록
	// MouseDelta.x -> y축(yaw) 회전 // MouseDelta.y -> x축(pitch) 회전

	XMVECTOR ForwardVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), CurrentCameraRotQuat));
	//XMVECTOR UpVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), CurrentCameraRotQuat));
	XMVECTOR UpVector    = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR RightVector = XMVector3Normalize(XMVector3Cross(UpVector, ForwardVector));

	XMVECTOR LocalPitchQuat = XMQuaternionRotationAxis(RightVector, XMConvertToRadians(MouseDelta.y * DeltaSpeed));
	XMVECTOR LocalYawQuat   = XMQuaternionRotationAxis(UpVector, XMConvertToRadians(MouseDelta.x * DeltaSpeed));

	XMVECTOR NewCameraRotQuat = XMQuaternionMultiply(CurrentCameraRotQuat, LocalPitchQuat);
	NewCameraRotQuat          = XMQuaternionNormalize(XMQuaternionMultiply(NewCameraRotQuat, LocalYawQuat));
	//XMVECTOR NewCameraRotQuat = XMQuaternionMultiply(CurrentCameraRotQuat, XMQuaternionRotationRollPitchYaw(
	//XMConvertToRadians(MouseDelta.y * DeltaSpeed),
	//XMConvertToRadians(MouseDelta.x * DeltaSpeed),
	//XMConvertToRadians(0.0f)));

	// MoveDelta.x -> 좌우 이동
	// MoveDelta.y -> 위 아래 이동
	// MoveDelta.z -> 앞뒤 이동
	ForwardVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), NewCameraRotQuat));
	//UpVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), NewCameraRotQuat));
	RightVector           = XMVector3Normalize(XMVector3Cross(UpVector, ForwardVector));
	XMVECTOR CurLocVec    = XMLoadFloat3(&CurrentLocation);
	XMVECTOR MoveDeltaVec = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	// 앞뒤
	MoveDeltaVec = XMVectorAdd(MoveDeltaVec, XMVectorScale(ForwardVector, MoveDelta.z));
	// 좌우
	MoveDeltaVec = XMVectorAdd(MoveDeltaVec, XMVectorScale(RightVector, MoveDelta.x));
	// 위아래
	MoveDeltaVec = XMVectorAdd(MoveDeltaVec, XMVectorScale(UpVector, MoveDelta.y));

	XMFLOAT3 NewLocation{0.0f, 0.0f, 0.0f};
	XMStoreFloat3(&NewLocation, XMVectorAdd(CurLocVec, MoveDeltaVec));

	EditorViewMatrices.UpdateViewMatrix(NewLocation, NewCameraRotQuat);
}

#endif

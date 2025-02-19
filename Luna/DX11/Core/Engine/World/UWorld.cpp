// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UWorld.h"

#include "imgui.h"
#include "Engine/UEngine.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/GameFramework/AActor.h"

UWorld::UWorld()
{
	
}

UWorld::~UWorld()
{
}


void UWorld::Init()
{
	UObject::Init();

	if(!PersistentLevel)
	{
		PersistentLevel = std::make_shared<ULevel>(shared_from_this());
	}

	GEngine->AddImGuiRenderFunction(std::bind(&UWorld::ImguiRender_WorldOutliner, this));
	GEngine->AddImGuiRenderFunction(std::bind(&UWorld::ImGuiRender_ActorDetail, this));

	GEngine->AddImGuizmoRenderFunction(std::bind(&UWorld::ImGuizmoRender_SelectComponentGizmo, this));
}

void UWorld::PostLoad()
{
	UObject::PostLoad();
}

void UWorld::TickWorld(float DeltaSeconds)
{
	std::cout<< "tick world" << std::endl;
	if(PersistentLevel)
	{
		PersistentLevel->TickLevel(DeltaSeconds);
	}
}

void UWorld::Tick()
{
}

void UWorld::TestDrawWorld()
{
	if(PersistentLevel)
	{
		PersistentLevel->TestDrawLevel();
	}
}

void UWorld::ImguiRender_WorldOutliner()
{
	if(PersistentLevel)
	{
		static int current_item = -1; 
		
		const std::vector<std::shared_ptr<AActor>>& LevelActors = PersistentLevel->GetLevelActors();

		
		ImGui::Begin("World Outliner", nullptr);

		if (ImGui::BeginListBox(" ", ImVec2(-FLT_MIN,-FLT_MIN))) {
			int ActorCount = LevelActors.size();
			for (int i = 0; i < ActorCount; i++) {
				const bool is_selected = (current_item == i);
				if (ImGui::Selectable(LevelActors[i]->GetName().c_str(), is_selected)) {
					current_item = i;
					ImGuiAction_SelectActor(LevelActors[i]);
				}

				// 선택된 항목에 대한 포커스 처리
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndListBox();
		}

		ImGui::End();

	}
}

void UWorld::ImGuiAction_SelectActor(const std::shared_ptr<AActor>& NewSelectActor)
{
	SelectActorComponents.clear();
	SelectActorComponentNames.clear();
	CurrentSelectedComponentIndex = 0;
	CurrentSelectedActor = NewSelectActor;
	ImGuiAction_FindActorComponentsAndNames(CurrentSelectedActor->GetRootComponent(), 0);
}

void UWorld::ImGuiAction_FindActorComponentsAndNames(const std::shared_ptr<USceneComponent>& TargetComponent, int CurrentHierarchyDepth)
{
	if(!CurrentSelectedActor)
	{
		return;
	}

	std::string HierarchyTabString;
	HierarchyTabString.reserve(2*CurrentHierarchyDepth);
	for(int i = 0; i < CurrentHierarchyDepth; ++i)
	{
		HierarchyTabString += "  ";
	}
	
	std::string TargetComponentName = HierarchyTabString + TargetComponent->GetName();
	
	SelectActorComponents.push_back(TargetComponent);
	SelectActorComponentNames.push_back(TargetComponentName);
	
	const std::vector<std::shared_ptr<USceneComponent>>& TargetComponentChildren = TargetComponent->GetAttachChildren();
	for(const auto& ChildComponent : TargetComponentChildren)
	{
		ImGuiAction_FindActorComponentsAndNames(ChildComponent, CurrentHierarchyDepth+1);
	}
}

void UWorld::ImGuiRender_ActorDetail()
{
	
	ImGui::Begin("Detail");
	if(PersistentLevel && CurrentSelectedActor)
	{
		// 컴퍼넌트들 렌더링
		{
			if(ImGui::BeginListBox(" ", ImVec2(-FLT_MIN,200.0f)))
			{
				int ComponentCount = SelectActorComponentNames.size();
				for (int i = 0; i < ComponentCount; i++) {
					const bool is_selected = (CurrentSelectedComponentIndex == i);
					if (ImGui::Selectable(SelectActorComponentNames[i].data(), is_selected)) {
						CurrentSelectedComponentIndex = i;
						// TODO : Select Component Action
					}
				
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndListBox();
			}
		}
		
	}
	else
	{
		ImGui::Text("No Select");
	}
	ImGui::End();
}

void UWorld::ImGuizmoRender_SelectComponentGizmo()
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

	const std::shared_ptr<USceneComponent>& CurrentSelectedComponent = SelectActorComponents[CurrentSelectedComponentIndex];
	FTransform ComponentTransform = CurrentSelectedComponent->GetComponentTransform();

	XMFLOAT3 RotationEuler = ComponentTransform.GetEulerRotation();
	XMMATRIX ComponentMatrix;// = ComponentTransform.ToMatrixWithScale();
	ImGuizmo::RecomposeMatrixFromComponents(reinterpret_cast<float*>(&ComponentTransform.Translation), reinterpret_cast<float*>(&RotationEuler), reinterpret_cast<float*>(&ComponentTransform.Scale3D), reinterpret_cast<float*>(&ComponentMatrix));
	XMMATRIX DeltaMatrixTemp = XMMatrixIdentity();
	float* DeltaMatrix = reinterpret_cast<float*>(&DeltaMatrixTemp);

	XMMATRIX ViewMat = GEngine->Test_DeleteLater_GetViewMatrix();
	XMMATRIX ProjMat = GEngine->Test_DeleteLater_GetProjectionMatrix();

	ImGuizmo::Manipulate(reinterpret_cast<float*>(&ViewMat), reinterpret_cast<float*>(&ProjMat),CurrentGizmoOperation,CurrentGizmoMode,reinterpret_cast<float*>(&ComponentMatrix),DeltaMatrix);

	XMFLOAT3 DeltaTranslation;
	XMFLOAT3 DeltaRot;
	XMFLOAT3 DeltaScale;
	ImGuizmo::DecomposeMatrixToComponents(DeltaMatrix, reinterpret_cast<float*>(&DeltaTranslation),reinterpret_cast<float*>(&DeltaRot),reinterpret_cast<float*>(&DeltaScale) );
	CurrentSelectedComponent->AddWorldOffset(DeltaTranslation);

	// TODO: 02.19 Scale, Rotation 모두 적용하기

	
}

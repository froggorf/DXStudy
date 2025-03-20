#include "ATestCube.h"

#include "Engine/UEditorEngine.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"


ATestCube::ATestCube()
{
	GetRootComponent()->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	TestCubeStaticMeshComp = std::make_shared<UStaticMeshComponent>();
	TestCubeStaticMeshComp->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_SkySphere"));
	TestCubeStaticMeshComp->SetupAttachment(GetRootComponent());
	
	TestCubeStaticMeshComp->SetRelativeLocation(XMFLOAT3(0.0f,0.0f,0.0f));
	//TestCubeStaticMeshComp->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	TestCubeStaticMeshComp->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	TestCubeStaticMeshComp->SetRelativeScale3D(XMFLOAT3(0.2f,0.2f,0.2f));


	Rename("ATestCube" + std::to_string(ActorID));
}

void ATestCube::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	
}

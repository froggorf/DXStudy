#include "ATestCube.h"

#include "Engine/UEditorEngine.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"


ATestCube::ATestCube()
{
	//GetRootComponent()->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	//TestCubeStaticMeshComp = std::make_shared<UStaticMeshComponent>();
	//TestCubeStaticMeshComp->SetStaticMesh(UStaticMesh::GetStaticMesh/("SM_SkySphere"));
	//TestCubeStaticMeshComp->SetupAttachment(GetRootComponent());
	//
	//TestCubeStaticMeshComp->SetRelativeLocation(XMFLOAT3(0.0f,0.0f,0.0f));
	////TestCubeStaticMeshComp->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	//TestCubeStaticMeshComp->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	//TestCubeStaticMeshComp->SetRelativeScale3D(XMFLOAT3(0.2f,0.2f,0.2f));
	//
	//TestCube2 = std::make_shared<UStaticMeshComponent>();
	//TestCube2->SetupAttachment(GetRootComponent());
	//TestCube2->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Cube"));
	//TestCube2->SetRelativeScale3D(XMFLOAT3(100.0f,1.0f,100.0f));
	

	TestCube3 = std::make_shared<UStaticMeshComponent>();
	TestCube3->SetupAttachment(GetRootComponent());
	TestCube3->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_TranslucentCube"));
	TestCube3->SetRelativeScale3D(XMFLOAT3{30.0f,30.0f,30.0f});

	Rename("ATestCube" + std::to_string(ActorID));
}

void ATestCube::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	//SetActorRotation(XMFLOAT4(0.0f,0.0f,0.0f,1.0f));

}

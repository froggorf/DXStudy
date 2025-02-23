#include "ATestCube.h"

#include "Engine/Components/USceneComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"


ATestCube::ATestCube()
{
	TestCubeStaticMeshComp = std::make_shared<UStaticMeshComponent>();
	TestCubeStaticMeshComp->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Racco"));
	TestCubeStaticMeshComp->SetupAttachment(GetRootComponent());

	TestCubeStaticMeshComp->SetRelativeLocation(XMFLOAT3(5.0f,0.0f,0.0f));
	//TestCubeStaticMeshComp->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	TestCubeStaticMeshComp->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	TestCubeStaticMeshComp->SetRelativeScale3D(XMFLOAT3(0.02f,0.02f,0.02f));

	GetRootComponent()->SetRelativeRotation(XMFLOAT3(45.0f,0.0f,0.0f));

}

void ATestCube::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
	auto p = GetRootComponent()->GetAttachChildren();

	p[0]->AddWorldRotation(XMFLOAT3(0.0f,10.0f,0.0f));
	
	//GetRootComponent()->AddWorldRotation(XMFLOAT3(0.0f,5.0f,0.0f));

}

#include "ATestActor2.h"

#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"

ATestActor2::ATestActor2() 
{
	TestSMComp = std::make_shared<UStaticMeshComponent>();
	TestSMComp->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Cube"));
	TestSMComp->SetupAttachment(TestCubeStaticMeshComp);
	TestSMComp->SetRelativeLocation(XMFLOAT3(100.0f,10.0f,0.0f));
	TestSMComp->SetRelativeScale3D(XMFLOAT3(10.0f,10.0f,10.0f));

	TestSMComp2= std::make_shared<UStaticMeshComponent>();
	TestSMComp2->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Sphere"));
	TestSMComp2->SetupAttachment(GetRootComponent());
	TestSMComp2->SetRelativeLocation(XMFLOAT3(-3.0f,0.0f,0.0f));
	TestSMComp2->SetRelativeScale3D(XMFLOAT3(0.5f,0.5f,0.5f));
}

void ATestActor2::Tick(float DeltaSeconds)
{
	ATestCube::Tick(DeltaSeconds);

	float RotationSpeed = 100;
	RootComponent->AddWorldRotation(XMFLOAT3(0.0f, DeltaSeconds*RotationSpeed, 0.0f));
}

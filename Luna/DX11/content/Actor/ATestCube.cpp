#include "ATestCube.h"

#include "Engine/UEditorEngine.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"


ATestCube::ATestCube()
{
	TestCubeStaticMeshComp = std::make_shared<UStaticMeshComponent>();
	TestCubeStaticMeshComp->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Racco"));
	TestCubeStaticMeshComp->SetupAttachment(GetRootComponent());
	
	TestCubeStaticMeshComp->SetRelativeLocation(XMFLOAT3(3.0f,0.0f,0.0f));
	//TestCubeStaticMeshComp->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	TestCubeStaticMeshComp->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	TestCubeStaticMeshComp->SetRelativeScale3D(XMFLOAT3(0.02f,0.02f,0.02f));

	TestCube2 = std::make_shared<UStaticMeshComponent>();
	TestCube2->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Cube"));
	TestCube2->SetupAttachment(TestCubeStaticMeshComp);
	TestCube2->SetRelativeLocation(XMFLOAT3(100.0f,10.0f,0.0f));
	TestCube2->SetRelativeScale3D(XMFLOAT3(10.0f,10.0f,10.0f));

	TestCube3 = std::make_shared<UStaticMeshComponent>();
	TestCube3->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Sphere"));
	TestCube3->SetupAttachment(GetRootComponent());
	TestCube3->SetRelativeLocation(XMFLOAT3(-3.0f,0.0f,0.0f));
	TestCube3->SetRelativeScale3D(XMFLOAT3(0.5f,0.5f,0.5f));

}

void ATestCube::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	
}

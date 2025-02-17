#include "ATestCube.h"

#include "Engine/Components/USceneComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"


ATestCube::ATestCube()
{
	TestCubeStaticMeshComp = std::make_shared<UStaticMeshComponent>();
	TestCubeStaticMeshComp->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Racco"));
	TestCubeStaticMeshComp->SetupAttachment(GetRootComponent());

	TestCubeStaticMeshComp->SetRelativeLocation(XMFLOAT3(1.0f,1.0f,0.0f));
	TestCubeStaticMeshComp->SetRelativeRotation(XMFLOAT3(0.0f, 0.0f,0.0f));
	TestCubeStaticMeshComp->SetRelativeScale3D(XMFLOAT3(0.02f,0.02f,0.02f));

	FTransform test =  TestCubeStaticMeshComp->GetComponentToWorld();

	TestCube2 = std::make_shared<UStaticMeshComponent>();
	TestCube2->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Racco"));
	TestCube2->SetupAttachment(TestCubeStaticMeshComp);
	TestCube2->SetRelativeLocation(XMFLOAT3(1.0f*50, -1.0f*50,2.0f*50));
	
	FTransform test2 = TestCube2->GetComponentToWorld();
}

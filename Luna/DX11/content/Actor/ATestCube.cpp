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
	TestCubeStaticMeshComp->SetRelativeScale3D(XMFLOAT3(0.02f,0.02f,0.02f));

	TestCube2 = std::make_shared<UStaticMeshComponent>();
	TestCube2->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Cube"));
	TestCube2->SetupAttachment(TestCubeStaticMeshComp);
	// TODO : SetRelativeLocation의 값 scale은 어떻게 하기
	TestCube2->SetRelativeLocation(XMFLOAT3(50.0f, -50.0f, 100.0f));
	TestCube2->SetRelativeScale3D(XMFLOAT3(10.0f,10.0f,10.0f));

	TestCube3 = std::make_shared<UStaticMeshComponent>();
	TestCube3->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Sphere"));
	TestCube3->SetupAttachment(RootComponent);
	TestCube3->SetRelativeLocation(XMFLOAT3(-1.0f,0.0f,0.0f));
	TestCube3->SetRelativeScale3D(XMFLOAT3(0.25f,0.25f,0.25f));
	

}

#include "ATestCube.h"

#include "Engine/Components/USceneComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"


ATestCube::ATestCube()
{
	TestCubeStaticMeshComp = std::make_shared<UStaticMeshComponent>();
	TestCubeStaticMeshComp->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Racco"));
	TestCubeStaticMeshComp->SetupAttachment(GetRootComponent());
	// 이따가 저 로우포인터 넘기는게 통상의 코딩에서 적합한지 묻기
	
}

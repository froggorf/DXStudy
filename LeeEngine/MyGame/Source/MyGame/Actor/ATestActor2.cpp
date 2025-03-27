#include "ATestActor2.h"

#include "Engine/Mesh/USkeletalMesh.h"
#include "Engine/Components/USkeletalMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"

ATestActor2::ATestActor2() 
{
	GetRootComponent()->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));

	//TestSMComp = std::make_shared<UStaticMeshComponent>();
	//TestSMComp->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_ChibiCat"));
	//TestSMComp->SetupAttachment(GetRootComponent());
	//TestSMComp->SetRelativeLocation(XMFLOAT3(0.0f,5.0f,0.0f));
	//TestSMComp->SetRelativeScale3D(XMFLOAT3(0.2f,0.2f,0.2f));
	
	TestSKComp = std::make_shared<USkeletalMeshComponent>();
	TestSKComp->SetSkeletalMesh(USkeletalMesh::GetSkeletalMesh("SK_ChibiCat"));
	TestSKComp->SetupAttachment(GetRootComponent());
	TestSKComp->SetRelativeLocation(XMFLOAT3(0.0f,5.0f,0.0f));
	TestSKComp->SetRelativeScale3D(XMFLOAT3(0.2f,0.2f,0.2f));
	if(GEngine&& GEngine->TestAnim1)
	{
		TestSKComp->SetAnimation(GEngine->TestAnim1);	
	}
	
	

	//TestSMComp2= std::make_shared<UStaticMeshComponent>();
	//TestSMComp2->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Sphere"));
	//TestSMComp2->SetupAttachment(GetRootComponent());
	//TestSMComp2->SetRelativeLocation(XMFLOAT3(-0.0f,0.0f,0.0f));
	//TestSMComp2->SetRelativeScale3D(XMFLOAT3(0.5f,0.5f,0.5f));
}

void ATestActor2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	//float RotationSpeed = 100;
	//RootComponent->AddWorldRotation(XMFLOAT3(0.0f, DeltaSeconds*RotationSpeed, 0.0f));
}

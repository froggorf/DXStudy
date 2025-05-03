#include "ATestActor2.h"

#include "Engine/Mesh/USkeletalMesh.h"
#include "Engine/Components/USkeletalMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"

ATestActor2::ATestActor2() 
{
	GetRootComponent()->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));

	/*TestSKComp = std::make_shared<USkeletalMeshComponent>();
	TestSKComp->SetupAttachment(GetRootComponent());
	TestSKComp->SetSkeletalMesh(USkeletalMesh::GetSkeletalMesh("SK_MyUEFN"));
	TestSKComp->SetAnimInstanceClass("UMyAnimInstance");
	
	TestSKComp->SetRelativeLocation(XMFLOAT3(0.0f,5.0f,0.0f));
	TestSKComp->SetRelativeScale3D(XMFLOAT3(0.2f,0.2f,0.2f));*/

	UTestComponent* NewTestComp = dynamic_cast<UTestComponent*>( CreateDefaultSubobject("TestActorComp", "UTestComponent"));
	if(NewTestComp)
	{
		TestComponent = std::make_shared<UTestComponent>(*NewTestComp);	
	}
	

}

void ATestActor2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	//float RotationSpeed = 100;
	//RootComponent->AddWorldRotation(XMFLOAT3(0.0f, DeltaSeconds*RotationSpeed, 0.0f));
}

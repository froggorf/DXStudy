#include "CoreMinimal.h"
#include "UShapeComponent.h"

#include "UPhysicsEngine.h"

void UShapeComponent::Register()
{
	UPrimitiveComponent::Register();

	RegisterPhysics();
}

void UShapeComponent::UnRegister()
{
	UPrimitiveComponent::UnRegister();

	UnRegisterPhysics();
}

void UShapeComponent::RegisterPhysics()
{
	RigidActor = CreateRigidActor();
	if (RigidActor)
	{
		RigidActor->userData = this;
	}
}

void UShapeComponent::UnRegisterPhysics()
{
	if (gPhysicsEngine)
	{
		if (RigidActor)
		{
			gPhysicsEngine->UnRegisterActor(RigidActor);
		}
	}
}

void UShapeComponent::SetWorldTransform(const FTransform& NewTransform)
{
	// StaticMesh와 같이 존재할 경우에는 부착된 클래스에 위치를 맞춰줘야하므로 해당 방식으로 위치 조정
	if (const std::shared_ptr<USceneComponent>& ParentComp = GetAttachParent())
	{
		ParentComp->SetWorldRotation(NewTransform.GetRotationQuat());
		ParentComp->SetWorldLocation(NewTransform.GetTranslation());		
	}
	else
	{
		SetWorldRotation(NewTransform.GetRotationQuat());
		SetWorldLocation(NewTransform.GetTranslation());	
	}
	
}

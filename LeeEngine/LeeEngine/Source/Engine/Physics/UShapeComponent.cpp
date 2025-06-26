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

void UShapeComponent::TickComponent(float DeltaSeconds)
{
	UPrimitiveComponent::TickComponent(DeltaSeconds);

#ifdef MYENGINE_BUILD_DEBUG || MYENGINE_BUILD_DEVELOPMENT
	if (bDrawDebug)
	{
		// 렌더링 쓰레드 프레임당 등록
		if (LastDrawDebugRenderThreadFrame != RenderingThreadFrameCount)
		{
			FDebugRenderData RenderData;
			RenderData.Transform = GetComponentTransform();
			RenderData.DebugColor = XMFLOAT4{0.0f,1.0f,0.0f,1.0f};
			RenderData.RemainTime = 0.007f;
			RenderData.ShapeComp = std::dynamic_pointer_cast<UShapeComponent>(shared_from_this());
			FScene::DrawDebugData_GameThread(RenderData, LastDrawDebugRenderThreadFrame);
			LastDrawDebugRenderThreadFrame = RenderingThreadFrameCount;	
		}
		
	}
#endif
}

void UShapeComponent::AddForce(const XMFLOAT3& Force) const
{
	if (bIsDynamic && RigidActor)
	{
		RigidActor->is<physx::PxRigidDynamic>()->addForce({Force.x,Force.y,-Force.z});
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

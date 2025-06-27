#include "CoreMinimal.h"
#include "UShapeComponent.h"

#include "UPhysicsEngine.h"

UShapeComponent::UShapeComponent()
{
	for (size_t i = 0; i < CollisionResponse.size(); ++i)
	{
		CollisionResponse[i] = ECollisionResponse::Block;
	}
}

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
		UpdatePhysicsFilterData();
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

#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
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

void UShapeComponent::SetObjectType(ECollisionChannel Channel)
{
	ObjectType = Channel;
	UpdatePhysicsFilterData();
}

void UShapeComponent::SetResponseToChannel(ECollisionChannel Channel, ECollisionResponse NewResponse)
{
	CollisionResponse[static_cast<UINT>(Channel)] = NewResponse;
	UpdatePhysicsFilterData();
}

void UShapeComponent::UpdatePhysicsFilterData()
{
	if (!RigidActor)
	{
		return;
	}

	physx::PxU32 ShapeCount = RigidActor->getNbShapes();
	std::vector<physx::PxShape*> Shapes(ShapeCount);
	RigidActor->getShapes(Shapes.data(), ShapeCount);
	
	

	for (physx::PxShape* Shape : Shapes)
	{
		//RigidActor->detachShape(*Shape);
		physx::PxFilterData FilterData;
		FilterData.word0 = static_cast<physx::PxU32>(1 << static_cast<UINT>(ObjectType));

		physx::PxU32 BlockMask = 0;
		physx::PxU32 OverlapMask = 0;

		for (UINT ChannelIdx = 0; ChannelIdx < static_cast<UINT>(ECollisionChannel::Count); ++ChannelIdx)
		{
			ECollisionResponse Response = CollisionResponse[ChannelIdx];
			physx::PxU32 Bit = 1 << ChannelIdx;

			if (Response == ECollisionResponse::Block)
			{
				BlockMask |= Bit;
			}
			else if (Response == ECollisionResponse::Overlap)
			{
				OverlapMask |= Bit;
			}
		}

		FilterData.word1 = BlockMask;
		FilterData.word2 = OverlapMask;

		Shape->setSimulationFilterData(FilterData);
		//RigidActor->attachShape(*Shape);
	}

	
}

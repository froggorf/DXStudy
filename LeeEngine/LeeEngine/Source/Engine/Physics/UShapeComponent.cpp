#include "CoreMinimal.h"
#include "UShapeComponent.h"

UShapeComponent::UShapeComponent()
{
	SetCollisionEnabled(ECollisionEnabled::Physics);
	ObjectType = ECollisionChannel::Visibility;
	for (size_t i = 0; i < CollisionResponse.size(); ++i)
	{
		CollisionResponse[i] = ECollisionResponse::Overlap;  // 기본값
	}

	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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
		GPhysicsEngine->AddActor(RigidActor);
		UpdatePhysicsFilterData();
	}
}

void UShapeComponent::UnRegisterPhysics()
{
	if (GPhysicsEngine)
	{
		if (RigidActor)
		{
			GPhysicsEngine->UnRegisterActor(RigidActor);
			RigidActor = nullptr;
		}
	}
}

void UShapeComponent::ResetPhysics()
{
	if (!RigidActor)
	{
		return;
	}

	GPhysicsEngine->UnRegisterActor(RigidActor);
	RegisterPhysics();
}

XMFLOAT3 UShapeComponent::GetPhysicsLocation() const
{
	if (!RigidActor)
	{
		return XMFLOAT3{0.0f, 0.0f, 0.0f};
	}

	physx::PxTransform GlobalPose = RigidActor->getGlobalPose();

	// PhysX 좌표계 -> 언리얼 좌표계 (Z축 반전)
	return XMFLOAT3{
		GlobalPose.p.x,
		GlobalPose.p.y,
		-GlobalPose.p.z
	};
}

XMFLOAT4 UShapeComponent::GetPhysicsRotation() const
{
	if (!RigidActor)
	{
		return XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f};
	}

	physx::PxTransform GlobalPose = RigidActor->getGlobalPose();
	physx::PxQuat PxQuat = GlobalPose.q;

	// PhysX 쿼터니언 -> 언리얼 쿼터니언 (Z축 반전)
	return XMFLOAT4{
		-PxQuat.x,
		-PxQuat.y,
		PxQuat.z,
		PxQuat.w
	};
}

FTransform UShapeComponent::GetPhysicsTransform() const
{
	FTransform Result;

	if (!RigidActor)
	{
		return Result;
	}

	physx::PxTransform GlobalPose = RigidActor->getGlobalPose();

	// Translation
	Result.Translation = XMFLOAT3{
		GlobalPose.p.x,
		GlobalPose.p.y,
		-GlobalPose.p.z
	};

	// Rotation
	Result.Rotation = XMFLOAT4{
		-GlobalPose.q.x,
		-GlobalPose.q.y,
		GlobalPose.q.z,
		GlobalPose.q.w
	};

	// Scale (PhysX는 스케일 정보 없음)
	Result.Scale3D = XMFLOAT3{1.0f, 1.0f, 1.0f};

	return Result;
}

void UShapeComponent::TickComponent(float DeltaSeconds)
{
	UPrimitiveComponent::TickComponent(DeltaSeconds);

#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
	if (bDrawDebug || bShowCollision)
	{
		// 렌더링 쓰레드 프레임당 등록
		if (LastDrawDebugRenderThreadFrame != RenderingThreadFrameCount)
		{
			FDebugRenderData RenderData;
			RenderData.Transform = GetComponentTransform();
			RenderData.DebugColor = XMFLOAT4{0.0f,1.0f,0.0f,1.0f};
			RenderData.RemainTime = 1.0f/60.0f;
			RenderData.ShapeComp = std::dynamic_pointer_cast<UShapeComponent>(shared_from_this());
			FScene::DrawDebugData_GameThread(RenderData);
			LastDrawDebugRenderThreadFrame = RenderingThreadFrameCount;	
		}
		
	}
#endif

	// simulate 되지 않는 오브젝트는 직접 갱신해주기
	// RigidActor가 있고, 시뮬레이션을 안하거나,
	if ( RigidActor && (!bSimulatePhysics)  )
	{
		const FTransform& CurTransform = GetComponentTransform();
		RigidActor->setGlobalPose(physx::PxTransform{CurTransform.Translation.x,CurTransform.Translation.y,-CurTransform.Translation.z, {-CurTransform.Rotation.x,-CurTransform.Rotation.y,CurTransform.Rotation.z,CurTransform.Rotation.w}});
	}
	// 피직스인데 kinematic인경우엔
	else if (RigidActor && CollisionEnabled == ECollisionEnabled::Physics)
	{
		if (physx::PxRigidDynamic* Dynamic = RigidActor->is<physx::PxRigidDynamic>())
		{
			if (Dynamic->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC))
			{
				const FTransform& CurTransform = GetComponentTransform();
				Dynamic->setKinematicTarget(physx::PxTransform{CurTransform.Translation.x,CurTransform.Translation.y,-CurTransform.Translation.z, {-CurTransform.Rotation.x,-CurTransform.Rotation.y,CurTransform.Rotation.z,CurTransform.Rotation.w}});
			}
		}
		
		
	}
}

void UShapeComponent::AddForce(const XMFLOAT3& Force) const
{
	if (RigidActor && bSimulatePhysics)
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
		physx::PxFilterData FilterData;
		FilterData.word0 = static_cast<physx::PxU32>(1 << static_cast<UINT>(ObjectType));

		if (FilterData.word0 == 0)
		{
			int a = 0;
		}
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
		Shape->setQueryFilterData(FilterData);
	}

	if (bIsAddedToPxScene && RigidActor->getScene())
	{
		RigidActor->getScene()->resetFiltering(*RigidActor);
	}

	if (physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>())
	{
		if (bIsAddedToPxScene)
		{
			RigidDynamic->wakeUp();
			
		}
	}

}

void UShapeComponent::SetSimulatePhysics(bool bNewSimulatePhysics)
{
	if (bNewSimulatePhysics != bSimulatePhysics)
	{
		bSimulatePhysics = bNewSimulatePhysics;
		ResetPhysics();
	}
}

void UShapeComponent::SetKinematicRigidBody(bool bNewKinematic)
{
	if (GetRigidActor())
	{
		if (physx::PxRigidDynamic* Dynamic = (GetRigidActor())->is<physx::PxRigidDynamic>())
		{
			Dynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC,true);
		}		
	}
}

void UShapeComponent::SetCollisionEnabled(ECollisionEnabled NewType)
{
	CollisionEnabled = NewType;
	//bDrawDebug = NewType != ECollisionEnabled::NoCollision;

	// NoCollision 이 안먹히는 현상이 존재하여 해당 방식을 추가 적용
	if (NewType == ECollisionEnabled::NoCollision)
	{
		if (RigidActor && bIsAddedToPxScene)
		{
			UnRegisterPhysics();
			return;
		}
	}
	else
	{
		if (!RigidActor && !bIsAddedToPxScene)
		{
			RegisterPhysics();
		}
	}

	if (!RigidActor)
	{
		return;
	}

	physx::PxU32 ShapeCount = RigidActor->getNbShapes();
	std::vector<physx::PxShape*> Shapes(ShapeCount);
	RigidActor->getShapes(Shapes.data(), ShapeCount);

	for (physx::PxShape* Shape : Shapes)
	{
		switch (NewType)
		{
		case ECollisionEnabled::NoCollision:
			Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
			Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
			Shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
		break;
		case ECollisionEnabled::Physics:
			Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
			Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
			Shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
		break;
		case ECollisionEnabled::QueryOnly:
			Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
			Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
			Shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		break;
		}
	}
	UpdatePhysicsFilterData();


}

#include "ATestCube.h"

#include "Engine/UEditorEngine.h"
#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"


ATestCube::ATestCube()
{
	//GetRootComponent()->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	//TestCubeStaticMeshComp = std::make_shared<UStaticMeshComponent>();
	//TestCubeStaticMeshComp->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Point"));
	//TestCubeStaticMeshComp->SetupAttachment(GetRootComponent());
	//TestCubeStaticMeshComp->SetRelativeScale3D(XMFLOAT3{30.0f,30.0f,30.0f});
	//
	//TestCubeStaticMeshComp->SetRelativeLocation(XMFLOAT3(0.0f,0.0f,0.0f));
	////TestCubeStaticMeshComp->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	//TestCubeStaticMeshComp->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	//TestCubeStaticMeshComp->SetRelativeScale3D(XMFLOAT3(0.2f,0.2f,0.2f));
	//
	TestCube2 = std::make_shared<UStaticMeshComponent>();
	TestCube2->SetupAttachment(GetRootComponent());
	TestCube2->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Cube"));
	TestCube2->SetRelativeScale3D(XMFLOAT3(200.0f,1.0f,200.0f));
	TestCube2->SetRelativeScale3D(XMFLOAT3(200.0f,1.0f,200.0f));
	TestCube2->SetRelativeLocation(XMFLOAT3(0.0f,-25.0f,0.0f));

	DummyComp = std::make_shared<USceneComponent>();
	DummyComp->Rename("DummyComp");
	DummyComp->SetupAttachment(GetRootComponent());
	DummyComp->SetRelativeLocation(XMFLOAT3{-45.0f,50.0f,0.0f});

	NiagaraComp = std::make_shared<UNiagaraComponent>();
	NiagaraComp->SetupAttachment(DummyComp);
	std::shared_ptr<UNiagaraSystem> System = UNiagaraSystem::GetNiagaraAsset("NS_Ribbon");
	NiagaraComp->SetNiagaraAsset(System);
	NiagaraComp->SetRelativeLocation(XMFLOAT3{0.0,30.0f,0.0f});
	NiagaraComp->SetRelativeRotation(XMFLOAT3{90.0f,0.0f,0.0f});


	Rename("ATestCube" + std::to_string(ActorID));
}

void ATestCube::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	//SetActorRotation(XMFLOAT4(0.0f,0.0f,0.0f,1.0f));
	
	if(false)
	{
		XMFLOAT4 RR = DummyComp->GetRelativeRotation();
		XMVECTOR RRVec = XMLoadFloat4(&RR);

		XMVECTOR Z1Quat = XMQuaternionRotationAxis(XMVectorSet(0,0,1,0), DeltaSeconds);


		DummyComp->SetRelativeRotation(XMQuaternionMultiply(RRVec,Z1Quat));

	}
	else
	{
		static XMVECTOR A{-100,30,-100};
		static XMVECTOR B{100,30,-100};
		static XMVECTOR C{100,30,100};
		static float t = 0.0f;
		t += DeltaSeconds/2;
		XMVECTOR Target;
		if(t < 1.0f)
		{
			Target = XMVectorLerp(A,B, t);
		}
		else if(t < 2.0f)
		{
			Target = XMVectorLerp(B,A, t-1);
		}
		else
		{
			Target = XMVectorLerp(B,A, t-1);
			t-=2.0f;
		}
		XMFLOAT3 Loc;
		XMStoreFloat3(&Loc, Target);
		DummyComp->SetWorldLocation(Loc);
	}
	
}

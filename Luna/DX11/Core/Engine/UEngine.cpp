// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UEngine.h"

#include "Mesh/UStaticMesh.h"
#include "World/UWorld.h"

UEngine* GEngine = nullptr;
void UEngine::InitEngine()
{
	CurrentWorld = std::make_shared<UWorld>();

	PostLoad();

	CurrentWorld->Init();
}

void UEngine::PostLoad()
{
	UObject::PostLoad();

	LoadAllObjectsFromFile();

	// TODO: 추후엔 Level을 직렬화를 통해 저장 및 로드를 할 수 있는 기능을 만드는게 낫지 않을까?
	// 그렇다면 여기에서 로드를 진행하기
}

void UEngine::Tick(float DeltaSeconds)
{
	if(CurrentWorld)
	{
		CurrentWorld->TickWorld(DeltaSeconds);
	}
}

void UEngine::Draw()
{
	std::cout << "UEngine::Draw()"<<std::endl;
}

void UEngine::LoadAllObjectsFromFile()
{
	// 디스크에 내에 있는 모든 myasset 파일을 로드하는 함수
	// TODO: 추후 디스크 읽는 라이브러리를 사용하기
	AssetManager::ReadMyAsset<UStaticMesh>("Asset/StaticMesh/SM_Racco.myasset");

	std::cout<< "StaticMesh Size -> "<<UStaticMesh::GetStaticMeshCache().size()<<std::endl;
}

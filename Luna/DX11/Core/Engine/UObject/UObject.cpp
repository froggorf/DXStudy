// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UObject.h"

//std::unordered_map<std::string, std::unique_ptr<UObject>> UObject::ClassDefaultObject;

UObject::UObject()
{
}

UObject::~UObject()
{
}

void UObject::Init()
{
	
}

void UObject::PostLoad()
{
}

void UObject::Rename(const std::string& NewName)
{
	NamePrivate = NewName;
}

void UObject::LoadDataFromFileData(std::map<std::string, std::string>& AssetData)
{
	Rename(AssetData["Name"]);
	
}

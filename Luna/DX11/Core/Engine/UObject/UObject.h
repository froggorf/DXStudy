// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/MyEngineUtils.h"

class UWorld;

class UObject
{
public:
	UObject();
	virtual ~UObject();

	virtual void Init();

	// 디스크에서 로드될 때 실행될 함수
	virtual void PostLoad();

	std::string GetName() const
	{
		return NamePrivate;
	}

	void Rename(const std::string& NewName);

	virtual void LoadDataFromFileData(std::vector<std::string>& StaticMeshAssetData);
protected:
private:
public:
protected:
private:
	std::string NamePrivate;
	
};

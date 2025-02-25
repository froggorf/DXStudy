// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/MyEngineUtils.h"

#define MY_GENERATED_BODY(CLASS)\
class MakeCDO\
{\
public:\
	MakeCDO()\
	{\
		UObject::AddClassDefaultObject(#CLASS,std::make_unique<CLASS>());\
	}\
};\
inline static MakeCDO _initializer;


class UWorld;

class UObject
{
	MY_GENERATED_BODY(UObject)
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

	// .myasset으로 부터 읽은 데이터를 각 상속된 클래스에서 재해석하여 데이터 읽기
	virtual void LoadDataFromFileData(std::map<std::string, std::string>& AssetData);


protected:
private:
public:
protected:
private:
	std::string NamePrivate;

private:

private:
	//MY_GENERATED_BODY(UObject)
	// Class Default Object 보관
	static std::unordered_map<std::string, std::unique_ptr<UObject>>& GetMap(){
		static std::unordered_map<std::string, std::unique_ptr<UObject>> ClassDefaultObjectMap;
		return ClassDefaultObjectMap;
	}
public:
	static void AddClassDefaultObject(const std::string& ClassName, std::unique_ptr<UObject>&& DefaultObject)
	{
		GetMap()[ClassName] = std::move(DefaultObject);
	}

};


//std::unordered_map<std::string, std::unique_ptr<UObject>> UObject::ClassDefaultObject;

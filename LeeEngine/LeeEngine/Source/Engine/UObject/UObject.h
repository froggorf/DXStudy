// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/MyEngineUtils.h"

#include "nlohmann/json.hpp"

#define GENERATE_UOBJECT_DERIVED_CLASS(CLASS)\
class MakeCDO\
{\
public:\
	MakeCDO()\
	{\
		UObject::AddClassDefaultObject(#CLASS,std::make_unique<CLASS>());\
	}\
};\
inline static MakeCDO _initializer;\


#define MY_GENERATED_BODY(CLASS)\
GENERATE_UOBJECT_DERIVED_CLASS(CLASS)\
public:\
	const virtual std::unique_ptr<UObject>& CreateUniqueInstance() const {return std::make_unique<CLASS>();}\
	virtual std::shared_ptr<UObject> CreateInstance() const override { return std::make_shared<CLASS>();}\
	std::string GetClass() const override { return #CLASS; } \


class UWorld;

class UObject
{
	GENERATE_UOBJECT_DERIVED_CLASS(UObject)
public:
	virtual const std::unique_ptr<UObject>& CreateUniqueInstance() const {return std::make_unique<UObject>();}
	virtual std::shared_ptr<UObject> CreateInstance() const { return std::make_shared<UObject>(); }
	virtual std::string GetClass() const {return "UObject";}
	virtual void BeginPlay() {}

	
	
public:
	UObject();
	UObject(const UObject& other) {};
	virtual ~UObject();

	virtual void Init();

	// 디스크에서 로드될 때 실행될 함수
	virtual void PostLoad();

	// 게임 시작 전 / Spawn 후 게임에 넣기 이전에 호출
	// 렌더쓰레드에 씬프록시를 추가하는 등의 작업 진행
	virtual void Register(){bIsRegister = true;}
	bool IsRegister() const {return bIsRegister;}
	
	std::string GetName() const
	{
		return NamePrivate;
	}

	void Rename(const std::string& NewName);

	// .myasset으로 부터 읽은 데이터를 각 상속된 클래스에서 재해석하여 데이터 읽기
	virtual void LoadDataFromFileData(const nlohmann::json& AssetData);
	virtual void SaveDataFromAssetToFile(nlohmann::json& Json);

protected:
private:
public:
protected:
private:
	std::string NamePrivate;


private:
	bool bIsRegister = false;
private:
	//MY_GENERATED_BODY(UObject)
	// Class Default Object 보관
	static std::unordered_map<std::string, std::unique_ptr<UObject>>& GetCDOMap(){
		static std::unordered_map<std::string, std::unique_ptr<UObject>> ClassDefaultObjectMap;
		return ClassDefaultObjectMap;
	}

public:
	static void AddClassDefaultObject(const std::string& ClassName, std::unique_ptr<UObject>&& DefaultObject)
	{
		GetCDOMap()[ClassName] = std::move(DefaultObject);
	}
	static const UObject* GetDefaultObject(const std::string& ClassName) {return GetCDOMap()[ClassName].get();}

};


//std::unordered_map<std::string, std::unique_ptr<UObject>> UObject::ClassDefaultObject;

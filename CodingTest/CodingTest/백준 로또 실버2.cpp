#include <map>
////#include <iostream>
////#include <string>
////#include <vector>
////
////int Count = 0;
////void Print(const std::vector<int>& Nums, int Current, std::vector<int>& Data)
////{
////	Data.emplace_back(Nums[Current]);
////	if (Data.size() == 6)
////	{
////		for (int i = 0; i < 6; ++i)
////		{
////			std::cout<< Data[i]<<" ";
////		}
////		std::cout<<"\n";
////		Data.pop_back();
////		return;
////	}
////
////
////	for (int i = Current+1; i < Count; ++i)
////	{
////		Print(Nums, i, Data);
////	}
////
////	Data.pop_back();
////}
////
////int main()
////{
////
////	while (true)
////	{
////		std::cin >> Count;
////		if (Count == 0) break;
////
////		std::vector<int> Nums(Count);
////		for (int i =0 ; i < Count; ++i)
////		{
////			std::cin >> Nums[i];
////		}
////
////		for (int i = 0; i < Count-6+1; ++i)
////		{
////			std::vector<int> Data;
////			Print(Nums,i, Data);
////		}
////
////		std::cout<<"\n";
////	}
////}
//
//
//
//#include <iostream>
//#include <memory>
//#include <string>
//#include <type_traits>
//#include <cassert>
//#include <typeinfo>
//#include <functional>
//
//// 기본 값 인터페이스
//class BaseValue {
//private:
//    // 타입 삭제를 위한 내부 함수 포인터들
//    std::function<const std::type_info&()> getTypeInfoFunc;
//    std::function<void*()> getValuePtrFunc;
//    std::function<void(const void*)> setValueFunc;
//
//public:
//    virtual ~BaseValue() = default;
//
//    // 생성자에서 타입 정보와 접근 함수들을 설정
//    template <typename T>
//    BaseValue(T* valuePtr) {
//        getTypeInfoFunc = []() -> const std::type_info&{ return typeid(T); };
//        getValuePtrFunc = [valuePtr](){ return static_cast<void*>(valuePtr); };
//        setValueFunc = [valuePtr](const void* newValuePtr){
//            *valuePtr = *static_cast<const T*>(newValuePtr);
//            };
//    }
//
//    // 타입 정보 반환
//    const std::type_info& GetTypeInfo() const {
//        return getTypeInfoFunc();
//    }
//
//    // 템플릿 Get 메서드
//    template <typename T>
//    T Get() const {
//        // 타입 검사
//        assert(typeid(T) == GetTypeInfo() && "타입 불일치: 요청한 타입으로 변환할 수 없습니다");
//
//        // 값 반환
//        void* valuePtr = getValuePtrFunc();
//        return *static_cast<T*>(valuePtr);
//    }
//
//    // 템플릿 Set 메서드
//    template <typename T>
//    void Set(const T& newValue) {
//        // 타입 검사
//        assert(typeid(T) == GetTypeInfo() && "타입 불일치: 요청한 타입으로 변환할 수 없습니다");
//
//        // 값 설정
//        setValueFunc(&newValue);
//    }
//};
//
//// 정수형 값 컨테이너
//class IntValue : public BaseValue {
//private:
//    int value;
//
//public:
//    explicit IntValue(int initialValue = 0) 
//        : BaseValue(&value), value(initialValue) {}
//
//    // 기본 Get/Set 메서드 (편의성을 위해)
//    int GetValue() const { return value; }
//    void SetValue(int newValue) { value = newValue; }
//};
//
//// 불리언 값 컨테이너
//class BoolValue : public BaseValue {
//private:
//    bool value;
//
//public:
//    explicit BoolValue(bool initialValue = false) 
//        : BaseValue(&value), value(initialValue) {}
//
//    // 기본 Get/Set 메서드 (편의성을 위해)
//    bool GetValue() const { return value; }
//    void SetValue(bool newValue) { value = newValue; }
//};
//
//// 공유 포인터 값 컨테이너
//template <typename T>
//class SharedPtrValue : public BaseValue {
//private:
//    std::shared_ptr<T> value;
//
//public:
//    explicit SharedPtrValue(const std::shared_ptr<T>& initialValue = nullptr) 
//        : BaseValue(&value), value(initialValue ? initialValue : std::make_shared<T>()) {
//
//        // 특수화된 Get/Set 메서드를 위한 오버라이드
//        // (이 부분은 현재 구현에서는 작동하지 않습니다. 더 복잡한 구현이 필요함)
//    }
//
//    // 기본 Get/Set 메서드 (편의성을 위해)
//    std::shared_ptr<T> GetValue() const { return value; }
//    void SetValue(const std::shared_ptr<T>& newValue) { value = newValue; }
//
//    // 원시 포인터 접근
//    T* GetRaw() const {
//        return value.get();
//    }
//
//    // 역참조 접근
//    T& operator*() const {
//        assert(value && "SharedPtrValue: null 포인터 역참조 시도");
//        return *value;
//    }
//
//    // 멤버 접근
//    T* operator->() const {
//        assert(value && "SharedPtrValue: null 포인터 멤버 접근 시도");
//        return value.get();
//    }
//};
//
//// 타입 안전한 다운캐스팅을 위한 헬퍼 함수
//template <typename T>
//T* value_cast(BaseValue* base) {
//    if (!base) return nullptr;
//
//    if (typeid(T) == typeid(*base) || 
//        (std::is_base_of_v<BaseValue, T> && dynamic_cast<T*>(base))) {
//        return static_cast<T*>(base);
//    }
//    return nullptr;
//}
//
//template <typename T>
//const T* value_cast(const BaseValue* base) {
//    return value_cast<T>(const_cast<BaseValue*>(base));
//}
//
//int main() {
//    // 요청하신 사용 패턴 구현
//    std::shared_ptr<BaseValue> value = std::make_shared<IntValue>(42);
//
//    // 템플릿 메서드 직접 호출 - 이제 올바르게 작동
//    int val = value->Get<int>();
//    std::cout << "IntValue via Get<int>(): " << val << std::endl;
//
//    value->Set<int>(100);
//    std::cout << "After Set<int>(100): " << value->Get<int>() << std::endl;
//
//    // 잘못된 타입으로 접근 시도 - assert 실패
//    // value->Set<bool>(true);  // assert 실패
//
//    // BoolValue 테스트
//    std::shared_ptr<BaseValue> boolValue = std::make_shared<BoolValue>(true);
//    bool bVal = boolValue->Get<bool>();
//    std::cout << "BoolValue: " << (bVal ? "true" : "false") << std::endl;
//
//    // 다운캐스팅 테스트
//    auto intValuePtr = value_cast<IntValue>(value.get());
//    if (intValuePtr) {
//        std::cout << "IntValue via value_cast: " << intValuePtr->GetValue() << std::endl;
//    }
//
//    // 공유 포인터 테스트 (기본 std::shared_ptr<T> 타입만 지원)
//    auto strPtr = std::make_shared<std::string>("Hello, World!");
//    std::shared_ptr<BaseValue> ptrValue = std::make_shared<SharedPtrValue<std::string>>(strPtr);
//
//    auto retrievedPtr = ptrValue->Get<std::shared_ptr<std::string>>();
//    std::cout << "SharedPtrValue: " << *retrievedPtr << std::endl;
//
//    std::shared_ptr<std::string> newStrPtr = std::make_shared<std::string>("Updated string");
//    ptrValue->Set<std::shared_ptr<std::string>>(newStrPtr);
//    std::cout << "After update: " << *ptrValue->Get<std::shared_ptr<std::string>>() << std::endl;
//
//    return 0;
//}
class UObject
{
public:
	int a = 1;
};

#include <iostream>
#include <memory>
#include <DirectXMath.h>

struct FEngineGenericTypes
{
	// 8-bit unsigned integer
	typedef unsigned char 		uint8;

	// 16-bit unsigned integer
	typedef unsigned short int	uint16;

	// 32-bit unsigned integer
	typedef unsigned int		uint32;

	// 64-bit unsigned integer
	typedef unsigned long long	uint64;

	// 8-bit signed integer
	typedef	signed char			int8;

	// 16-bit signed integer
	typedef signed short int	int16;

	// 32-bit signed integer
	typedef signed int	 		int32;

	// 64-bit signed integer
	typedef signed long long	int64;
};

typedef FEngineGenericTypes::uint8		uint8;
typedef FEngineGenericTypes::uint16		uint16;
typedef FEngineGenericTypes::uint32 	uint32;
typedef FEngineGenericTypes::uint64		uint64;
typedef FEngineGenericTypes::int8		int8;
typedef FEngineGenericTypes::int16		int16;
typedef FEngineGenericTypes::int32		int32;
typedef FEngineGenericTypes::int64		int64;



struct FBlackBoardValueType
{
	virtual ~FBlackBoardValueType() = default;

	template<typename T>
	static bool SetValueInMemory(uint8* MemoryBlock, const T& Value)
	{
		const bool bChanged = *((T*)MemoryBlock) != Value;
		*((T*)MemoryBlock) = Value;

		return bChanged;
	}

	template<typename T>
	static T GetValueFromMemory(const unsigned char* MemoryBlock)
	{
		return *((T*)MemoryBlock);
	}

	template<typename T>
	static bool SetWeakObjectInMemory(uint8* MemoryBlock, const std::shared_ptr<T>& Value)
	{
		std::weak_ptr<T>* PrevValue = (std::weak_ptr<T>*)MemoryBlock;
		const bool bChanged = (Value) != PrevValue->lock();
		*((std::weak_ptr<T>*)MemoryBlock) = Value;

		return bChanged;
	}
};

struct FBlackBoardValueType_Bool : public FBlackBoardValueType
{
public:
	~FBlackBoardValueType_Bool() override = default;

	typedef bool FDataType;
	static constexpr FDataType InvalidValue = false;

	static bool SetValue(FBlackBoardValueType_Bool* ValueObj, uint8* RawData, const bool& bValue);
	static bool GetValue(const FBlackBoardValueType_Bool* ValueObj, const uint8* RawData);

	static uint8* GetRawData(const FBlackBoardValueType_Bool* ValueObj) {return (uint8*)(&ValueObj->Value);}
protected:
	bool Value = false;
};

bool FBlackBoardValueType_Bool::SetValue(FBlackBoardValueType_Bool* ValueObj, uint8* RawData, const bool& bValue)
{
	return SetValueInMemory<uint8>(RawData, bValue);
}

bool FBlackBoardValueType_Bool::GetValue(const FBlackBoardValueType_Bool* ValueObj, const uint8* RawData)
{
	return GetValueFromMemory<uint8>(RawData) != 0;
}

struct FBlackBoardValueType_Int : public FBlackBoardValueType
{
public:
	~FBlackBoardValueType_Int() override = default;

	typedef int FDataType;
	static constexpr FDataType InvalidValue = INT32_MIN;

	static bool SetValue(FBlackBoardValueType_Int* ValueObj, uint8* RawData, const int& Value);
	static int GetValue(const FBlackBoardValueType_Int* ValueObj, const uint8* RawData);

	static uint8* GetRawData(const FBlackBoardValueType_Int* ValueObj) {return (uint8*)(&ValueObj->Value);}
protected:
	int Value = INT32_MIN;
};

bool FBlackBoardValueType_Int::SetValue(FBlackBoardValueType_Int* ValueObj, uint8* RawData, const int& Value)
{
	return SetValueInMemory<int>(RawData, Value);
}

int FBlackBoardValueType_Int::GetValue(const FBlackBoardValueType_Int* ValueObj, const uint8* RawData)
{
	return GetValueFromMemory<int>(RawData);
}

struct FBlackBoardValueType_Object : public FBlackBoardValueType
{
public:
	~FBlackBoardValueType_Object() override = default;

	typedef std::shared_ptr<UObject> FDataType;
	static const FDataType InvalidValue;

	static bool SetValue(FBlackBoardValueType_Object* ValueObj, uint8* RawData, const std::shared_ptr<UObject>& Value);
	static std::shared_ptr<UObject> GetValue(const FBlackBoardValueType_Object* ValueObj, const uint8* RawData);

	static uint8* GetRawData(const FBlackBoardValueType_Object* ValueObj) {return (uint8*)(&ValueObj->Value);}
protected:
	std::weak_ptr<UObject> Value;
};
const FBlackBoardValueType_Object::FDataType FBlackBoardValueType_Object::InvalidValue = nullptr;

bool FBlackBoardValueType_Object::SetValue(FBlackBoardValueType_Object* ValueObj, uint8* RawData, const std::shared_ptr<UObject>& Value)
{
	return SetWeakObjectInMemory<UObject>(RawData, Value);
}

std::shared_ptr<UObject> FBlackBoardValueType_Object::GetValue(const FBlackBoardValueType_Object* ValueObj, const uint8* RawData)
{
	return ValueObj->Value.lock();
}


class FBlackBoard
{
public:
	template<class TDataClass>
	bool SetValue(const std::string& KeyName, typename TDataClass::FDataType Value)
	{
		auto iter = BlackBoard.find(KeyName);
		if (iter == BlackBoard.end()) {
			std::shared_ptr<TDataClass> NewValue = std::make_shared<TDataClass>();
			iter = BlackBoard.emplace(KeyName, NewValue).first;
		}

		std::shared_ptr<TDataClass> ClassValue = std::dynamic_pointer_cast<TDataClass>(iter->second);
		if (!ClassValue)
		{
			return false;
		}

		uint8* RawData = TDataClass::GetRawData(ClassValue.get());
		return TDataClass::SetValue(ClassValue.get(), RawData, Value);
	}

	template<class TDataClass>
	typename TDataClass::FDataType GetValue(const std::string& KeyName)
	{
		auto BlackBoardIter = BlackBoard.find(KeyName);
		if (BlackBoardIter == BlackBoard.end()) 
		{
			return TDataClass::InvalidValue;
		}

		std::shared_ptr<TDataClass> ClassValue = std::dynamic_pointer_cast<TDataClass>(BlackBoardIter->second);
		if (!ClassValue) 
		{
			return TDataClass::InvalidValue;
		}

		uint8* RawData = TDataClass::GetRawData(ClassValue.get());
		return TDataClass::GetValue(ClassValue.get(), RawData);
	}

protected:
	std::map<std::string, std::shared_ptr<FBlackBoardValueType>> BlackBoard;
};

int main()
{
	std::shared_ptr<FBlackBoard> BlackBoard = std::make_shared<FBlackBoard>();
	std::shared_ptr<UObject> Object = std::make_shared<UObject>();
	Object->a=1000;
	BlackBoard->SetValue<FBlackBoardValueType_Object>("Objasd", Object);
	std::shared_ptr<UObject> NewObject = BlackBoard->GetValue<FBlackBoardValueType_Object>("Obj");
	if (NewObject)
	{
		std::cout<<NewObject->a;
	}
}
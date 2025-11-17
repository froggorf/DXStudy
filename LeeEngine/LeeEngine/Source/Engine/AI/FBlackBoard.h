#pragma once
#include "CoreMinimal.h"
#include "Engine/Misc/Delegate.h"

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

struct FBlackBoardValueType_FLOAT : public FBlackBoardValueType
{
public:
	~FBlackBoardValueType_FLOAT() override = default;

	typedef float FDataType;
	static constexpr FDataType InvalidValue = FLT_MAX;

	static bool SetValue(FBlackBoardValueType_FLOAT* ValueObj, uint8* RawData, const float& Value);
	static int GetValue(const FBlackBoardValueType_FLOAT* ValueObj, const uint8* RawData);

	static uint8* GetRawData(const FBlackBoardValueType_FLOAT* ValueObj) {return (uint8*)(&ValueObj->Value);}
protected:
	float Value = FLT_MAX;
};

struct FBlackBoardValueType_FLOAT2 : public FBlackBoardValueType
{
public:
	~FBlackBoardValueType_FLOAT2() override = default;

	typedef DirectX::XMFLOAT2 FDataType;
	static const FDataType InvalidValue;

	static bool SetValue(FBlackBoardValueType_FLOAT2* ValueObj, uint8* RawData, const DirectX::XMFLOAT2& Value);
	static DirectX::XMFLOAT2 GetValue(const FBlackBoardValueType_FLOAT2* ValueObj, const uint8* RawData);

	static uint8* GetRawData(const FBlackBoardValueType_FLOAT2* ValueObj) {return (uint8*)(&ValueObj->Value);}
protected:
	DirectX::XMFLOAT2 Value = InvalidValue;
};

struct FBlackBoardValueType_FLOAT3 : public FBlackBoardValueType
{
public:
	~FBlackBoardValueType_FLOAT3() override = default;

	typedef DirectX::XMFLOAT3 FDataType;
	static const FDataType InvalidValue;

	static bool SetValue(FBlackBoardValueType_FLOAT3* ValueObj, uint8* RawData, const DirectX::XMFLOAT3& Value);
	static DirectX::XMFLOAT3 GetValue(const FBlackBoardValueType_FLOAT3* ValueObj, const uint8* RawData);

	static uint8* GetRawData(const FBlackBoardValueType_FLOAT3* ValueObj) {return (uint8*)(&ValueObj->Value);}
protected:
	DirectX::XMFLOAT3 Value = InvalidValue;
};

struct FBlackBoardValueType_FLOAT4 : public FBlackBoardValueType
{
public:
	~FBlackBoardValueType_FLOAT4() override = default;

	typedef DirectX::XMFLOAT4 FDataType;
	static const FDataType InvalidValue;

	static bool SetValue(FBlackBoardValueType_FLOAT4* ValueObj, uint8* RawData, const DirectX::XMFLOAT4& Value);
	static DirectX::XMFLOAT4 GetValue(const FBlackBoardValueType_FLOAT4* ValueObj, const uint8* RawData);

	static uint8* GetRawData(const FBlackBoardValueType_FLOAT4* ValueObj) {return (uint8*)(&ValueObj->Value);}
protected:
	DirectX::XMFLOAT4 Value = InvalidValue;
};

struct FBlackBoardValueType_UINT : public FBlackBoardValueType
{
public:
	~FBlackBoardValueType_UINT() override = default;

	typedef uint32 FDataType;
	static constexpr FDataType InvalidValue = UINT32_MAX;

	static bool SetValue(FBlackBoardValueType_UINT* ValueObj, uint8* RawData, const uint32& Value);
	static uint32 GetValue(const FBlackBoardValueType_UINT* ValueObj, const uint8* RawData);

	static uint8* GetRawData(const FBlackBoardValueType_UINT* ValueObj) {return (uint8*)(&ValueObj->Value);}
protected:
	uint32 Value = UINT32_MAX;
};

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

		const bool bIsValueChanged = TDataClass::SetValue(ClassValue.get(), RawData, Value);
		if (bIsValueChanged && BlackBoardValueObserver.contains(KeyName))
		{
			OnBlackBoardValueChanged.Broadcast();
			BlackBoardValueObserver.clear();
		}
		
		return bIsValueChanged;
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

public:
	void AddBlackBoardValueChangeObserver(const std::string& KeyName)
	{
		BlackBoardValueObserver.emplace(KeyName);
	}

	Delegate<> OnBlackBoardValueChanged;
protected:
	std::map<std::string, std::shared_ptr<FBlackBoardValueType>> BlackBoard;
	std::set<std::string> BlackBoardValueObserver;
};

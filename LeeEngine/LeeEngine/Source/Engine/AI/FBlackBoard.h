#pragma once
#include "CoreMinimal.h"

#include "Engine/Misc/Delegate.h"

enum class EBlackBoardValueType
{
	Bool,
	Int,
	Float,
	Float2,
	Float3,
	Float4,
	Actor,
};

struct FBlackBoardValue
{
	FBlackBoardValue(EBlackBoardValueType InType) : Type(InType) {}
	virtual ~FBlackBoardValue() = default;

	EBlackBoardValueType GetType() const { return Type; }

	void SetKeyValue(EBlackBoardValueType Type, const void* Value);

	void GetKeyValue(EBlackBoardValueType Type, void* OutValue);

protected:
	virtual void Set(const void* InValue) = 0;
	virtual void Get(void* OutValue) = 0;
private:
	EBlackBoardValueType Type;
};

// Bool
struct FBlackBoardBool : FBlackBoardValue
{
	FBlackBoardBool() : FBlackBoardValue(EBlackBoardValueType::Bool) {}
	~FBlackBoardBool() override = default;
private:
	void Set(const void* InValue) override
	{
		if (InValue)
		{
			Value = *(static_cast<const bool*>(InValue));
		}
	}
	void Get(void* OutValue) override
	{
		if (OutValue)
		{
			*(static_cast<bool*>(OutValue)) = Value;
		}
	}
private:
	bool Value = false;
};

// Int
struct FBlackBoardInt : FBlackBoardValue
{
	FBlackBoardInt() : FBlackBoardValue(EBlackBoardValueType::Int) {}
	~FBlackBoardInt() override = default;
private:
	void Set(const void* InValue) override
	{
		if (InValue)
		{
			Value = *(static_cast<const int*>(InValue));
		}
	}
	void Get(void* OutValue) override
	{
		if (OutValue)
		{
			*(static_cast<int*>(OutValue)) = Value;
		}
	}
private:
	int Value = 0;
};

// Float
struct FBlackBoardFloat : FBlackBoardValue
{
	FBlackBoardFloat() : FBlackBoardValue(EBlackBoardValueType::Float) {}
	~FBlackBoardFloat() override = default;
private:
	void Set(const void* InValue) override
	{
		if (InValue)
		{
			Value = *(static_cast<const float*>(InValue));
		}
	}
	void Get(void* OutValue) override
	{
		if (OutValue)
		{
			*(static_cast<float*>(OutValue)) = Value;
		}
	}
private:
	float Value = 0.0f;
};

// Float2
struct FBlackBoardFloat2 : FBlackBoardValue
{
	FBlackBoardFloat2() : FBlackBoardValue(EBlackBoardValueType::Float2) {}
	~FBlackBoardFloat2() override = default;
private:
	void Set(const void* InValue) override
	{
		if (InValue)
		{
			Value = *(static_cast<const XMFLOAT2*>(InValue));
		}
	}
	void Get(void* OutValue) override
	{
		if (OutValue)
		{
			*(static_cast<XMFLOAT2*>(OutValue)) = Value;
		}
	}
private:
	XMFLOAT2 Value;
};

// Float3
struct FBlackBoardFloat3 : FBlackBoardValue
{
	FBlackBoardFloat3() : FBlackBoardValue(EBlackBoardValueType::Float3) {}
	~FBlackBoardFloat3() override = default;
private:
	void Set(const void* InValue) override
	{
		if (InValue)
		{
			Value = *(static_cast<const XMFLOAT3*>(InValue));
		}
	}
	void Get(void* OutValue) override
	{
		if (OutValue)
		{
			*(static_cast<XMFLOAT3*>(OutValue)) = Value;
		}
	}
private:
	XMFLOAT3 Value;
};

// Float4
struct FBlackBoardFloat4 : FBlackBoardValue
{
	FBlackBoardFloat4() : FBlackBoardValue(EBlackBoardValueType::Float4) {}
	~FBlackBoardFloat4() override = default;
private:
	void Set(const void* InValue) override
	{
		if (InValue)
		{
			Value = *(static_cast<const XMFLOAT4*>(InValue));
		}
	}
	void Get(void* OutValue) override
	{
		if (OutValue)
		{
			*(static_cast<XMFLOAT4*>(OutValue)) = Value;
		}
	}
private:
	XMFLOAT4 Value;
};

// AActor
class AActor;
struct FBlackBoardActor : FBlackBoardValue
{
	FBlackBoardActor() : FBlackBoardValue(EBlackBoardValueType::Actor) {}
	~FBlackBoardActor() override = default;
private:
	void Set(const void* InValue) override
	{
		if (InValue)
		{
			Value = static_cast<AActor*>(const_cast<void*>(InValue));
		}
	}
	void Get(void* OutValue) override
	{
		if (OutValue)
		{
			*static_cast<AActor**>(OutValue) = Value; 
		}
	}
private:
	AActor* Value = nullptr;
};

class FBlackBoard final
{
public:
	void AddKeyValue(const std::string& Key, const void* InitValue, EBlackBoardValueType Type);
	bool GetBlackBoardValue(const std::string& Key, void* OutValue, EBlackBoardValueType Type);
	void ChangeValue(const std::string& Key, const void* ChangeValue, EBlackBoardValueType Type);
protected:
	static std::shared_ptr<FBlackBoardValue> MakeBlackBoardValueByType(EBlackBoardValueType Type)
	{
		switch (Type)
		{
		case EBlackBoardValueType::Bool:
			return std::make_shared<FBlackBoardBool>();
		case EBlackBoardValueType::Float:
			return std::make_shared<FBlackBoardFloat>();
		case EBlackBoardValueType::Float2:
			return std::make_shared<FBlackBoardFloat2>();
		case EBlackBoardValueType::Float3:
			return std::make_shared<FBlackBoardFloat3>();
		case EBlackBoardValueType::Float4:
			return std::make_shared<FBlackBoardFloat4>();
		case EBlackBoardValueType::Int:
			return std::make_shared<FBlackBoardInt>();
		case EBlackBoardValueType::Actor:
			return std::make_shared<FBlackBoardActor>();
		default:
			break;
		}
		assert(nullptr && "잘못된 타입");
		return nullptr;
	}
public:
	Delegate<> OnBlackBoardValueChanged;
private:
	std::map<std::string, std::shared_ptr<FBlackBoardValue>> BlackBoard;
};
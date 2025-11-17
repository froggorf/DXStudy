#include "CoreMinimal.h"
#include "FBlackBoard.h"

bool FBlackBoardValueType_Bool::SetValue(FBlackBoardValueType_Bool* ValueObj, uint8* RawData, const bool& bValue)
{
	return SetValueInMemory<uint8>(RawData, bValue);
}

bool FBlackBoardValueType_Bool::GetValue(const FBlackBoardValueType_Bool* ValueObj, const uint8* RawData)
{
	return GetValueFromMemory<uint8>(RawData) != 0;
}


bool FBlackBoardValueType_Int::SetValue(FBlackBoardValueType_Int* ValueObj, uint8* RawData, const int& Value)
{
	return SetValueInMemory<int>(RawData, Value);
}

int FBlackBoardValueType_Int::GetValue(const FBlackBoardValueType_Int* ValueObj, const uint8* RawData)
{
	return GetValueFromMemory<int>(RawData);
}

bool FBlackBoardValueType_FLOAT::SetValue(FBlackBoardValueType_FLOAT* ValueObj, uint8* RawData, const float& Value)
{
	return SetValueInMemory<float>(RawData, Value);
}

int FBlackBoardValueType_FLOAT::GetValue(const FBlackBoardValueType_FLOAT* ValueObj, const uint8* RawData)
{
	return GetValueFromMemory<float>(RawData);
}

const DirectX::XMFLOAT2 FBlackBoardValueType_FLOAT2::InvalidValue = DirectX::XMFLOAT2(FLT_MAX, FLT_MAX);

bool FBlackBoardValueType_FLOAT2::SetValue(FBlackBoardValueType_FLOAT2* ValueObj, uint8* RawData, const DirectX::XMFLOAT2& Value)
{
	return SetValueInMemory<DirectX::XMFLOAT2>(RawData, Value);
}

DirectX::XMFLOAT2 FBlackBoardValueType_FLOAT2::GetValue(const FBlackBoardValueType_FLOAT2* ValueObj, const uint8* RawData)
{
	return GetValueFromMemory<DirectX::XMFLOAT2>(RawData);
}

const DirectX::XMFLOAT3 FBlackBoardValueType_FLOAT3::InvalidValue = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);

bool FBlackBoardValueType_FLOAT3::SetValue(FBlackBoardValueType_FLOAT3* ValueObj, uint8* RawData, const DirectX::XMFLOAT3& Value)
{
	return SetValueInMemory<DirectX::XMFLOAT3>(RawData, Value);
}

DirectX::XMFLOAT3 FBlackBoardValueType_FLOAT3::GetValue(const FBlackBoardValueType_FLOAT3* ValueObj, const uint8* RawData)
{
	return GetValueFromMemory<DirectX::XMFLOAT3>(RawData);
}

const DirectX::XMFLOAT4 FBlackBoardValueType_FLOAT4::InvalidValue = DirectX::XMFLOAT4(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

bool FBlackBoardValueType_FLOAT4::SetValue(FBlackBoardValueType_FLOAT4* ValueObj, uint8* RawData, const DirectX::XMFLOAT4& Value)
{
	return SetValueInMemory<DirectX::XMFLOAT4>(RawData, Value);
}

DirectX::XMFLOAT4 FBlackBoardValueType_FLOAT4::GetValue(const FBlackBoardValueType_FLOAT4* ValueObj, const uint8* RawData)
{
	return GetValueFromMemory<DirectX::XMFLOAT4>(RawData);
}

bool FBlackBoardValueType_UINT::SetValue(FBlackBoardValueType_UINT* ValueObj, uint8* RawData, const uint32& Value)
{
	return SetValueInMemory<uint32>(RawData, Value);
}

uint32 FBlackBoardValueType_UINT::GetValue(const FBlackBoardValueType_UINT* ValueObj, const uint8* RawData)
{
	return GetValueFromMemory<uint32>(RawData);
}

const FBlackBoardValueType_Object::FDataType FBlackBoardValueType_Object::InvalidValue = nullptr;

bool FBlackBoardValueType_Object::SetValue(FBlackBoardValueType_Object* ValueObj, uint8* RawData, const std::shared_ptr<UObject>& Value)
{
	return SetWeakObjectInMemory<UObject>(RawData, Value);
}

std::shared_ptr<UObject> FBlackBoardValueType_Object::GetValue(const FBlackBoardValueType_Object* ValueObj, const uint8* RawData)
{
	return ValueObj->Value.lock();
}
#include "CoreMinimal.h"
#include "FBlackBoard.h"

#include "Engine/RenderCore/EditorScene.h"

void FBlackBoardValue::SetKeyValue(EBlackBoardValueType Type, const void* Value)
{
	if (GetType() != Type)
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Error, "블랙보드 키 타입이 맞지 않습니다.");
		return;
	}
	Set(Value);
}

void FBlackBoardValue::GetKeyValue(EBlackBoardValueType Type, void* OutValue)
{
	if (GetType() != Type)
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Error, "블랙보드 키 타입이 맞지 않습니다.");
		return;
	}
	Get(OutValue);
}

void FBlackBoard::AddKeyValue(const std::string& Key, const void* InitValue, EBlackBoardValueType Type)
{
	auto Pair = BlackBoard.insert(std::make_pair(Key, MakeBlackBoardValueByType(Type)));
	if (Pair.second)
	{
		Pair.first->second->SetKeyValue(Type, InitValue);
	}
}

bool FBlackBoard::GetBlackBoardValue(const std::string& Key, void* OutValue, EBlackBoardValueType Type)
{
	auto Iter = BlackBoard.find(Key);
	if (Iter != BlackBoard.end())
	{
		Iter->second->GetKeyValue(Type, OutValue);
		return true;
	}
	return false;
}

void FBlackBoard::ChangeValue(const std::string& Key, const void* ChangeValue, EBlackBoardValueType Type)
{
	auto Iter = BlackBoard.find(Key);
	if (Iter != BlackBoard.end())
	{
		Iter->second->SetKeyValue(Type, ChangeValue);
		OnBlackBoardValueChanged.Broadcast();
	}
}

#pragma once
#include "Engine/UObject/UObject.h"

class UGameInstance : public UObject
{
	MY_GENERATE_BODY(UGameInstance)

	UGameInstance() = default;
	~UGameInstance() override = default;

	virtual void LoadInitialData() {}

    template<typename T = UGameInstance>
    static T* GetInstance()
    {
        static_assert(std::is_base_of_v<UGameInstance, T>, "T must derive from UGameInstance");

        static std::unique_ptr<T> Instance = nullptr;
        if (!Instance)
        {
            Instance = std::make_unique<T>();
        }
        return Instance.get();
    }
};
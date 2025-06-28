
#pragma once
#include "CoreMinimal.h"

// 델리게이트
template<typename... Args>
class Delegate
{
public:
	template<typename T>
	void Add(T* Obj, void (T::*Func)(Args...))
	{
		Callbacks.push_back([Obj, Func](Args... args) {
			(Obj->*Func)(args...);
			});
	}

	void Broadcast(Args... args)
	{
		for (auto& func : Callbacks)
			func(args...);
	}

private:
	std::vector<std::function<void(Args...)>> Callbacks;
};
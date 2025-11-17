
#pragma once
#include "CoreMinimal.h"

// 델리게이트
template<typename... Args>
class Delegate
{
public:
	Delegate() = default;
	
	template<typename T>
	Delegate(T* Obj, void (T::*Func)(Args...))
	{
		Add(Obj, Func);
	}

	template<typename T>
	void Add(T* Obj, void (T::*Func)(Args...))
	{
		Callbacks.push_back([Obj, Func](Args... args) {
			(Obj->*Func)(args...);
			});
	}

	void Add(std::function<void(Args...)> Func)
	{
		Callbacks.emplace_back(Func);
	}

	void Broadcast(Args... args)
	{
		for (auto& func : Callbacks)
			func(args...);
	}

	bool operator!() const
	{
		return Callbacks.empty();
	}

private:
	std::vector<std::function<void(Args...)>> Callbacks;
};
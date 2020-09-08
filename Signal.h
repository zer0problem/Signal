// requires C++17
// https://github.com/zer0problem/Signal/
#pragma once
#include <vector>
#include <type_traits>
#include <functional>

template<class ...ArgumentTypes>
class Signal
{
public:
	template<auto Callback, class SourceType>
	void Connect(SourceType *aSource)
	{
		mySubscribers.push_back(TemplateFunctionPair(reinterpret_cast<VoidFunctionPointer>(&MemberFunction<Callback, SourceType>), aSource));
	}
	template<auto Callback>
	void Connect()
	{
		mySubscribers.push_back(TemplateFunctionPair(reinterpret_cast<VoidFunctionPointer>(&GlobalFunction<Callback>), nullptr));
	}
	template<class CallbackType>
	void Connect(CallbackType *aCallback)
	{
		static_assert(std::is_invocable<CallbackType, ArgumentTypes...>::value, "CallbackType is not a pointer to an invocable type");
		mySubscribers.push_back(TemplateFunctionPair(reinterpret_cast<VoidFunctionPointer>(&FunctorFunction<CallbackType>), aCallback));
	}
	
	template<auto Callback, class SourceType>
	void Disconnect(SourceType *aSource)
	{
		for (auto it = mySubscribers.begin(); it != mySubscribers.end(); it++)
		{
			if (reinterpret_cast<VoidFunctionPointer>(&MemberFunction<Callback, SourceType>) == it->first && it->second == aSource)
			{
				mySubscribers.erase(it);
				break;
			}
		}
	}
	template<auto Callback>
	void Disconnect()
	{
		for (auto it = mySubscribers.begin(); it != mySubscribers.end(); it++)
		{
			if (reinterpret_cast<VoidFunctionPointer>(&GlobalFunction<Callback>) == it->first)
			{
				mySubscribers.erase(it);
				break;
			}
		}
	}
	template<class CallbackType>
	void Disconnect(CallbackType *aCallback)
	{
		static_assert(std::is_invocable<CallbackType, ArgumentTypes...>::value, "CallbackType is not of an invocable type");
		for (auto it = mySubscribers.begin(); it != mySubscribers.end(); it++)
		{
			if (reinterpret_cast<VoidFunctionPointer>(&FunctorFunction<CallbackType>) == it->first && it->second == aCallback)
			{
				mySubscribers.erase(it);
				break;
			}
		}
	}
	void Emit(ArgumentTypes ...aArguments)
	{
		for (auto it = mySubscribers.begin(); it != mySubscribers.end(); it++)
		{
			(*it).first((*it).second, aArguments...);
		}
	}
private:
	using VoidFunctionPointer = void(*)(void *, ArgumentTypes...);
	using TemplateFunctionPair = std::pair<VoidFunctionPointer, void *>;
	std::vector<TemplateFunctionPair> mySubscribers;

	template<auto Callback, class SourceType>
	static void MemberFunction(SourceType *aSource, ArgumentTypes... aArguments)
	{
		std::invoke(Callback, aSource, aArguments...);
	}
	template<auto Callback>
	static void GlobalFunction(void *, ArgumentTypes... aArguments)
	{
		std::invoke(Callback, aArguments...);
	}
	template<class CallbackType>
	static void FunctorFunction(CallbackType *aFunctor, ArgumentTypes... aArguments)
	{
		std::invoke(*aFunctor, aArguments...);
	}
};

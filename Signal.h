// requires C++17
// https://github.com/zer0problem/Signal/
#pragma once
#include <vector>
#include <functional>

template<class ...ArgumentTypes>
class Signal
{
public:
	// matches the correct private Connect function, allows only sending the function in as a template argument instead of both Type, Function* and Source*
	template<auto Callback, class SourceType>
	inline void Connect(SourceType *aSource)
	{
		Connect<SourceType, Callback>(aSource);
	}
	// directly adds a sourceless or global function
	template<void (*TemplateFunction)(ArgumentTypes...)>
	void Connect()
	{
		mySubscribers.push_back(TemplateFunctionPair(reinterpret_cast<VoidFunctionPointer>(&GlobalFunction<TemplateFunction>), nullptr));
	}
	// matches the correct private Disconnect function, just like the Connect function
	template<auto Callback, class SourceType>
	inline void Disconnect(SourceType *aSource)
	{
		Disconnect<SourceType, Callback>(aSource);
	}
	// searches for and removes the first matching Connection of the type
	template<void (*TemplateFunction)(ArgumentTypes...)>
	void Disconnect()
	{
		for(auto it = mySubscribers.begin(); it != mySubscribers.end(); it++)
		{
			if(reinterpret_cast<VoidFunctionPointer>(&GlobalFunction<TemplateFunction>) == it->first)
			{
				mySubscribers.erase(it);
				break;
			}
		}
	}
	void Emit(ArgumentTypes ...aArguments)
	{
		for(auto it = mySubscribers.begin(); it != mySubscribers.end(); it++)
		{
			(*it).first((*it).second, aArguments...);
		}
	}
private:
	using VoidFunctionPointer = void(*)(void *, ArgumentTypes...);
	using TemplateFunctionPair = std::pair<VoidFunctionPointer, void *>;
	std::vector<TemplateFunctionPair> mySubscribers;

	// these 3 template functions that the function pair is gonna point at, takes and resolves the correct function at compiling, avoiding vtables for virtual classes to target the correct function
	template<class SourceType, void (SourceType:: *TemplateFunction)(ArgumentTypes...)>
	static void MemberFunction(SourceType *aSource, ArgumentTypes... aArguments)
	{
		(aSource->*TemplateFunction)(aArguments...);
	}
	template<class SourceType, void (*TemplateFunction)(SourceType *, ArgumentTypes...)>
	static void StaticFunction(SourceType *aSource, ArgumentTypes... aArguments)
	{
		(*TemplateFunction)(aSource, aArguments...);
	}
	template<void (*TemplateFunction)(ArgumentTypes...)>
	static void GlobalFunction(void *, ArgumentTypes... aArguments)
	{
		(*TemplateFunction)(aArguments...);
	}
	
	// these 2 matches appropriate template function according to what type is sent in and adds the pointer to the function and the object
	template<class SourceType, void (SourceType:: *TemplateFunction)(ArgumentTypes...)>
	void Connect(SourceType *aSource)
	{
		mySubscribers.push_back(TemplateFunctionPair(reinterpret_cast<VoidFunctionPointer>(&MemberFunction<SourceType, TemplateFunction>), aSource));
	}
	template<class SourceType, void (*TemplateFunction)(SourceType *, ArgumentTypes...)>
	void Connect(SourceType *aSource)
	{
		mySubscribers.push_back(TemplateFunctionPair(reinterpret_cast<VoidFunctionPointer>(&StaticFunction<SourceType, TemplateFunction>), aSource));
	}
	// these 2 matches appropriate template function according to what type is sent in and removes a matching pointer to the function and object
	template<class SourceType, void (SourceType:: *TemplateFunction)(ArgumentTypes...)>
	void Disconnect(SourceType *aSource)
	{
		for(auto it = mySubscribers.begin(); it != mySubscribers.end(); it++)
		{
			if(reinterpret_cast<VoidFunctionPointer>(&MemberFunction<SourceType, TemplateFunction>) == it->first && it->second == aSource)
			{
				mySubscribers.erase(it);
				break;
			}
		}
	}
	template<class SourceType, void (*TemplateFunction)(SourceType *, ArgumentTypes...)>
	void Disconnect(SourceType *aSource)
	{
		for(auto it = mySubscribers.begin(); it != mySubscribers.end(); it++)
		{
			if(reinterpret_cast<VoidFunctionPointer>(&StaticFunction<SourceType, TemplateFunction>) == it->first && it->second == aSource)
			{
				mySubscribers.erase(it);
				break;
			}
		}
	}
};

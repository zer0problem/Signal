// requires C++17
// https://github.com/zer0problem/Signal/
#pragma once
#include <algorithm>
#include <vector>
#include <functional>

template<class ...ArgumentTypes>
class Signal {
public:
	template<class SourceType>
	void Connect(SourceType *aSource, void(SourceType:: *aCallback)(ArgumentTypes ...)) {
		mySubscribers.push_back(DataObject());
		new(&mySubscribers[mySubscribers.size() - 1]) MemberWrapper<SourceType>(aSource, aCallback);
	};
	template<class SourceType>
	void Connect(SourceType *aSource, void(*aCallback)(SourceType *, ArgumentTypes ...)) {
		mySubscribers.push_back(DataObject());
		new(&mySubscribers[mySubscribers.size() - 1]) StaticWrapper<SourceType>(aSource, aCallback);
	};
	void Connect(void(*aCallback)(ArgumentTypes ...)) {
		mySubscribers.push_back(DataObject());
		new(&mySubscribers[mySubscribers.size() - 1]) GlobalWrapper(aCallback);
	};
	template<class SourceType>
	void Disconnect(SourceType *aSource, void(SourceType:: *aCallback)(ArgumentTypes ...)) {
		for(auto it = mySubscribers.begin(); it != mySubscribers.end(); it++) {
			MemberWrapper<SourceType> *wrapper = reinterpret_cast<MemberWrapper<SourceType> *>(&(*it));
			if(wrapper->mySource == aSource && wrapper->myCallback == aCallback) {
				mySubscribers.erase(it);
				break;
			}
		}
	};
	template<class SourceType>
	void Disconnect(SourceType *aSource, void(*aCallback)(SourceType *, ArgumentTypes ...)) {
		for(auto it = mySubscribers.begin(); it != mySubscribers.end(); it++) {
			StaticWrapper<SourceType> *wrapper = reinterpret_cast<StaticWrapper<SourceType> *>(&(*it));
			if(wrapper->mySource == aSource && wrapper->myCallback == aCallback) {
				mySubscribers.erase(it);
				break;
			}
		}
	};
	void Disconnect(void(*aCallback)(ArgumentTypes ...)) {
		for(auto it = mySubscribers.begin(); it != mySubscribers.end(); it++) {
			GlobalWrapper *wrapper = reinterpret_cast<GlobalWrapper *>(&(*it));
			if(wrapper->myCallback == aCallback) {
				mySubscribers.erase(it);
				break;
			}
		}
	};
	void Emit(ArgumentTypes ...aArguments) {
		for(auto it = mySubscribers.begin(); it != mySubscribers.end(); it++) {
			(*reinterpret_cast<BaseWrapper *>(&(*it)))(aArguments...);
		}
	}
private:
	class BaseWrapper {
	public:
		virtual void operator()(ArgumentTypes...) = 0;
	};
	template<class SourceType>
	class MemberWrapper : public BaseWrapper {
	public:
		MemberWrapper(SourceType *aSource, void(SourceType:: *aCallback)(ArgumentTypes ...)) {
			mySource = aSource;
			myCallback = aCallback;
		}
		virtual void operator()(ArgumentTypes... aArguments) override {
			std::invoke(myCallback, mySource, aArguments...);
		}
		SourceType *mySource;
		void(SourceType:: *myCallback)(ArgumentTypes ...);
	};
	template<class SourceType>
	class StaticWrapper : public BaseWrapper {
	public:
		StaticWrapper(SourceType *aSource, void(*aCallback)(SourceType *, ArgumentTypes ...)) {
			mySource = aSource;
			myCallback = aCallback;
		}
		virtual void operator()(ArgumentTypes... aArguments) override {
			std::invoke(myCallback, mySource, aArguments...);
		}
		SourceType *mySource;
		void(*myCallback)(SourceType *, ArgumentTypes ...);
	};
	class GlobalWrapper : public BaseWrapper {
	public:
		GlobalWrapper(void(*aCallback)(ArgumentTypes ...)) {
			myCallback = aCallback;
		}
		virtual void operator()(ArgumentTypes... aArguments) override {
			std::invoke(myCallback, aArguments...);
		}
		void(*myCallback)(ArgumentTypes ...);
	};
	struct DataObject {
		char myData[std::max(sizeof(MemberWrapper<Signal>), std::max(sizeof(StaticWrapper<Signal>), sizeof(GlobalWrapper)))];
	};
	std::vector<DataObject> mySubscribers;
};

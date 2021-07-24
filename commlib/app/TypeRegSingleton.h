

#ifndef __TYPE_REG_SINGLETON_H__
#define __TYPE_REG_SINGLETON_H__

#include <string>
#include <functional>
#include <map>
#include "../basic/CommonSingleton.h"

template<class BaseClass>
class TypeRegContainer
{
	using CbFunc = std::function<BaseClass* ()>;
	friend CWSLib::CommSingleton<TypeRegContainer>;
public:
	CbFunc create(const std::string& className)
	{
		auto iter = cbFuncMap.find(className);
		if (iter != cbFuncMap.end())
		{
			return iter->second;
		}
		else
		{
			DEBUG_LOG("container size:[%u]", cbFuncMap.size());
			return nullptr;
		}
	}

	void join(const std::string& className, CbFunc __cb)
	{
		cbFuncMap.insert(std::make_pair(className, __cb));
	}

private:
	TypeRegContainer() {}
	TypeRegContainer(TypeRegContainer& alFac) {}

	std::map<std::string, CbFunc> cbFuncMap;
};

template<class BaseClass>
class TypeFactory
{
	using CbFunc = std::function<BaseClass*()>;
	using CTypeRegContainer = CWSLib::CommSingleton<TypeRegContainer<BaseClass>>;
	friend CWSLib::CommSingleton<TypeFactory>;
public:
	BaseClass* create(const std::string& className)
	{
		CbFunc funcPtr = CTypeRegContainer::instance()->create(className);
		if (funcPtr == nullptr)
		{
			return nullptr;
		}
		return funcPtr();
	}

	BaseClass* create()
	{
		return create(m_defaultTypeName);
	}

	void join(const std::string& className, CbFunc __cb, bool setDefault)
	{
		if (setDefault)
		{
			m_defaultTypeName = className;
		}
		CTypeRegContainer::instance()->join(className, __cb);
	}

private:
	TypeFactory() {}
	TypeFactory(TypeFactory& alFac) {}

private:
	std::string m_defaultTypeName;
};


template <typename BaseType, typename T, bool isDefault>
class TypeRegister
{
private:
    struct Meta
    {
        Meta() {TypeRegister::AddFunction();}
        void DoNothing() {}
    };
    static Meta meta;
public:
    static void AddFunction()
    {
        //NORMAL_LOG("Adding " #__cb_func);
        CWSLib::CommSingleton<TypeFactory<BaseType>>::instance()->join(T::name, getInstance, isDefault);
        meta.DoNothing();
    }
    static BaseType* getInstance()
    {
        return new T;
    }
};
template <typename BaseType, typename T, bool isDefault>
typename TypeRegister<BaseType, T, isDefault>::Meta TypeRegister<BaseType, T, isDefault>::meta;

#define REG_TYPE(BaseType, TypeName, __cb_func, is_default) \
class TypeRegCaller ## TypeName {\
private:\
	class Meta {\
	public:\
		Meta() {\
			TypeRegCaller ## TypeName::AddFunction();\
		}\
		void DoNothing() {}\
	};\
	static Meta m_meta;\
\
public:\
	static void AddFunction() {\
		NORMAL_LOG("Adding " #__cb_func); \
		CWSLib::CommSingleton<TypeFactory<BaseType>>::instance()->join(#__cb_func, getInstance, is_default);\
		m_meta.DoNothing();\
	} \
	static BaseType* getInstance() {\
		return new TypeName;\
	}\
};\
TypeRegCaller ## TypeName::Meta TypeRegCaller ## TypeName::m_meta

//#define REG_TYPE(BaseType, TypeName, __cb_func) static TypeRegCaller<BaseType, TypeName> caller_##__cb_func(#__cb_func)
//#define REG_TYPE_DEFAULT(BaseType, TypeName, __cb_func) static TypeRegCaller<BaseType, TypeName> caller_##__cb_func(#__cb_func, true)

#endif // !__TYPE_REG_SINGLETON_H__


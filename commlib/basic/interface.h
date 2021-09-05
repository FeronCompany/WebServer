#ifndef INTERFACE_H
#define INTERFACE_H
#include <functional>
#include <map>
#include <string>
#include <memory>
#include <tuple>
namespace CWSLib {

namespace InterfaceSpace {

template<typename ...T>
class Interface : public T...{};

class Method {};

template<typename TR, typename ...TArgs>
class MethodImpl : public Method
{
public:
    explicit MethodImpl(const std::string &name_, const std::function<TR(TArgs...)>& func_):name(name_),func(func_){}
    std::string name;
    std::function<TR(TArgs...)> func;
};

class Interfaces
{
public:
    template<typename TR, typename ...TArgs>
    void registerMethod(const std::string &name, const std::function<TR(TArgs...)>& func)
    {
        methods[name] = std::shared_ptr<Method>(new MethodImpl<TR, TArgs...>(name, func));
        return;
    }
    std::shared_ptr<Method> operator[](const std::string &name)
    {
        if (methods.find(name) == methods.end()) {
            return nullptr;
        }
        return methods[name];
    }
    template<typename TR, typename ...TArgs>
    TR invoke(const std::string &name, TArgs&& ...args)
    {
        return static_cast<MethodImpl<TR, TArgs...>*>(methods[name].get())->func(std::forward<TArgs>(args)...);
    }
private:
    std::map<std::string, std::shared_ptr<Method> > methods;
};
}
}
#endif // INTERFACE_H

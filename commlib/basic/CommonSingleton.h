#ifndef __COMMON_SINGLETON_H__
#define __COMMON_SINGLETON_H__

#include <memory>
#include <map>
#include <string>

namespace CWSLib
{
	template<typename T>
	class CommSingleton
	{
	private:
		class SingleQuaran
		{
		public:
			SingleQuaran() { (void)CommSingleton<T>::instance(); }
			void doNothing() {}
		};

	public:
		static T* instance()
		{
			static T inst;
			m_quaran.doNothing();
			return &inst;
		}

	private:
		static SingleQuaran m_quaran;

		CommSingleton() {}
		CommSingleton(CommSingleton& comm) {}
	};
    template<typename T>
	typename CommSingleton<T>::SingleQuaran CommSingleton<T>::m_quaran;

	template<typename T>
	class SingletonFactory
	{
		class UniqueFactory
		{
		public:
			static UniqueFactory& Get()
			{
				static UniqueFactory factory;
				return factory;
			}
			template<typename ...Args>
			void Create(Args&&... args)
			{
				m_instance.reset(new T(std::forward<Args>(args)...));
			}
			std::shared_ptr<T> Instance()
			{
				return m_instance;
			}
		private:
			std::shared_ptr<T> m_instance;
		};
		
		class MultiFactory
		{
			static MultiFactory& Get()
			{
				static MultiFactory factory;
				return factory;
			}	
			template<typename ...Args>
			void Create(const std::string& name, Args&&... args)
			{
				m_instanceMap[name].reset(new T(std::forward<Args>(args)...));
			}
			std::shared_ptr<T> Instance(const std::string& name)
			{
				return m_instanceMap[name];
			}	
		private:
			std::map<std::string, std::shared_ptr<T>> m_instanceMap;
		};
		
	public:
		template<typename ...Args>
		static void CreateUnique(Args&&... args)
		{
			auto& factory = UniqueFactory::Get();
			factory.Create(std::forward<Args>(args)...);
		}
		template<typename ...Args>
		static void Create(const std::string& name, Args&&... args)
		{
			auto& factory = MultiFactory::Get();
			factory.Create(name, std::forward<Args>(args)...);
		}
		static std::shared_ptr<T> Instance()
		{
			auto& factory = UniqueFactory::Get();
			return factory.Instance();
		}
		static std::shared_ptr<T> Instance(const std::string& name)
		{
			auto& factory = MultiFactory::Get();
			return factory.Instance(name);
		}
	};
}

#endif // !__COMMON_SINGLETON_H__


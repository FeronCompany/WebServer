#ifndef __COMMON_SINGLETON_H__
#define __COMMON_SINGLETON_H__

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
}

#endif // !__COMMON_SINGLETON_H__


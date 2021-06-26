#pragma once
namespace Basic {

	template< class T >
	class Singleton
	{
	public:
		Singleton() = default;

	public:
		static auto Instance() -> T&
		{
			static T instance;
			return instance;
		}

	protected:
		Singleton(const Singleton&) = delete;
		auto operator = (const Singleton&)->Singleton& = delete;
	};

}
#pragma once
#include <Windows.h>
#include <chrono>
namespace Basic {

	class Timer {
	public:
		Timer() = default;

	public:
		void Reset() {
			m_start = GetTickCount64();
		}

		unsigned long long Elapsed() {
			return GetTickCount64() - m_start;
		}


		unsigned long long m_start;
	};

};
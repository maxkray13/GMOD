#pragma once
#include "../../../include/stdafx.h"
#include "BasicModuleInfo.h"
#include "Pattern.h"
#include "VTHook.h"



namespace Core {
	namespace Memory {


		template<class T>
		inline T vfunc(void* p_table, unsigned int index) {
			auto procedure_array = *reinterpret_cast<std::uintptr_t**>(p_table);
			return (T)procedure_array[index];
		}

		template< typename T >
		inline T read(std::uintptr_t address) {
			return *(T*)address;
		}

		template< typename T >
		inline T write(std::uintptr_t address, const T& data) {
			*(T*)address = data;
		}

		unsigned long follow_to(DWORD src);
		unsigned long get_imm(DWORD src);
		unsigned long get_disp(DWORD src);


	}

}
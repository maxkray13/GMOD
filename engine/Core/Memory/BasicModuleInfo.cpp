#include "../../../include/stdafx.h"
#include "BasicModuleInfo.h"
namespace Core {
	namespace Memory {
		BasicModuleInfo::BasicModuleInfo(LPCSTR lpModuleName)
		{
			DWORD hDLL = 0; do {
				hDLL = reinterpret_cast<DWORD>(GetModuleHandleA(lpModuleName));
				Sleep(100);
			} while (!hDLL);

			this->StartAddr = hDLL;
			this->Size = PIMAGE_NT_HEADERS(this->StartAddr + (DWORD)PIMAGE_DOS_HEADER(this->StartAddr)->e_lfanew)->OptionalHeader.SizeOfImage;
			this->EndAddr = this->StartAddr + this->Size;
		}
		DWORD BasicModuleInfo::GetStartAddr()
		{
			return StartAddr;
		}
		DWORD BasicModuleInfo::GetSize()
		{
			return Size;
		}
		DWORD BasicModuleInfo::GetEndAddr()
		{
			return EndAddr;
		}
	}
}


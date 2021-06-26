#pragma once

namespace Core {
	namespace Memory {
		class BasicModuleInfo {
		public:
			BasicModuleInfo() = default;
			BasicModuleInfo(LPCSTR lpModuleName);
			DWORD GetStartAddr();
			DWORD GetSize();
			DWORD GetEndAddr();
		private:
			DWORD StartAddr;
			DWORD Size;
			DWORD EndAddr;
		};
	}
}
#pragma once
namespace Core {
	namespace Memory {
		//DarthTon
		void FillShiftTable(const uint8_t * pPattern, size_t patternSize, uint8_t wildcard, size_t * bad_char_skip);
		const void * Search(const uint8_t * pScanPos, size_t scanSize, const uint8_t * pPattern, size_t patternSize, uint8_t wildcard);
		LPVOID FindPattern(PBYTE baseAddress, DWORD size, const char * pattern, uint8_t UnkByte);
	}
}
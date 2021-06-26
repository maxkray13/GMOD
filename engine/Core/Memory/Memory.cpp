#include "Memory.h"


namespace Core {
	namespace Memory {
		DWORD follow_to(DWORD src)
		{
			HDE_STRUCT de32;
			DWORD len = hde_disasm((uint32_t*)src, &de32);
			DWORD rel = 0u;
			if (de32.rel8)
				rel = de32.rel8;
			if (de32.rel16)
				rel = de32.rel16;
			if (de32.rel32)
				rel = de32.rel32;
			return src + (rel + len);
		}
		unsigned long get_imm(DWORD src)
		{
			HDE_STRUCT de32;
			hde_disasm((uint32_t*)src, &de32);
			DWORD imm = 0u;
			if (de32.imm8)
				imm = de32.imm8;
			if (de32.imm16)
				imm = de32.imm16;
			if (de32.imm32)
				imm = de32.imm32;
			return imm;
		}
		DWORD get_disp(DWORD src)
		{
			HDE_STRUCT de32;
			hde_disasm((uint32_t*)src, &de32);
			DWORD disp = 0u;
			if (de32.disp8)
				disp = de32.disp8;
			if (de32.disp16)
				disp = de32.disp16;
			if (de32.disp32)
				disp = de32.disp32;
			return disp;
		}
	}
}
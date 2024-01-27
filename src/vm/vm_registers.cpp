#include "vmachine.h"

namespace vm
{
	void clear(Tregisters& registers)
	{
		for (int i = 0; i < 16; i++) {
			registers.r[i] = 0;
			registers.fl[i] = 0;
		}

		registers.cr = 0;
		registers.cf = 0;
		registers.zf = 0;
		registers.pc = 0;
	}
}
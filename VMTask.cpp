#include "VMTask.h"

VMTask::VMTask(VMachine* _vm)
{
	vm = _vm;
}

void VMTask::operator()()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	//vm->interpret("orbitalProgram1.oasm");
	vm->interpret("ballisticProgram.oasm");
	std::cout << "VM thread executed";
}

VMTask::~VMTask()
{
	
}

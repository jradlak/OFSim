#include <thread>
#include "VMachine.h"

class VMTask 
{
public:
	VMTask(VMachine* _vm);
	
	void operator()();

	~VMTask();

private:
	std::thread* vmThread;
	VMachine* vm;
};

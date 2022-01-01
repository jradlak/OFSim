#pragma once

#include "VMachine.h"

VMachine::VMachine()
{
	memory = new Memory;
	registers = new Registers;
}

void VMachine::interrupt(short code)
{
}

VMachine::~VMachine()
{
	delete memory;
	delete registers;
}


void VMachine::interruptHandler()
{
}

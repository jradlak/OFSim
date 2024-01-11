#pragma once
class Task
{
	virtual void init() = 0;

	virtual void start() = 0;

	virtual void stop() = 0;

	virtual void restart() = 0;
};


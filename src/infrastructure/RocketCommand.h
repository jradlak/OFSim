#pragma once

class RocketCommand
{
public:
	RocketCommand(int code, double value) { _code = code; _value = value; }
	inline int code() { return _code; }
	inline double value() { return _value; }

private:
	int _code;

	double _value;
};
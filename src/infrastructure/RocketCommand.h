#pragma once

class RocketCommand
{
public:
	RocketCommand(int code, double value) { _code = code; _value = value; }
	inline int code() const { return _code; }
	inline double value() const { return _value; }

private:
	int _code;
	double _value;
};
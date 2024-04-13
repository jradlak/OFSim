#pragma once

enum class RocketCommandCode : unsigned int
{
	THRUST_MAGNITUDE_CHANGE = 1,
	THRUST_ROTATION_CHANGE_X,
	THRUST_ROTATION_CHANGE_Y,
	THRUST_ROTATION_CHANGE_Z,
	GYRO_ROTATION_CHANGE_X,
	GYRO_ROTATION_CHANGE_Y,
    GYRO_ROTATION_CHANGE_Z
};

class RocketCommand
{
public:
	RocketCommand(RocketCommandCode code, double value) { _code = code; _value = value; }
	inline RocketCommandCode code() const { return _code; }
	inline double value() const { return _value; }

private:
	RocketCommandCode _code;
	double _value;
};

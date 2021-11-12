#pragma once
#include <vector>;

class Rocket
{
public:
	Rocket();

	//void init();

	~Rocket();
private:
	std::vector<float> getVertices();
	std::vector<int> getIndices();
};


#include "MathHelper.h"

// Simple Helper, Rotates only on the Y-Axes
vector<float> MathHelper::rotateVector(float angle, float value[])
{
	vector<float> rotated;

	rotated.push_back(value[2]*cos(angle) - value[0]*sin(angle));// z
	rotated.push_back(value[1]);// y
	rotated.push_back(value[2]*sin(angle) + value[0]*cos(angle));// x

	return rotated;
}

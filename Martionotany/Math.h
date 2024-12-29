#pragma once
#include "Include.h"

vec2 SNormalize(vec2 original)
{
	if (original == vec2(0))
		return vec2(0);
	return normalize(original);
}

float TryAddF(const float original, float additional, float maximum)
{
	additional += original;
	maximum = std::max(maximum, std::abs(original));
	if (additional != 0 && std::abs(additional) > maximum)
		return glm::sign(additional) * maximum;
	return additional;
}

vec2 TryAddVec2(const vec2 original, vec2 additional, float maximum)
{
	additional += original;
	maximum = std::max(maximum, glm::length(original));
	if (additional != vec2(0) && glm::length2(additional) > maximum * maximum)
		return glm::normalize(additional) * maximum;
	return additional;
}

float TrySubF(const float original, const float subtractional)
{
	if (original == 0)
		return original;
	float result = original - glm::sign(original) * subtractional;
	if (original * result > 0)
		return result;
	return 0;
}

vec2 TrySubVec2(const vec2 original, const float subtractional)
{
	if (original == vec2(0))
		return original;
	vec2 result = original - glm::normalize(original) * subtractional;
	if (glm::dot(original, result) > 0)
		return result;
	return vec2(0);
}

vec2 ToGrid(vec2 pos)
{
	return (vec2(0.5f) + glm::floor(pos * (float)pixelsPerUnit)) / (float)pixelsPerUnit;
}

vec2 ToGridCentered(vec2 pos)
{
	return glm::round(pos * (float)pixelsPerUnit) / (float)pixelsPerUnit;
}
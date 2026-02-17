#pragma once

#include <algorithm>
#include <cmath>
#include "headmodel/collimation/JawAperture.h"

namespace headmodel::collimation
{
// Signed distance to an axis-aligned rectangle
// Negative inside; positive outside; zero on boundary

inline double signedDistanceToRect(const JawApertureRect& r, double x, double y)
{
	// If inside: distance is negative = -min(distance to each side)
	if(r.contains(x,y))
	{
		const double dl = x - r.xMin;
		const double dr = r.xMax - x;
		const double db = y - r.yMin;
		const double dt = r.yMax - y;
		const double minToEdge = std::min(std::min(dl, dr), std::min(db, dt));
		return -minToEdge;
	}

	// If outside: distance to nearest point on rectangle
	const double cx = std::clamp(x, r.xMin, r.xMax);
	const double cy = std::clamp(y, r.yMin, r.yMax);
	const double dx = x - cx;
	const double dy = y - cy;
	return std::sqrt(dx*dx + dy*dy);
}

}

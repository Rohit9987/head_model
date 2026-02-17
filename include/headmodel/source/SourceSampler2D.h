#pragma once

#include <random>
#include "headmodel/geom/Vec3.h"

namespace headmodel::source
{

// Samples points in the "source plane" (z fixed by caller) in the source local XY frame.
// Return value is (x,y) offset from nominal source center, in mm.

class SourceSampler2D
{
public:
	virtual ~SourceSampler2D() = default;

	// Draw one sample offset (dx, dy, 0). caller adds to nominal source position.
	virtual headmodel::geom::Vec3 sampleOffset(std::mt19937& rng) const = 0;
};
}

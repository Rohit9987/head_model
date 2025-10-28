#pragma once
#include <vector>
#include "grid2d.h"

// Jaws rectangle (mm at isocentre): x in [x1,x2], y in [y1,y2]
struct Jaws { float x1_mm, x2_mm, y1_mm, y2_mm; };

std::vector<float> rasterize_jaws(const Grid2D& g, const Jaws& jaws, int supersample=4);


#pragma once

#include "headmodel/HeadModelConfig.h"
#include "headmodel/geom/Vec3.h"
#include "headmodel/grid/Grid2D.h"
#include <stdexcept>

namespace headmodel
{

struct FluenceResult
{
    grid::Grid2D<float> primary;
    grid::Grid2D<float> extra;
    grid::Grid2D<float> total;
};

struct FluencePointResult
{
	double primary = 0.0;
	double extra = 0.0;
	double total = 0.0;

	double w_primary = 0.9;
	double w_extra = 0.1;
};

class HeadModel
{
public:
    explicit HeadModel(const HeadModelConfig& config);

    FluenceResult computeOpenField(double fieldSize_mm);

    FluenceResult computeField(
        double x1_mm, double x2_mm,
        double y1_mm, double y2_mm
    );

	FluencePointResult computeOpenFieldAtPoint(
				const FluenceResult& isoFluence,
				const geom::Vec3& point_mm) const;

private:
    HeadModelConfig config_;

	double sqr(double x) const { return x * x; }

	double distanceSquared(const headmodel::geom::Vec3& a, 
						   const headmodel::geom::Vec3& b) const
	{
		return sqr(a.x - b.x) + 
			   sqr(a.y - b.y) +
			   sqr(a.z - b.z);
	}

	headmodel::geom::Vec3 intersectRayWithZPlane(
			const headmodel::geom::Vec3& source, 
			const headmodel::geom::Vec3& point,
			double zPlane_mm) const;

	double inverseSquareRelativeToIso(
			const headmodel::geom::Vec3& source,
			const headmodel::geom::Vec3& point,
			double zIso_mm) const;

};

}

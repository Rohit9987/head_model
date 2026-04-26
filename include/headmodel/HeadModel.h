#pragma once

#include "headmodel/HeadModelConfig.h"
#include "headmodel/grid/Grid2D.h"

namespace headmodel
{

struct FluenceResult
{
    grid::Grid2D<float> primary;
    grid::Grid2D<float> extra;
    grid::Grid2D<float> total;
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

private:
    HeadModelConfig config_;
};

}

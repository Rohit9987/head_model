// headmodel/fluence/FluenceModel.h
#pragma once
#include "headmodel/grid/Grid2D.h"
#include "headmodel/fluence/FluenceContext.h"

namespace headmodel::fluence {

class FluenceModel {
public:
    virtual ~FluenceModel() = default;
    virtual void compute(const FluenceContext& ctx, headmodel::grid::Grid2D<float>& out) const = 0;
};

}


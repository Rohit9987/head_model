// headmodel/fluence/FluenceContext.h
#pragma once
#include "headmodel/geom/Vec3.h"
#include "headmodel/collimation/JawAperture.h"

namespace headmodel::fluence {

struct BeamGeometry {
    headmodel::geom::Vec3 source;     // point source position in world coords
    double jawPlaneZ{};               // z location of jaw plane (world coords)
    double fluencePlaneZ{};           // z location where fluence is computed
};

struct FluenceContext {
    BeamGeometry geom;
    headmodel::collimation::JawApertureRect jawsAtJawPlane;
};

}


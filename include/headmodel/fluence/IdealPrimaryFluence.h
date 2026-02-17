// headmodel/fluence/IdealPrimaryFluence.h
#pragma once
#include "headmodel/fluence/FluenceModel.h"
#include "headmodel/geom/Vec3.h"

namespace headmodel::fluence {

class IdealPrimaryFluence final : public FluenceModel {
public:
    void compute(const FluenceContext& ctx, headmodel::grid::Grid2D<float>& out) const override {
        using headmodel::geom::Vec3;

        const double zJaw = ctx.geom.jawPlaneZ;
        const Vec3 S = ctx.geom.source;

        for(int j=0;j<out.ny();++j){
            for(int i=0;i<out.nx();++i){

                // Pixel center on fluence plane (assume plane is z = fluencePlaneZ)
                const double xF = out.xCenter(i);
                const double yF = out.yCenter(j);
                const double zF = ctx.geom.fluencePlaneZ;
                const Vec3 P{xF, yF, zF};

                // Ray from source S to pixel P: R(t)=S + t(P-S), t in [0,1]
                // Intersect with jaw plane z=zJaw:
                const double denom = (P.z - S.z);
                if(std::abs(denom) < 1e-12) { out(i,j) = 0.0f; continue; }

                const double tJaw = (zJaw - S.z) / denom;
                if(tJaw <= 0.0 || tJaw >= 1.0) { out(i,j) = 0.0f; continue; }

                const Vec3 Q = headmodel::geom::lerp(S, P, tJaw); // intersection point at jaw plane

                out(i,j) = ctx.jawsAtJawPlane.contains(Q.x, Q.y) ? 1.0f : 0.0f;
            }
        }
    }
};

}


#pragma once

#include <cstdint>
#include <memory>
#include <random>

#include "headmodel/fluence/FluenceModel.h"
#include "headmodel/source/SourceSampler2D.h"
#include "headmodel/geom/Vec3.h"
#include "headmodel/collimation/JawTransmissionModel.h"

namespace headmodel::fluence
{


// Exercise 1: geometric penumbra via finite focal spot sampling.
// For each pixel, average "visible through jaws?" over N source samples.

class FiniteSourceFluence final: public FluenceModel
{
public:
    struct Settings {
        int numSamplesPerPixel;
        uint32_t rngSeed;

        Settings(int n = 256, uint32_t seed = 12345)
            : numSamplesPerPixel(n), rngSeed(seed) {}
    };

	FiniteSourceFluence(std::shared_ptr<headmodel::source::SourceSampler2D> sampler, headmodel::collimation::JawTransmissionModel txModel, Settings s = Settings()):
			m_sampler(std::move(sampler)), m_s(s), m_txModel(std::move(txModel)) {}

	
	void compute(const FluenceContext& ctx, headmodel::grid::Grid2D<float>& out) const override
	{
		using headmodel::geom::Vec3;

		if(!m_sampler)
			throw std::runtime_error("FiniteSourceFluence: sampler is null");

		if(m_s.numSamplesPerPixel <=0)
			throw std::runtime_error("FiniteSourceFluence: numSamplesPerPixel must be >0");


		const double zJaw = ctx.geom.jawPlaneZ;
		const double zF = ctx.geom.fluencePlaneZ;

		const Vec3 S0 = ctx.geom.source;	// nominal source center

		// RNG: we want deterministic, but also avoid identical noise patterns per pixel
		// We'll hash pixel index into the seed
		for(int j = 0; j < out.ny(); ++j)
		{
			for(int i = 0; i < out.nx(); ++i)
			{
				const double xF = out.xCenter(i);
				const double yF = out.yCenter(j);
				const Vec3 P{xF, yF, zF};

				// Per-pixel RNG for reproducible but spatially varying noise:
				const uint32_t pixelHash = static_cast<uint32_t>(i * 73856093u) ^ static_cast<uint32_t>(j*19349663u);
				std::mt19937 rng(m_s.rngSeed ^ pixelHash);

				int visibleCount = 0;

				// Ex2 change:
				// We now count how many sampled source points were actually visible (not occluded)/
				int validSourceSamples = 0;

				// Exercise 3
				double sumW = 0.0;

				for(int s = 0; s < m_s.numSamplesPerPixel; ++s)
				{
					// Sample source point
					const Vec3 d = m_sampler->sampleOffset(rng);
					const Vec3 Sk = S0 + d;

					// Ex2:
					// discard source points that are occluded by jaws.
					// This is independent of the pixel P
					if(!isSourcePointVisibleThroughJaws(S0, Sk, ctx))
						continue;

					++validSourceSamples;

					// Ray S->P intersects jaw plane?
					const double denom = (P.z - Sk.z);
					if (std::abs(denom) < 1e-12)
						continue;

					const double tJaw = (zJaw - Sk.z)/denom;
					if(tJaw <=0.0 || tJaw >= 1.0)
						continue;

					const Vec3 Q = headmodel::geom::lerp(Sk, P, tJaw);

					// Exercise 3: continuous transmission weight instead of boolean contains ()
					const double w = m_txModel.Transmission(ctx.jawsAtJawPlane, Q.x, Q.y);
					sumW += w;

					if(ctx.jawsAtJawPlane.contains(Q.x, Q.y))
						++ visibleCount;
				}
			if(validSourceSamples > 0)
				{
					out(i,j) = static_cast<float>(sumW / static_cast<float>(validSourceSamples));
				}
			else
				out(i,j) = 0.0f;
			}
		}
	}

private:

	// Exercise 2
	bool isSourcePointVisibleThroughJaws(const headmodel::geom::Vec3& S0, 
						  const headmodel::geom::Vec3& Sk, 
						  const FluenceContext& ctx) const
	{
		using headmodel::geom::Vec3;

		const Vec3 Pref{0.0, 0.0, ctx.geom.fluencePlaneZ};
		using headmodel::geom::Vec3;

		const double denom = (Pref.z - Sk.z);
		if (std::abs(denom) < 1e-12)
			return true;		// degenerate -> treat as visible

		const double tJaw = (ctx.geom.jawPlaneZ - S0.z) / denom;

		// If jaw plane is not strictly between S0 and Sk, it can't occlude Sk.
		if(tJaw < 0.0 || tJaw >= 1.0)
			return true;	// jaw now between S0 and Sk

		const Vec3 Q = headmodel::geom::lerp(Sk, Pref, tJaw);

		return ctx.jawsAtJawPlane.contains(Q.x, Q.y);
	}

	std::shared_ptr<headmodel::source::SourceSampler2D> m_sampler;
	Settings m_s;
	headmodel::collimation::JawTransmissionModel m_txModel;
};

}


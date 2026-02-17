#pragma once

#include <random>
#include "headmodel/source/SourceSampler2D.h"

namespace headmodel::source 
{

// 2D isotropic Gaussian focal spot: dx, dy ~ N(0, sigma^2)
class GaussianSourceSampler2D final: public SourceSampler2D
{
public:
	explicit GaussianSourceSampler2D(double sigma_mm): m_sigma(sigma_mm) {}

	headmodel::geom::Vec3 sampleOffset(std::mt19937& rng) const override
	{
		std::normal_distribution<double> n(0.0, m_sigma);
		return { n(rng), n(rng), 0.0};
	}

	double sigmaMm() const { return m_sigma; }

private:
	double m_sigma{};
};
}

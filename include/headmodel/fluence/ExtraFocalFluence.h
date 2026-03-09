#pragma once
#include <memory>

#include "headmodel/fluence/FluenceModel.h"
#include "headmodel/fluence/FiniteSourceFluence.h"
#include "headmodel/source/SourceSampler2D.h"

namespace headmodel::fluence
{

// Explicit extra-focal source component.
// For now this is a thin wrapper around FiniteSourceFluence
// using a broader source distribution and the same geometry logic

class ExtraFocalFluence final: public FluenceModel
{
public:
	using Settings = FiniteSourceFluence::Settings;

	explicit ExtraFocalFluence(
			std::shared_ptr<headmodel::source::SourceSampler2D> sampler,
			Settings s = Settings()
			): m_impl(std::move(sampler), s);
			{}

	void compute(const FluenceContext& ctx, headmodel::grid::Grid2D<float>& out) const override
	{
		m_impl.compute(ctx, out);
	}

private:
	FiniteSourceFluence m_impl;
};

}

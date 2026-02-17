#pragma once

#include <memory>
#include <stdexcept>

#include "headmodel/fluence/FluenceModel.h"
#include "headmodel/grid/Grid2D.h"

namespace headmodel::fluence
{
// combines two fluence models linearly:
// Phi = w_primary * Phi_primary + w_extra * Phi_extra
class DualSourceFluenceModel final: public FluenceModel
{
public:
	struct Params
	{
		double w_primary = 0.9;
		double w_extra = 0.1;

		Params(double wp = 0.9, double we = 0.1)
			: w_primary(wp), w_extra(we) {}
	};

	DualSourceFluenceModel(std::shared_ptr<FluenceModel> primary,
						   std::shared_ptr<FluenceModel> extra,
						   Params p = Params())
		: m_primary(std::move(primary)),
		  m_extra(std::move(extra)),
		  m_p(p)
	{
		if(!m_primary || !m_extra)
			throw std::runtime_error("DualSourceFluenceModel: null submodel");

		if(m_p.w_primary < 0.0 || m_p.w_extra < 0.0)
			throw std::runtime_error("DualSourceFluenceModel: weights must be >= 0");

		const double sum = m_p.w_primary + m_p.w_extra;
		if(sum <= 0.0)
			throw std::runtime_error("DualSourceFluenceModel: weight sum must be > 0");


		// normalize weights
		m_p.w_primary /= sum;
		m_p.w_extra /= sum;
	}

	void compute(const FluenceContext& ctx, headmodel::grid::Grid2D<float>& out) const override
	{
		using headmodel::grid::Grid2D;
		Grid2D<float> tmpPrimary(out.nx(), out.ny(),
								 out.dx(), out.dy(),
								 out.xCenter(0) - 0.5 * out.dx(),
								 out.yCenter(0) - 0.5 * out.dy());

		Grid2D<float> tmpExtra(out.nx(), out.ny(),                                                
							   out.dx(), out.dy(),                                                
                               out.xCenter(0) - 0.5 * out.dx(),                                   
                               out.yCenter(0) - 0.5 * out.dy()); 

		tmpPrimary.fill(0.0f);
		tmpExtra.fill(0.0f);

		m_primary->compute(ctx, tmpPrimary);
		m_extra->compute(ctx, tmpExtra);

		for (int j = 0; j < out.ny(); ++j) {
            for (int i = 0; i < out.nx(); ++i) {
                out(i,j) = static_cast<float>(
                    m_p.w_primary * tmpPrimary(i,j) +
                    m_p.w_extra   * tmpExtra(i,j)
                );
            }
        }
    }

private:
    std::shared_ptr<FluenceModel> m_primary;
    std::shared_ptr<FluenceModel> m_extra;
    Params m_p;
};

} // namespace headmodel::fluence



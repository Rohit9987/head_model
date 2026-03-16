#pragma once

#include <cmath>
#include <stdexcept>

#include "headmodel/collimation/RectSignedDistance.h"

namespace headmodel::collimation
{
// Simple smooth jaw transmission model based on signed distance at jaw plane.
class JawTransmissionModel
{
public:
	struct Params
	{
		double T_leak = 0.003;	// 0.3% leakage as a starting point	 1.5 % is for MLC, can be modelled later into MLC
		double k_mm = 1.0;		// edge falloff scale (mm)

		Params(double T = 0.003, double k = 1.0): T_leak(T), k_mm(k) {}
	};

    explicit JawTransmissionModel(Params p = Params()) : m_p(p) {
        if (!(m_p.T_leak >= 0.0 && m_p.T_leak <= 1.0))
            throw std::runtime_error("JawTransmissionModel: T_leak must be in [0,1]");
        if (!(m_p.k_mm > 0.0))
            throw std::runtime_error("JawTransmissionModel: k_mm must be > 0");
    }

	double Transmission(const JawApertureRect& jaws, double x, double y) const
	{
		const double d = headmodel::collimation::signedDistanceToRect(jaws, x, y);

		// Logistic transition: inside (d<0) -> ~1; outside -> -T_leak
		const double e = std::exp(d / m_p.k_mm);
		return m_p.T_leak + (1.0 - m_p.T_leak) / (1.0 + e);
	}

private:
	Params m_p;
};

}

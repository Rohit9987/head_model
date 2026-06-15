#include "headmodel/HeadModel.h"

#include <iterator>
#include <memory>
#include <stdexcept>

#include "headmodel/geom/Vec3.h"
#include "headmodel/collimation/JawTransmissionModel.h"
#include "headmodel/fluence/FluenceContext.h"
#include "headmodel/fluence/FiniteSourceFluence.h"
#include "headmodel/fluence/DualSourceFluenceModel.h"
#include "headmodel/source/GaussianSourceSampler2D.h"

namespace headmodel
{

HeadModel::HeadModel(const HeadModelConfig& config)
    : config_(config)
{
    if (config_.nx <= 0 || config_.ny <= 0)
        throw std::runtime_error("HeadModel: grid dimensions must be > 0");

    if (config_.dx_mm <= 0.0 || config_.dy_mm <= 0.0)
        throw std::runtime_error("HeadModel: grid spacing must be > 0");

    if (config_.numSamplesPerPixel <= 0)
        throw std::runtime_error("HeadModel: numSamplesPerPixel must be > 0");
}

FluenceResult HeadModel::computeOpenField(double fieldSize_mm)
{
    const double half = 0.5 * fieldSize_mm;

    return computeField(
        -half, +half,
        -half, +half
    );
}

FluenceResult HeadModel::computeField(
    double x1Iso_mm, double x2Iso_mm,
    double y1Iso_mm, double y2Iso_mm
)
{
    using headmodel::geom::Vec3;
    using headmodel::grid::Grid2D;

    if (!(x2Iso_mm > x1Iso_mm) || !(y2Iso_mm > y1Iso_mm))
        throw std::runtime_error("HeadModel::computeField: invalid field limits");

    const double x0_mm = -0.5 * config_.nx * config_.dx_mm;
    const double y0_mm = -0.5 * config_.ny * config_.dy_mm;

    FluenceResult result{
        Grid2D<float>(config_.nx, config_.ny, config_.dx_mm, config_.dy_mm, x0_mm, y0_mm),
        Grid2D<float>(config_.nx, config_.ny, config_.dx_mm, config_.dy_mm, x0_mm, y0_mm),
        Grid2D<float>(config_.nx, config_.ny, config_.dx_mm, config_.dy_mm, x0_mm, y0_mm)
    };

    result.primary.fill(0.0f);
    result.extra.fill(0.0f);
    result.total.fill(0.0f);

    const Vec3 primarySource{0.0, 0.0, config_.zSource_mm};
    const Vec3 extraSource{0.0, 0.0, config_.extraSourceZ_mm};

    const double dSJ = config_.zJaw_mm - config_.zSource_mm;
    const double dSI = config_.zIso_mm - config_.zSource_mm;

    if (std::abs(dSI) < 1e-12)
        throw std::runtime_error("HeadModel::computeField: invalid source-isocentre distance");

    const double scaleIsoToJaw = dSJ / dSI;

    fluence::FluenceContext ctx;
    ctx.geom.source = primarySource;
    ctx.geom.jawPlaneZ = config_.zJaw_mm;
    ctx.geom.fluencePlaneZ = config_.zIso_mm;

    ctx.jawsAtJawPlane.xMin = x1Iso_mm * scaleIsoToJaw;
    ctx.jawsAtJawPlane.xMax = x2Iso_mm * scaleIsoToJaw;
    ctx.jawsAtJawPlane.yMin = y1Iso_mm * scaleIsoToJaw;
    ctx.jawsAtJawPlane.yMax = y2Iso_mm * scaleIsoToJaw;

    auto primarySampler =
        std::make_shared<source::GaussianSourceSampler2D>(config_.primarySigma_mm);

    auto extraSampler =
        std::make_shared<source::GaussianSourceSampler2D>(config_.extraFocalSigma_mm);

    collimation::JawTransmissionModel::Params txParams;
    txParams.T_leak = config_.jawLeakage;
    txParams.k_mm = config_.jawPenumbraK_mm;

    collimation::JawTransmissionModel tx(txParams);

    fluence::FiniteSourceFluence::Settings primarySettings;
    primarySettings.numSamplesPerPixel = config_.numSamplesPerPixel;
    primarySettings.rngSeed = config_.rngSeed;
    primarySettings.offaxis = true;

    fluence::FiniteSourceFluence::Settings extraSettings;
    extraSettings.numSamplesPerPixel = config_.numSamplesPerPixel;
    extraSettings.rngSeed = config_.rngSeed;
    extraSettings.offaxis = false;

    auto primaryModel =
        std::make_shared<fluence::FiniteSourceFluence>(
            primarySampler,
            primarySource,
            tx,
            primarySettings
        );

    auto extraModel =
        std::make_shared<fluence::FiniteSourceFluence>(
            extraSampler,
            extraSource,
            tx,
            extraSettings
        );

    fluence::DualSourceFluenceModel::Params dualParams;
    dualParams.w_primary = config_.primaryWeight;
    dualParams.w_extra = config_.extraFocalWeight;

    fluence::DualSourceFluenceModel dualModel(
        primaryModel,
        extraModel,
        dualParams
    );

    primaryModel->compute(ctx, result.primary);
    extraModel->compute(ctx, result.extra);
    dualModel.compute(ctx, result.total);

    return result;
}

double HeadModel::inverseSquareRelativeToIso(
		const headmodel::geom::Vec3& source,
		const headmodel::geom::Vec3& point,
		double zIso_mm) const
{
	const auto isoPoint = intersectRayWithZPlane(source, point, zIso_mm);

	const double d2Iso = distanceSquared(source, isoPoint);
	const double d2Point = distanceSquared(source, point);

	if(d2Point <= 0.0)
		return 0.0;

	return d2Iso/d2Point;
}


headmodel::geom::Vec3 HeadModel::intersectRayWithZPlane(
		const headmodel::geom::Vec3& source, 
		const headmodel::geom::Vec3& point,
		double zPlane_mm) const
{
	const double dz = point.z - source.z;

	if(std::abs(dz) < 1e-12)
		throw std::runtime_error("intersectRayWithZPlane: ray parallel to z-plane");

	const double t = (zPlane_mm - source.z) / dz;

	return
	{
		source.x + t * (point.x - source.x),
		source.y + t * (point.y - source.y),
		zPlane_mm
	};
}


FluencePointResult HeadModel::computeOpenFieldAtPoint(
			const FluenceResult& isoFluence,
			const geom::Vec3& point_mm) const
{

	const geom::Vec3 primarySource{
		0.0,
		0.0,
		config_.zSource_mm
	};

	const geom::Vec3 extraSource{
		0.0,
		0.0, 
		config_.extraSourceZ_mm
	};

	const double zIso = config_.zIso_mm;

	// Do not evalueate points upstream of either source.
	if(point_mm.z <= primarySource.z || point_mm.z <= extraSource.z)
		return {};

	const geom::Vec3 primaryIsoPoint = 
		intersectRayWithZPlane(primarySource, point_mm, zIso);

	const geom::Vec3 extraIsoPoint = 
		intersectRayWithZPlane(extraSource, point_mm, zIso);

	const double primaryInvSq =
		inverseSquareRelativeToIso(primarySource, point_mm, zIso);


    const double extraInvSq =
        inverseSquareRelativeToIso(extraSource, point_mm, zIso);

    const double primaryIso =
        isoFluence.primary.sampleBilinear(
            primaryIsoPoint.x,
            primaryIsoPoint.y
        );

    const double extraIso =
        isoFluence.extra.sampleBilinear(
            extraIsoPoint.x,
            extraIsoPoint.y
        );

    FluencePointResult out;
    out.primary = primaryIso * primaryInvSq;
    out.extra = extraIso * extraInvSq;
    out.total = out.w_primary * out.primary + out.w_extra * out.extra;

	return out;
}


} // namespace headmodel

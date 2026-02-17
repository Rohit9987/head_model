#include <iostream>
#include <string>
#include <memory>

#include "headmodel/grid/Grid2D.h"
#include "headmodel/geom/Vec3.h"
#include "headmodel/collimation/JawAperture.h"
#include "headmodel/fluence/FluenceContext.h"
#include "headmodel/fluence/IdealPrimaryFluence.h"
#include "headmodel/io/ImageIO.h"

#include "headmodel/source/GaussianSourceSampler2D.h"
#include "headmodel/fluence/FiniteSourceFluence.h"
#include "headmodel/fluence/DualSourceFluenceModel.h"

int main(int argc, char** argv)
{
	try
	{
		// ----------------------------
        // User-tweakable parameters
        // ----------------------------

		const double SAD_mm = 1000.0;		// source to iso
		const double zIso_mm = 0.0;			// define isocentre plane at z=0
		const double zSource_mm = -SAD_mm;	// source on negative z axis
		const double zJaw_mm = -500.0;		// effective jaw plane (example)	TODO: varian specs


		// Fluence grid at isocentre: 40x40 cm at 1 mm resolution
		const int nx = 401;
		const int ny = 401;
		const double dx_mm = 1.0;
		const double dy_mm = 1.0;


		// Lower-left corner so that center is near(0,0)
		const double x0_mm = -0.5 * nx * dx_mm;
		const double y0_mm = -0.5 * ny * dy_mm;

		// Jaw-defined field size "at isocentre": 100mm x 100mm (10x10 cm)
		// BUT: jaws are defined at the jaw plane. For Exercise 0 we will set the
        // jaw opening at the jaw plane to create a 10x10 cm field at isocentre
        // assuming straight-line geometry from point source.
		const double fieldSizeIso_mm = 100.0;	// 1 cm
		const double halfIso = 0.5 * fieldSizeIso_mm;

		// Similar triangles: halfJaw = halfIso * (distance source->jaw/distance source->iso)
		const double dSJ = (zJaw_mm - zSource_mm);
		const double dSI = (zIso_mm - zSource_mm);
		const double halfJaw = halfIso * (dSJ / dSI);

	    // Output filenames (optional argument base name)
        std::string base = (argc > 1) ? argv[1] : std::string("fluence0");
        const std::string pgmPath  = base + ".pgm";
        const std::string csvPath  = base + ".csv";
        const std::string profPath = base + "_profile_y0.csv";


        // ----------------------------
        // Build context
        // ----------------------------
		headmodel::fluence::FluenceContext ctx;
		ctx.geom.source = headmodel::geom::Vec3{0.0, 0.0, zSource_mm};
		ctx.geom.jawPlaneZ = zJaw_mm;
		ctx.geom.fluencePlaneZ = zIso_mm;

		ctx.jawsAtJawPlane.xMin = -halfJaw;
		ctx.jawsAtJawPlane.xMax = +halfJaw;
        ctx.jawsAtJawPlane.yMin = -halfJaw;
        ctx.jawsAtJawPlane.yMax = +halfJaw;

        // ----------------------------
        // Allocate grid and compute
        // ----------------------------
		headmodel::grid::Grid2D<float> fluence(nx, ny, dx_mm, dy_mm, x0_mm, y0_mm);
		fluence.fill(0.0f);

		//headmodel::fluence::IdealPrimaryFluence model;
		auto sampler = std::make_shared<headmodel::source::GaussianSourceSampler2D>(/*sigma_mm=*/0.3);
		auto extraSampler = std::make_shared<headmodel::source::GaussianSourceSampler2D>(15.0);
		headmodel::fluence::FiniteSourceFluence::Settings s;
		s.numSamplesPerPixel = 512;
		s.rngSeed = 12345;

		headmodel::collimation::JawTransmissionModel::Params p;
		p.T_leak = 0.015; //	1.5 %
		p.k_mm = 1.0;
		headmodel::collimation::JawTransmissionModel tx(p);


		//headmodel::fluence::FiniteSourceFluence model(sampler, tx, s);
		//model.compute(ctx, fluence);
		auto primaryModel = std::make_shared<headmodel::fluence::FiniteSourceFluence>(sampler, tx, s);
		auto extraModel = std::make_shared<headmodel::fluence::FiniteSourceFluence>(
				extraSampler, tx, s);

		headmodel::fluence::DualSourceFluenceModel dualModel(
			primaryModel, extraModel, headmodel::fluence::DualSourceFluenceModel::Params());

		auto runCase = [&](double fieldSizeIso_mm, const std::string& tag)
		{
			const double halfIso = 0.5 * fieldSizeIso_mm;

			const double dSJ = (zJaw_mm - zSource_mm);
			const double dSI = (zIso_mm - zSource_mm);
			const double halfJaw = halfIso * (dSJ / dSI);

			ctx.jawsAtJawPlane = {-halfJaw, +halfJaw, -halfJaw, +halfJaw};

			fluence.fill(0.0f);
			dualModel.compute(ctx, fluence);

			headmodel::io::writePGM_U8(tag, fluence, false, 0.0f, 1.0f);
			headmodel::io::writeProfileCSV(tag + "_profile_y0.csv", fluence, 0.0);
			std::cout << "Written " << tag ;
		};

		std::cout << "Beginning Run ...\n";

		runCase(200.0, "fluence_ex2_20x20");
		runCase(100.0, "fluence_ex2_10x10");
		runCase(20.0, "fluence_ex2_2x2");
		runCase(10.0, "fluence_ex2_1x1");
		runCase(5.0, "fluence_ex2_0.5x0.5");

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}


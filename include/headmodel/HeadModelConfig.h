#pragma once

namespace headmodel {

struct HeadModelConfig
{
    double SAD_mm = 1000.0;
    double zIso_mm = 0.0;
    double zSource_mm = -1000.0;
    double zJaw_mm = -595.0;

	/*
	const double xJaw_mm = -633.9 + 78.0/2;		 
	const double yJaw_mm = -72.11 + 77.7/2;
	const double mlc_mm  = -533.0 + 56.1/2; 
	*/

    int nx = 401;
    int ny = 401;
    double dx_mm = 1.0;
    double dy_mm = 1.0;

    double primarySigma_mm = 0.3;
    double extraFocalSigma_mm = 15.0;
    double extraSourceZ_mm = -900.0;

    int numSamplesPerPixel = 512;
    unsigned int rngSeed = 12345;

    double jawLeakage = 0.003;
    double jawPenumbraK_mm = 1.0;

    double primaryWeight = 0.9;
    double extraFocalWeight = 0.1;
};

}

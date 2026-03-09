#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <cstdint>
#include <stdexcept>


using Grid = std::vector<std::vector<double>>;

struct JawRect
{
	double X;	// cm 
	double Y;	// cm
};

struct DevGeom
{
	double zSource;	
	double zJaw;
	double zIso;
};

// check if the ray from S(xS, yS, zSource) to P(xP, yP, zIso) pass through jaw opening at zJaw
inline bool isVisibleThroughJaws(double xS, double yS, double xP, double yP, 
									const DevGeom& g, const JawRect& jaw)
{
	const double denom = (g.zIso - g.zSource);
	if(std::abs(denom) < 1e-12)
		return false;

	const double t = (g.zJaw - g.zSource) / denom;

	const double xJ = xS + t * (xP - xS);
	const double yJ = yS + t * (yP - yS);

	return (std::abs(xJ) <= jaw.X *0.5) && (std::abs(yJ) <= jaw.Y * 0.5);
}

// Extra focal source intensity on source plane (2D Gaussian)
inline double extraFocalIntensity(double xS, double yS, double A, double sigmaS_cm)
{
	const double r2 = xS * xS + yS * yS;
	return A * std::exp(-r2 / (2.0 * sigmaS_cm, sigmaS_cm));
}

// Compute extra-focal fluence at ONE point P on isocenter plane using DEV integration
double computeExtraFocalDEV(double xP, double yP,
							const DevGeom& g, const JawRect& jaw,
							double A, double sigmaS_cm,
							double dS_cm,
							bool useInverseSquare = true)
{
	// integrate over +/- where L ~ 6*sigma cpatures almost all gaussian area
	const double L = 6.0 * sigmaS_cm;

	double sum = 0.0;

	for (double xS = -L; xS <= L; xS += dS_cm)
	{
		for(double yS = -L; yS <= L; yS += dS_cm)
		{
			if(!isVisibleThroughJaws(xS, yS, xP, yP, g, jaw))
				continue;

			double I = extraFocalIntensity(xS, yS, A, sigmaS_cm);

			if(useInverseSquare)
			{
				const double dx = xP - xS;
				const double dy = yP - yS;
				const double dz = g.zIso - g.zSource;
				const double R2 = dx*dx + dy * dy + dz* dz;
				I /= std::max(R2, 1e-12);
			}

			sum += I;
		}
	}

	// multiply by area element
	sum *= (dS_cm * dS_cm);

	return sum;
}

Grid computeExtraFocalDEVMap(int nx, int ny, double spacingCm,
								const DevGeom& g, const JawRect& jaw,
								double A, double sigmaS_cm,
								double dS_cm,
								bool useInverseSquare =true)
{
	Grid out(nx, std::vector<double> (ny, 0.0));

	for(int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			double xP = (i - nx/2) * spacingCm;
			double yP = (j - ny/2) * spacingCm;

			out[i][j] = computeExtraFocalDEV(xP, yP, g, jaw, A, sigmaS_cm, dS_cm, useInverseSquare);
		}
	}

	return out;
}

void saveYProfileCSV(const Grid& g, double spacingCm,
					 int xIndex, const std::string& path)
{
	std::ofstream out(path);
	if(!out)
		throw std::runtime_error("Failed to open CSV file: " + path);

		const int nx = static_cast<int>(g.size());
		const int ny = static_cast<int>(g[0].size());

		if(xIndex < 0 || xIndex >= nx)
			throw std::runtime_error("xIndex out of range in saveYProfileCSV");

		out << "y_cm, value\n";
		for (int j = 0; j < ny; ++j)
		{
			double y = (j - ny/2) * spacingCm;
			out << y << ","  << g[xIndex][j] << '\n';
		}
}

void saveGridAsPGM(const Grid& g, const std::string& path, double clampMax = -1.0)
{
    std::ofstream out(path, std::ios::binary);
    if (!out)
        throw std::runtime_error("Failed to open PGM file: " + path);

    const int nx = static_cast<int>(g.size());
    const int ny = static_cast<int>(g[0].size());

    // Find min/max (for normalization)
    double mn = g[0][0], mx = g[0][0];
    for (int i = 0; i < nx; ++i)
    {
        for (int j = 0; j < ny; ++j)
        {
            mn = std::min(mn, g[i][j]);
            mx = std::max(mx, g[i][j]);
        }
    }

    // Optional clamp (useful if a few high pixels ruin contrast)
    if (clampMax > 0.0)
        mx = std::min(mx, clampMax);

    const double denom = (mx > mn) ? (mx - mn) : 1.0;

    // PGM header (P5 = binary grayscale)
    // Note: PGM expects width then height. We'll write width=ny, height=nx.
    out << "P5\n" << ny << " " << nx << "\n255\n";

    // Write pixels row-major
    for (int i = 0; i < nx; ++i)
    {
        for (int j = 0; j < ny; ++j)
        {
            double v = g[i][j];
            if (clampMax > 0.0) v = std::min(v, clampMax);
            double norm = (v - mn) / denom;                 // 0..1
            int pix = static_cast<int>(std::lround(norm * 255.0));
            pix = std::clamp(pix, 0, 255);
            const uint8_t b = static_cast<uint8_t>(pix);
            out.write(reinterpret_cast<const char*>(&b), 1);
        }
    }
}


int main()
{
    int nx = 201;
    int ny = 201;
    double spacing = 0.25;   // cm

    DevGeom g;
    g.zSource = 12.5;   // cm
    g.zJaw    = 45.0;   // cm
    g.zIso    = 100.0;  // cm

    JawRect jaw;
    jaw.X = 10.0; // 10x10 cm
    jaw.Y = 10.0;

    double A = 1.0;        // relative amplitude
    double sigmaS = 1.0;   // cm on source plane
    double dS = 0.1;       // cm integration step

    auto devMap = computeExtraFocalDEVMap(nx, ny, spacing,
                                          g, jaw,
                                          A, sigmaS,
                                          dS, true);

    saveGridAsPGM(devMap, "dev_extra_focal.pgm");

    int xCenter = nx / 2;
    saveYProfileCSV(devMap, spacing, xCenter, "dev_extra_focal_y_profile.csv");

    std::cout << "Saved:\n";
    std::cout << "  dev_extra_focal.pgm\n";
    std::cout << "  dev_extra_focal_2d.csv\n";
    std::cout << "  dev_extra_focal_y_profile.csv\n";

    return 0;
}

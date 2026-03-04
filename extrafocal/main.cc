#include <vector>
#include <cmath>
#include <iostream>

#include <fstream>
#include <algorithm>
#include <string>
#include <cstdint>

using Grid = std::vector<std::vector<double>>;

Grid createAperture(int nx, int ny, double fieldSizeCm, double spacingCm)
{
	Grid aperture(nx, std::vector<double>(ny, 0.0));

	double halfField = fieldSizeCm/2;

	for(int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			double x = (i - nx/2) * spacingCm;
			double y = (j - ny/2) * spacingCm;

			if(std::abs(x) <= halfField
				&& std::abs(y) <= halfField)
				aperture[i][j] = 1.0;
		}
	}

	return aperture;
}

//			Gaussian Kernel

Grid createGaussianKernel(int size, double sigmaPixels)
{
	Grid kernel(size, std::vector<double>(size));
	int center = size / 2;

	for(int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			double dx = i - center;
			double dy = j - center;

			double r2 = dx*dx + dy*dy;

			kernel[i][j] = std::exp(-r2 / (2.0 * sigmaPixels *sigmaPixels));
		}
	}

	return kernel;
}

//			Convolution
Grid Convolve(const Grid& input, const Grid& kernel)
{
	int nx = input.size();
	int ny = input[0].size();
	int ksize = kernel.size();
	int kcenter = ksize/2;

	Grid output(nx, std::vector<double>(ny, 0.0));

	for(int x = 0; x < nx; x++)
	{
		for(int y = 0; y < ny; ++y)
		{
			double sum = 0.0;

			for(int i = 0; i < ksize; i++)
			{
				for(int j = 0; j < ksize; ++j)
				{
					int xi = x + i - kcenter;
					int yj = y + j - kcenter;
					
					if(xi >= 0 && xi < nx && yj >= 0 && yj < ny)
						sum += input[xi][yj] * kernel[i][j];
				}
			}
			output[x][y] = sum;
		}
	}
	return output;
}


void saveGridAsCSV(const Grid& g, const std::string& path)
{
    std::ofstream out(path);
    if (!out)
        throw std::runtime_error("Failed to open CSV file: " + path);

    const int nx = static_cast<int>(g.size());
    const int ny = static_cast<int>(g[0].size());

    for (int i = 0; i < nx; ++i)
    {
        for (int j = 0; j < ny; ++j)
        {
            out << g[i][j];
            if (j + 1 < ny) out << ",";
        }
        out << "\n";
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

	double spacing = 0.25;		// cm
	double fieldSize = 10.0;	// cm

	double sigmaCm = 1.0;		// Gaussian width from Pinnacle
	double height = 0.03;		// Gaussian height

	double sigmaPixels = sigmaCm / spacing;

	auto aperture = createAperture(nx, ny, fieldSize, spacing);

	auto kernel = createGaussianKernel(41, sigmaPixels);

	auto extraFluence = Convolve(aperture, kernel);

	// scale by height
	for(int i  = 0; i < nx; i++)
	{
		for(int j = 0; j < ny; ++j)
		{
			extraFluence[i][j] *= height;
		}
	}
	std::cout << "Extra-focal computed.\n";

	saveGridAsCSV(aperture, "aperture.csv");
	saveGridAsPGM(aperture, "aperture.pgm");

	saveGridAsCSV(extraFluence, "extra_focal.csv");
	saveGridAsPGM(extraFluence, "extra_focal.pgm");

	return 0;
}



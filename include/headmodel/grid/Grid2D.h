// headmodel/grid/Grid2D.h
#pragma once
#include <vector>
#include <stdexcept>

namespace headmodel::grid {

template<typename T>
class Grid2D {
public:
    Grid2D(int nx, int ny, double dx, double dy, double x0, double y0)
        : m_nx(nx), m_ny(ny), m_dx(dx), m_dy(dy), m_x0(x0), m_y0(y0),
          m_data(static_cast<size_t>(nx)*static_cast<size_t>(ny)) {
        if(nx<=0 || ny<=0) throw std::runtime_error("Grid2D invalid size");
    }

    int nx() const { return m_nx; }
    int ny() const { return m_ny; }
    double dx() const { return m_dx; }
    double dy() const { return m_dy; }

    // World coordinate of pixel center (i,j)
    double xCenter(int i) const { return m_x0 + (i + 0.5)*m_dx; }
    double yCenter(int j) const { return m_y0 + (j + 0.5)*m_dy; }

    T& operator()(int i, int j) { return m_data[static_cast<size_t>(j)*m_nx + i]; }
    const T& operator()(int i, int j) const { return m_data[static_cast<size_t>(j)*m_nx + i]; }

    void fill(const T& v){ std::fill(m_data.begin(), m_data.end(), v); }

	T sampleBilinear(double x_mm, double y_mm) const
	{
		const double fx = (x_mm - m_x0) / m_dx;
		const double fy = (y_mm - m_y0) / m_dy;

		const int ix0 = static_cast<int>(std::floor(fx));
		const int iy0 = static_cast<int>(std::floor(fy));

		const int ix1 = ix0 + 1;
		const int iy1 = iy0 + 1;

		if (ix0 < 0 || iy0 < 0 ||
			ix1 >= m_nx ||
			iy1 >= m_ny)
		{
			return T{};
		}

		const double tx = fx - static_cast<double>(ix0);
		const double ty = fy - static_cast<double>(iy0);

		const double v00 = static_cast<double>(at(ix0, iy0));
		const double v10 = static_cast<double>(at(ix1, iy0));
		const double v01 = static_cast<double>(at(ix0, iy1));
		const double v11 = static_cast<double>(at(ix1, iy1));

		const double v0 = (1.0 - tx) * v00 + tx * v10;
		const double v1 = (1.0 - tx) * v01 + tx * v11;

		return static_cast<T>((1.0 - ty) * v0 + ty * v1);
	}

	T& at(int ix, int iy)
	{
		return m_data[static_cast<std::size_t>(iy * m_nx + ix)];
	}

	const T& at(int ix, int iy) const
	{
		return m_data[static_cast<std::size_t>(iy * m_nx + ix)];
	}

	double x0() const { return m_x0; }
	double y0() const { return m_y0; }


private:
    int m_nx{}, m_ny{};
    double m_dx{}, m_dy{};
    double m_x0{}, m_y0{}; // lower-left corner (world coords)
    std::vector<T> m_data;
};

}


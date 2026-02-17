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

private:
    int m_nx{}, m_ny{};
    double m_dx{}, m_dy{};
    double m_x0{}, m_y0{}; // lower-left corner (world coords)
    std::vector<T> m_data;
};

}


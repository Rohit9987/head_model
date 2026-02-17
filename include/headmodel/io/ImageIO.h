#pragma once
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "headmodel/grid/Grid2D.h"

namespace headmodel::io {

// Map float image to [0,255] and write PGM (P5)
inline void writePGM_U8(
    const std::string& filePath,
    const headmodel::grid::Grid2D<float>& img,
    bool autoWindow = true,
    float vmin = 0.0f,
    float vmax = 1.0f
) {
    if (img.nx() <= 0 || img.ny() <= 0) {
        throw std::runtime_error("writePGM_U8: empty image");
    }

    float lo = vmin, hi = vmax;

    if (autoWindow) {
        lo = std::numeric_limits<float>::infinity();
        hi = -std::numeric_limits<float>::infinity();
        for (int j = 0; j < img.ny(); ++j) {
            for (int i = 0; i < img.nx(); ++i) {
                const float v = img(i, j);
                lo = std::min(lo, v);
                hi = std::max(hi, v);
            }
        }
        if (!(hi > lo)) { // all same
            lo = 0.0f;
            hi = lo + 1.0f;
        }
    } else {
        if (!(hi > lo)) {
            throw std::runtime_error("writePGM_U8: vmax must be > vmin");
        }
    }

    std::ofstream out(filePath, std::ios::binary);
    if (!out) throw std::runtime_error("writePGM_U8: cannot open " + filePath);

    out << "P5\n" << img.nx() << " " << img.ny() << "\n255\n";

    std::vector<uint8_t> buffer(static_cast<size_t>(img.nx()) * img.ny());
    const float inv = 1.0f / (hi - lo);

    for (int j = 0; j < img.ny(); ++j) {
        for (int i = 0; i < img.nx(); ++i) {
            float x = (img(i, j) - lo) * inv;
            x = std::clamp(x, 0.0f, 1.0f);
            buffer[static_cast<size_t>(j) * img.nx() + i] =
                static_cast<uint8_t>(std::lround(255.0f * x));
        }
    }

    out.write(reinterpret_cast<const char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
    if (!out) throw std::runtime_error("writePGM_U8: failed writing " + filePath);
}

// Write grid to CSV with header: x(mm), y(mm), value
inline void writeCSV_XYV(
    const std::string& filePath,
    const headmodel::grid::Grid2D<float>& img
) {
    std::ofstream out(filePath);
    if (!out) throw std::runtime_error("writeCSV_XYV: cannot open " + filePath);

    out << "x_mm,y_mm,value\n";
    out << std::fixed << std::setprecision(6);

    for (int j = 0; j < img.ny(); ++j) {
        const double y = img.yCenter(j);
        for (int i = 0; i < img.nx(); ++i) {
            const double x = img.xCenter(i);
            out << x << "," << y << "," << img(i, j) << "\n";
        }
    }
}

// Extract and write a 1D profile at fixed y = y0 (nearest row)
// Output CSV: x_mm,value
inline void writeProfileCSV(
    const std::string& filePath,
    const headmodel::grid::Grid2D<float>& img,
    double y0_mm
) {
    // find nearest row
    int bestJ = 0;
    double bestD = std::numeric_limits<double>::infinity();
    for (int j = 0; j < img.ny(); ++j) {
        const double d = std::abs(img.yCenter(j) - y0_mm);
        if (d < bestD) { bestD = d; bestJ = j; }
    }

    std::ofstream out(filePath);
    if (!out) throw std::runtime_error("writeProfileCSV: cannot open " + filePath);

    out << "x_mm,value\n";
    out << std::fixed << std::setprecision(6);
    for (int i = 0; i < img.nx(); ++i) {
        out << img.xCenter(i) << "," << img(i, bestJ) << "\n";
    }
}

} // namespace headmodel::io


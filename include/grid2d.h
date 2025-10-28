#pragma once
#include <utility>
#include <cstddef>

struct Grid2D {
  int nx{}, ny{};
  float dx{}, dy{};     // mm
  float x0{}, y0{};     // lower-left corner (mm)
};
inline std::pair<int,int> cax_index(const Grid2D& g){ return {g.nx/2, g.ny/2}; }
inline size_t lin(int i, int j, int nx){ return static_cast<size_t>(j)*nx + i; }


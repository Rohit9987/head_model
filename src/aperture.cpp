#include "aperture.h"
#include <algorithm>

std::vector<float> rasterize_jaws(const Grid2D& g, const Jaws& J, int supersample)
{
  std::vector<float> A((size_t)g.nx*g.ny, 0.f);
  const float sx = g.dx / supersample;
  const float sy = g.dy / supersample;

  for (int j=0; j<g.ny; ++j){
    for (int i=0; i<g.nx; ++i){
      int inside = 0;
      for(int sj=0; sj<supersample; ++sj){
        for(int si=0; si<supersample; ++si){
          const float x = (g.x0 + (i+0.5f)*g.dx) + (si - (supersample-1)/2.f)*sx;
          const float y = (g.y0 + (j+0.5f)*g.dy) + (sj - (supersample-1)/2.f)*sy;
          if (x>=J.x1_mm && x<=J.x2_mm && y>=J.y1_mm && y<=J.y2_mm) inside++;
        }
      }
      A[lin(i,j,g.nx)] = float(inside) / float(supersample*supersample);
    }
  }
  return A;
}


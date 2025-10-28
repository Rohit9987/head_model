#include "convolve.h"
#include <algorithm>

static std::vector<float> convolve_1d_x(const std::vector<float>& img, int nx, int ny,
                                        const std::vector<float>& kx)
{
  int half = (int)kx.size()/2;
  std::vector<float> tmp(img.size(), 0.f);
  for (int j=0; j<ny; ++j){
    for (int i=0; i<nx; ++i){
      float acc = 0.f;
      for (int t=-half; t<=half; ++t){
        int ii = std::clamp(i+t, 0, nx-1);
        acc += kx[t+half] * img[(size_t)j*nx + ii];
      }
      tmp[(size_t)j*nx + i] = acc;
    }
  }
  return tmp;
}

static std::vector<float> convolve_1d_y(const std::vector<float>& img, int nx, int ny,
                                        const std::vector<float>& ky)
{
  int half = (int)ky.size()/2;
  std::vector<float> out(img.size(), 0.f);
  for (int j=0; j<ny; ++j){
    for (int i=0; i<nx; ++i){
      float acc = 0.f;
      for (int t=-half; t<=half; ++t){
        int jj = std::clamp(j+t, 0, ny-1);
        acc += ky[t+half] * img[(size_t)jj*nx + i];
      }
      out[(size_t)j*nx + i] = acc;
    }
  }
  return out;
}

std::vector<float> convolve_separable(const std::vector<float>& img, int nx, int ny,
                                      const std::vector<float>& kx, const std::vector<float>& ky)
{
  auto tmp = convolve_1d_x(img, nx, ny, kx);
  return convolve_1d_y(tmp, nx, ny, ky);
}

void normalize_cax(std::vector<float>& phi, int nx, int ny)
{
  const int cx = nx/2, cy = ny/2;
  const float cax = phi[(size_t)cy*nx + cx];
  if (cax > 0.f) for (auto& v : phi) v /= cax;
}


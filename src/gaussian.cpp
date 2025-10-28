#include "gaussian.h"
#include <cmath>
#include <algorithm>

std::vector<float> gaussian_1d(float sigma_mm, float d_mm, float truncate_sigmas)
{
  const float sigma_px = sigma_mm / d_mm;
  int half = std::max(1, (int)std::ceil(truncate_sigmas * sigma_px));
  std::vector<float> k(2*half+1);
  const float s2 = 2.f * sigma_px * sigma_px;
  float sum = 0.f;
  for (int n=-half; n<=half; ++n){
    float v = std::exp(-(n*n) / s2);
    k[n+half] = v; sum += v;
  }
  for (auto& v: k) v /= sum;
  return k;
}


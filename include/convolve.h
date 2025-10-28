#pragma once
#include <vector>

// Separable convolution: y = (kx ⊗ ky) * img
std::vector<float> convolve_separable(const std::vector<float>& img, int nx, int ny,
                                      const std::vector<float>& kx, const std::vector<float>& ky);

// Normalize map so central-axis pixel is 1.0
void normalize_cax(std::vector<float>& phi, int nx, int ny);


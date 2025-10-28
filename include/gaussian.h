#pragma once
#include <vector>

// Normalized 1-D Gaussian taps for separable convolution.
std::vector<float> gaussian_1d(float sigma_mm, float d_mm, float truncate_sigmas=4.f);


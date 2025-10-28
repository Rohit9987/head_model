#pragma once
#include <string>
#include <vector>
#include "grid2d.h"

struct PrimaryParams {
  float SAD_mm{};
  float sigma_mm{};   // focal-spot Gaussian sigma at isocentre (mm)
  float fwhm_mm{};    // optional (info)
};

bool h5_load_model(const std::string& path, Grid2D& grid, PrimaryParams& prm,
                   std::vector<float>& x_mm, std::vector<float>& y_mm);

bool h5_write_fluence(const std::string& path, const std::vector<float>& phi, int nx, int ny, bool filled);

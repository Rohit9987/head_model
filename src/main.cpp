#include <iostream>
#include <string>
#include "h5_model.h"
#include "aperture.h"
#include "gaussian.h"
#include "convolve.h"

int main(int argc, char** argv)
{
  if (argc < 2){
    std::cerr << "Usage: primary_fluence <truebeam_6X_primary_headmodel.h5> [field_x_cm=10] [field_y_cm=10]\n";
    return 1;
  }
  std::string h5 = argv[1];
  float fx_cm = (argc>2)? std::stof(argv[2]) : 10.f;
  float fy_cm = (argc>3)? std::stof(argv[3]) : 10.f;

  Grid2D grid; PrimaryParams prm; std::vector<float> x_mm, y_mm;
  if (!h5_load_model(h5, grid, prm, x_mm, y_mm)){
    std::cerr << "Failed to load HDF5\n"; return 2;
  }

  // Rasterize jaws at isocentre
  const float hx_mm = 0.5f * fx_cm * 10.f;
  const float hy_mm = 0.5f * fy_cm * 10.f;
  Jaws jaws{-hx_mm, +hx_mm, -hy_mm, +hy_mm};
  auto aperture = rasterize_jaws(grid, jaws, /*supersample=*/4);

  // Separable Gaussian from sigma & grid spacing
  auto kx = gaussian_1d(prm.sigma_mm, grid.dx);
  auto ky = gaussian_1d(prm.sigma_mm, grid.dy);

  // Convolution → primary fluence
  auto phi = convolve_separable(aperture, grid.nx, grid.ny, kx, ky);
  normalize_cax(phi, grid.nx, grid.ny);

  // Write back
  if (!h5_write_fluence(h5, phi, grid.nx, grid.ny, /*filled=*/true)){
    std::cerr << "Failed to write fluence\n"; return 3;
  }

  std::cout << "Primary fluence written. CAX=1.0 for " << fx_cm << "x" << fy_cm << " cm^2.\n";
  return 0;
}


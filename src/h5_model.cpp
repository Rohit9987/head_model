#include "h5_model.h"
#include <highfive/H5File.hpp>
#include <stdexcept>

using namespace HighFive;

bool h5_load_model(const std::string& path, Grid2D& grid, PrimaryParams& prm,
                   std::vector<float>& x_mm, std::vector<float>& y_mm)
{
  File f(path, File::ReadWrite);
  auto g_beam  = f.getGroup("/beam");
  auto g_plane = f.getGroup("/plane_isocenter");
  auto g_grid  = g_plane.getGroup("grid");
  auto g_prim  = g_plane.getGroup("primary");

  g_beam.getAttribute("SAD_mm").read(prm.SAD_mm);
  g_prim.getAttribute("sigma_mm").read(prm.sigma_mm);
  if (g_prim.hasAttribute("fwhm_mm")) g_prim.getAttribute("fwhm_mm").read(prm.fwhm_mm);

  f.getDataSet("/plane_isocenter/grid/x_mm").read(x_mm);
  f.getDataSet("/plane_isocenter/grid/y_mm").read(y_mm);
  grid.nx = (int)x_mm.size();
  grid.ny = (int)y_mm.size();

  float tmp[2];
  g_grid.getAttribute("spacing_mm").read(tmp); grid.dx = tmp[0]; grid.dy = tmp[1];
  g_grid.getAttribute("origin_mm").read(tmp);  grid.x0 = tmp[0]; grid.y0 = tmp[1];
  return true;
}

// helper: reshape flat -> 2D
static std::vector<std::vector<float>>
as_2d(const std::vector<float>& v, int nx, int ny) {
    std::vector<std::vector<float>> M(ny, std::vector<float>(nx));
    for (int j = 0; j < ny; ++j)
        std::copy_n(v.data() + j*nx, nx, M[j].begin());
    return M;
}

bool h5_write_fluence(const std::string& path,
                      const std::vector<float>& phi, int nx, int ny, bool filled)
{
    using namespace HighFive;
    File f(path, File::ReadWrite);
    DataSet ds = f.getDataSet("/plane_isocenter/primary/fluence_primary");

    const auto dims = ds.getSpace().getDimensions();
    if (dims.size()!=2 || dims[0]!=(size_t)ny || dims[1]!=(size_t)nx)
        throw std::runtime_error("fluence_primary dims mismatch");

    auto M = as_2d(phi, nx, ny);   // <-- 2-D container
    ds.write(M);                   // HighFive infers rank from container-of-containers

    // Optional: set attributes on DATASET
    if (ds.hasAttribute("filled")) ds.deleteAttribute("filled");
    const uint8_t v = filled ? 1u : 0u;
    ds.createAttribute<uint8_t>("filled", HighFive::DataSpace::From(v)).write(v);
    const std::string note = "CAX=1.0 after computation (reference field normalized)";
    if (ds.hasAttribute("normalized")) ds.getAttribute("normalized").write(note);
    else ds.createAttribute<std::string>("normalized", HighFive::DataSpace::From(note)).write(note);

    return true;
}


/*
bool h5_write_fluence(const std::string& path,
                      const std::vector<float>& phi, int nx, int ny, bool filled)
{
	HighFive::File f(path, HighFive::File::ReadWrite);
	HighFive::DataSet ds = f.getDataSet("/plane_isocenter/primary/fluence_primary");

	// sanity check
	const auto dims = ds.getSpace().getDimensions();
	if (dims.size()!=2 || dims[0]!=(size_t)ny || dims[1]!=(size_t)nx)
	  throw std::runtime_error("fluence_primary dims mismatch");

	std::vector<std::vector<float>> M(ny, std::vector<float>(nx));
	for (int j=0; j<ny; ++j) std::copy_n(&phi[j*nx], nx, M[j].begin());
	ds.write(M);                    // HighFive infers rank from container-of-containers


	// Optional: also update group attrs if present (backward compatibility)
	auto g_prim = f.getGroup("/plane_isocenter/primary");
	if (g_prim.hasAttribute("filled"))  g_prim.getAttribute("filled").write(filled);
	if (g_prim.hasAttribute("normalized")) g_prim.getAttribute("normalized").write("CAX=1.0 for 10x10 cm^2 open field");

	return true;
}

*/

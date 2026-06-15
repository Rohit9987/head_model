#include <iostream>
#include <vector>
#include <string>

#include "headmodel/HeadModel.h"
#include "headmodel/HeadModelConfig.h"
#include "headmodel/io/ImageIO.h"


int main()
{
    try
    {
        headmodel::HeadModelConfig config;

        headmodel::HeadModel model(config);

        const std::vector<double> fieldSizes_mm = {
            200.0, 100.0
        };

        const double planeZ_mm = 100.0; // 110 cm from source if iso is z = 0 and source is z = -1000

        for (double fieldSize : fieldSizes_mm)
        {
            auto isoResult = model.computeOpenField(fieldSize);

            std::string tag =
                "fluence_ex2_" + std::to_string(static_cast<int>(fieldSize / 10.0)) + "x" +
                std::to_string(static_cast<int>(fieldSize / 10.0));

            headmodel::io::writePGM_U8(tag + "_iso", isoResult.total, false, 0.0f, 1.0f);

            headmodel::io::writeProfileCSV_Multi(
                tag + "_iso_profile.csv",
                isoResult.primary,
                isoResult.extra,
                isoResult.total,
                0
            );

            std::cout << "Written " << tag << "_iso\n";

            const double x0_mm = isoResult.total.x0();
            const double y0_mm = isoResult.total.y0();
            const double dx_mm = isoResult.total.dx();
            const double dy_mm = isoResult.total.dy();
            const int nx = isoResult.total.nx();
            const int ny = isoResult.total.ny();

            headmodel::FluenceResult plane110{
                headmodel::grid::Grid2D<float>(nx, ny, dx_mm, dy_mm, x0_mm, y0_mm),
                headmodel::grid::Grid2D<float>(nx, ny, dx_mm, dy_mm, x0_mm, y0_mm),
                headmodel::grid::Grid2D<float>(nx, ny, dx_mm, dy_mm, x0_mm, y0_mm)
            };

            plane110.primary.fill(0.0f);
            plane110.extra.fill(0.0f);
            plane110.total.fill(0.0f);

            for (int iy = 0; iy < ny; ++iy)
            {
                for (int ix = 0; ix < nx; ++ix)
                {
                    const double x_mm = x0_mm + static_cast<double>(ix) * dx_mm;
                    const double y_mm = y0_mm + static_cast<double>(iy) * dy_mm;

                    headmodel::geom::Vec3 point{
                        x_mm,
                        y_mm,
                        planeZ_mm
                    };

                    const auto phi =
                        model.computeOpenFieldAtPoint(isoResult, point);

                    plane110.primary.at(ix, iy) = static_cast<float>(phi.primary);
                    plane110.extra.at(ix, iy) = static_cast<float>(phi.extra);
                    plane110.total.at(ix, iy) = static_cast<float>(phi.total);
                }
            }

            headmodel::io::writePGM_U8(tag + "_z110cm", plane110.total, false, 0.0f, 1.0f);

            headmodel::io::writeProfileCSV_Multi(
                tag + "_z110cm_profile.csv",
                plane110.primary,
                plane110.extra,
                plane110.total,
                0
            );

            std::cout << "Written " << tag << "_z110cm\n";
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}

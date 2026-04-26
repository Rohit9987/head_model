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

        for (double fieldSize : fieldSizes_mm)
        {
            auto result = model.computeOpenField(fieldSize);

            std::string tag =
                "fluence_ex2_" + std::to_string(static_cast<int>(fieldSize / 10.0)) + "x" +
                std::to_string(static_cast<int>(fieldSize / 10.0));

            headmodel::io::writePGM_U8(tag, result.total, false, 0.0f, 1.0f);

            headmodel::io::writeProfileCSV_Multi(
                tag + "_profile.csv",
                result.primary,
                result.extra,
                result.total,
                0
            );

            std::cout << "Written " << tag << "\n";
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}

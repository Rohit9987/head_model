#pragma once
#include <vector>
#include <cstddef>

namespace headmodel::fluence {

struct OAFPoint
{
    double r_cm;
    double value;
};

inline const std::vector<OAFPoint> oafTable = {
    {0.0,  1.0000},
    {0.5,  1.0010},
    {1.0,  1.0025},
    {1.5,  1.0030},
    {2.0,  1.0020},
    {2.5,  1.0021},
    {3.0,  1.0040},
    {3.5,  1.0070},
    {4.0,  1.0100},
    {5.0,  1.0160},
    {6.0,  1.0195},
    {7.0,  1.0210},
    {8.0,  1.0220},
    {9.0,  1.0250},
    {10.0, 1.0300},
    {11.0, 1.0350},
    {12.0, 1.0400},
    {13.0, 1.0430},
    {14.0, 1.0480},
    {15.0, 1.0520},
    {16.0, 1.0560},
    {17.0, 1.0610},
    {18.0, 1.0670},
    {19.0, 1.0690},
    {20.0, 1.0700},
    {21.0, 1.0700},
    {22.0, 1.0700},
    {23.0, 1.0700},
    {24.0, 1.0700},
    {25.0, 1.0700},
    {26.0, 1.0700},
    {27.0, 1.0700},
    {28.0, 1.0700},
    {29.0, 1.0700},
    {30.0, 1.0700}
};

inline double oafFactor(double r_cm)
{
    if (r_cm <= oafTable.front().r_cm)
        return oafTable.front().value;

    if (r_cm >= oafTable.back().r_cm)
        return oafTable.back().value;

    for (std::size_t i = 0; i + 1 < oafTable.size(); ++i)
    {
        if (r_cm >= oafTable[i].r_cm && r_cm <= oafTable[i + 1].r_cm)
        {
            const double r0 = oafTable[i].r_cm;
            const double r1 = oafTable[i + 1].r_cm;
            const double f0 = oafTable[i].value;
            const double f1 = oafTable[i + 1].value;
            const double t = (r_cm - r0) / (r1 - r0);
            return f0 + t * (f1 - f0);
        }
    }

    return 1.0;
}

}

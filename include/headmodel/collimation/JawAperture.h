// headmodel/collimation/JawAperture.h
#pragma once

namespace headmodel::collimation {

// Jaw opening defined at the jaw plane in that plane's coordinate system.
// For now: symmetric rectangle in X/Y (jaw-defined field).
struct JawApertureRect {
    double xMin{}, xMax{};
    double yMin{}, yMax{};

    bool contains(double x, double y) const {
        return (x >= xMin && x <= xMax && y >= yMin && y <= yMax);
    }
};

}


#pragma once
#include <vector>
#include "Point.h"

class AddMenu {
public:
    bool isOpen;

    AddMenu();
    
    // Returns true if a new object was successfully injected into the scene
    bool draw(std::vector<Point>& points, std::vector<unsigned int>& indices);
};
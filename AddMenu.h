#pragma once
#include <vector>
#include "Point.h"

class AddMenu {
public:
    bool isOpen;

    AddMenu();
    
    bool draw(std::vector<Point>& points, std::vector<unsigned int>& indices);
};
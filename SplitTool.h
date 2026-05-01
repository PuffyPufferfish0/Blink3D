#pragma once
#include "Point.h"
#include "Line.h"
#include <vector>

class SplitTool {
public:
    static bool execute(std::vector<Point>& points, std::vector<unsigned int>& indices, std::vector<Line>& lines);
};
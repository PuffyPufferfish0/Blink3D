#pragma once
#include "Point.h"
#include "Line.h"
#include <vector>

class SplitTool {
public:
    // Returns true if the topology was successfully modified
    static bool execute(std::vector<Point>& points, std::vector<unsigned int>& indices, std::vector<Line>& lines);
};
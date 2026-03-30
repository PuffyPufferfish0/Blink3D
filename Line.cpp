#include "Line.h"
#include <algorithm>

Line::Line(int v1, int v2) : selected(false) {
    // Always store the smaller index first so A->B and B->A are recognized as the same edge
    this->v1 = std::min(v1, v2);
    this->v2 = std::max(v1, v2);
}

void Line::select() { selected = true; }
void Line::deselect() { selected = false; }

bool Line::operator==(const Line& other) const {
    return (v1 == other.v1 && v2 == other.v2);
}
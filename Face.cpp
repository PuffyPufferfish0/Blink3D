#include "Face.h"

Face::Face(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3), selected(false) {}

void Face::select() { selected = true; }
void Face::deselect() { selected = false; }
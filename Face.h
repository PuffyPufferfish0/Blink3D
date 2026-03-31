#pragma once

class Face {
public:
    int v1;
    int v2;
    int v3;
    bool selected;

    Face(int v1, int v2, int v3);
    void select();
    void deselect();
};
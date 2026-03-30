#pragma once

class Line {
public:
    int v1;
    int v2;
    bool selected;

    Line(int v1, int v2);
    void select();
    void deselect();
    
    // Allows us to compare edges and avoid drawing duplicates!
    bool operator==(const Line& other) const;
};
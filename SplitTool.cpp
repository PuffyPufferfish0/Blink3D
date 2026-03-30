#include "SplitTool.h"

bool SplitTool::execute(std::vector<Point>& points, std::vector<unsigned int>& indices, std::vector<Line>& lines) {
    std::vector<std::pair<int, int>> toSplit;
    for (const auto& l : lines) {
        if (l.selected) toSplit.push_back({l.v1, l.v2});
    }
    
    if (toSplit.empty()) return false;

    // Process each selected line sequentially
    for (auto& pair : toSplit) {
        int splitV1 = pair.first;
        int splitV2 = pair.second;

        // 1. Calculate the exact midpoint and add the new vertex
        glm::vec3 midpoint = (points[splitV1].position + points[splitV2].position) * 0.5f;
        points.push_back(Point(midpoint));
        int mIdx = points.size() - 1;

        std::vector<unsigned int> newIndices;
        
        // 2. Scan every triangle in the mesh to see if it relied on this severed edge
        for (size_t i = 0; i < indices.size(); i += 3) {
            unsigned int t[3] = {indices[i], indices[i+1], indices[i+2]};
            int idx1 = -1, idx2 = -1;
            
            for (int j = 0; j < 3; j++) {
                if (t[j] == splitV1) idx1 = j;
                else if (t[j] == splitV2) idx2 = j;
            }

            if (idx1 != -1 && idx2 != -1) {
                // This triangle has the split edge! We must safely bisect it into two 
                // smaller triangles while maintaining the 3D winding order so the lighting works.
                newIndices.push_back(t[0] == splitV2 ? mIdx : t[0]);
                newIndices.push_back(t[1] == splitV2 ? mIdx : t[1]);
                newIndices.push_back(t[2] == splitV2 ? mIdx : t[2]);

                newIndices.push_back(t[0] == splitV1 ? mIdx : t[0]);
                newIndices.push_back(t[1] == splitV1 ? mIdx : t[1]);
                newIndices.push_back(t[2] == splitV1 ? mIdx : t[2]);
            } else {
                // Triangle is unaffected, pass it through
                newIndices.push_back(t[0]);
                newIndices.push_back(t[1]);
                newIndices.push_back(t[2]);
            }
        }
        indices = newIndices; // Replace the global mesh structure with the newly bisected one
    }
    return true;
}
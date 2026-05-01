#include "SplitTool.h"

bool SplitTool::execute(std::vector<Point>& points, std::vector<unsigned int>& indices, std::vector<Line>& lines) {
    std::vector<std::pair<int, int>> toSplit;
    for (const auto& l : lines) {
        if (l.selected) toSplit.push_back({l.v1, l.v2});
    }
    
    if (toSplit.empty()) return false;

    for (auto& pair : toSplit) {
        int splitV1 = pair.first;
        int splitV2 = pair.second;

        glm::vec3 midpoint = (points[splitV1].position + points[splitV2].position) * 0.5f;
        points.push_back(Point(midpoint));
        int mIdx = points.size() - 1;

        std::vector<unsigned int> newIndices;
        
        for (size_t i = 0; i < indices.size(); i += 3) {
            unsigned int t[3] = {indices[i], indices[i+1], indices[i+2]};
            int idx1 = -1, idx2 = -1;
            
            for (int j = 0; j < 3; j++) {
                if (t[j] == splitV1) idx1 = j;
                else if (t[j] == splitV2) idx2 = j;
            }

            if (idx1 != -1 && idx2 != -1) {
                newIndices.push_back(t[0] == splitV2 ? mIdx : t[0]);
                newIndices.push_back(t[1] == splitV2 ? mIdx : t[1]);
                newIndices.push_back(t[2] == splitV2 ? mIdx : t[2]);

                newIndices.push_back(t[0] == splitV1 ? mIdx : t[0]);
                newIndices.push_back(t[1] == splitV1 ? mIdx : t[1]);
                newIndices.push_back(t[2] == splitV1 ? mIdx : t[2]);
            } else {
                newIndices.push_back(t[0]);
                newIndices.push_back(t[1]);
                newIndices.push_back(t[2]);
            }
        }
        indices = newIndices; 
    }
    return true;
}
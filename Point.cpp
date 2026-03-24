#include "Point.h"

const glm::vec3 Point::COLOR_DEFAULT = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 Point::COLOR_SELECTED = glm::vec3(1.0f, 1.0f, 0.0f);

Point::Point(glm::vec3 pos) : position(pos), selected(false), color(COLOR_DEFAULT) {}

void Point::select() {
    selected = true;
    updateState();
}

void Point::deselect() {
    selected = false;
    updateState();
}

void Point::updateState() {
    color = selected ? COLOR_SELECTED : COLOR_DEFAULT;
}
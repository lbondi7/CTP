#include "Triangle.h"

#include <algorithm>

Triangle::Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 _pos)
{
	vertices[0] = v1;
	vertices[1] = v2;
	vertices[2] = v3;

	pos = _pos;
	center = (v1 + v2 + v3) / 3.0f;
	edges[0] = vertices[1] - vertices[0];
	edges[1] = vertices[2] - vertices[1];
	edges[2] = vertices[0] - vertices[2];

	normal = glm::normalize(glm::cross(edges[0], edges[1]));

}


float Triangle::dot2(glm::vec3 v) { return glm::dot(v, v); }

float Triangle::udTriangle(glm::vec3 p)
{
    // prepare data    
    glm::vec3 p1 = p - vertices[0];
    glm::vec3 p2 = p - vertices[1];
    glm::vec3 p3 = p - vertices[2];


    return sqrt( // inside/outside test    
        (sin(dot(cross(edges[0], normal), p1)) +
            sin(dot(cross(edges[1], normal), p2)) +
            sin(dot(cross(edges[2], normal), p3)) < 2.0)
        ?
        // 3 edges    
        std::min(std::min(
            dot2(edges[0] * std::clamp(glm::dot(edges[0], p1) / dot2(edges[0]), 0.0f, 1.0f) - p1),
            dot2(edges[1] * std::clamp(glm::dot(edges[1], p2) / dot2(edges[1]), 0.0f, 1.0f) - p2)),
            dot2(edges[2] * std::clamp(glm::dot(edges[2], p3) / dot2(edges[2]), 0.0f, 1.0f) - p3))
        :
        // 1 face    
        glm::dot(normal, p1) * glm::dot(normal, p1) / dot2(normal));
}
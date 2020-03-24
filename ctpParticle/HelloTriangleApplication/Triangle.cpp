#include "Triangle.h"
#include "Utilities.h"

#include <algorithm>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

Triangle::Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, const Transform& transform)
{
	localVertices[0] = v1;
	localVertices[1] = v2;
	localVertices[2] = v3;

    for (size_t i = 0; i < 3; i++)
    {
        vertices[i] = glm::vec4(localVertices[i], 0.0f)
            * glm::translate(glm::mat4(1.0f), transform.pos);
    }

    edges[0] = vertices[1] - vertices[0];
    edges[1] = vertices[2] - vertices[1];
    edges[2] = vertices[0] - vertices[2];

    center = (vertices[0] + vertices[1] + vertices[2]) / 3.0f;

    normal = glm::normalize(glm::cross(edges[0], edges[2]));

    max.x = std::max(std::max(vertices[0].x, vertices[1].x), vertices[2].x);
    max.y = std::max(std::max(vertices[0].y, vertices[1].y), vertices[2].y);
    max.z = std::max(std::max(vertices[0].z, vertices[1].z), vertices[2].z);

    min.x = std::min(std::min(vertices[0].x, vertices[1].x), vertices[2].x);
    min.y = std::min(std::min(vertices[0].y, vertices[1].y), vertices[2].y);
    min.z = std::min(std::min(vertices[0].z, vertices[1].z), vertices[2].z);
}

void Triangle::Update(const glm::vec3& p)
{
    for (size_t i = 0; i < 3; i++)
    {
        vertices[i] = p + localVertices[i];
    }

    edges[0] = vertices[1] - vertices[0];
    edges[1] = vertices[2] - vertices[1];
    edges[2] = vertices[0] - vertices[2];

    center = (vertices[0] + vertices[1] + vertices[2]) / 3.0f;

    //normal = glm::cross(edges[0], edges[1]);

    max.x = std::max(std::max(vertices[0].x, vertices[1].x), vertices[2].x);
    max.y = std::max(std::max(vertices[0].y, vertices[1].y), vertices[2].y);
    max.z = std::max(std::max(vertices[0].z, vertices[1].z), vertices[2].z);

    min.x = std::min(std::min(vertices[0].x, vertices[1].x), vertices[2].x);
    min.y = std::min(std::min(vertices[0].y, vertices[1].y), vertices[2].y);
    min.z = std::min(std::min(vertices[0].z, vertices[1].z), vertices[2].z);
}

float Triangle::udTriangle(glm::vec3 p)
{
    // prepare data    
    glm::vec3 p1 = p - vertices[0];
    glm::vec3 p2 = p - vertices[1];
    glm::vec3 p3 = p - vertices[2];


    return sqrt( // inside/outside test    
        (sin(dot(cross(edges[0], normal), p1)) +
            sin(dot(cross(edges[1], normal), p2)) +
            sin(dot(cross(edges[2], normal), p3)) < 2.0f)
        ?
        // 3 edges    
        std::min(std::min(
            Utilities::Dot2(edges[0] * std::clamp(glm::dot(edges[0], p1) / Utilities::Dot2(edges[0]), 0.0f, 1.0f) - p1),
            Utilities::Dot2(edges[1] * std::clamp(glm::dot(edges[1], p2) / Utilities::Dot2(edges[1]), 0.0f, 1.0f) - p2)),
            Utilities::Dot2(edges[2] * std::clamp(glm::dot(edges[2], p3) / Utilities::Dot2(edges[2]), 0.0f, 1.0f) - p3))
        :
        // 1 face    
        glm::dot(normal, p1) * glm::dot(normal, p1) / Utilities::Dot2(normal));
}

void Triangle::operator=(const Triangle& other)
{
    center = other.center;
    localVertices = other.localVertices;
    vertices = other.vertices;
    edges = other.edges;
    normal = other.normal;
    min = other.min;
    max = other.max;
}

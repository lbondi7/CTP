#include "Triangle.h"
#include "Utillities.h"

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

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    //modelMatrix = glm::scale(modelMatrix, transform.scale);
    modelMatrix = glm::translate(modelMatrix, transform.pos);
    for (size_t i = 0; i < 3; i++)
    {
        vertices[i] = glm::vec4(localVertices[i], 1.0f) * modelMatrix;
    }

    edges[0] = vertices[1] - vertices[0];
    edges[1] = vertices[2] - vertices[1];
    edges[2] = vertices[0] - vertices[2];

    center = (vertices[0] + vertices[1] + vertices[2]) / 3.0f;

    normal = glm::normalize(glm::cross(edges[0], edges[1]));

    max.x = std::max(std::max(vertices[0].x, vertices[1].x), vertices[2].x);
    max.y = std::max(std::max(vertices[0].y, vertices[1].y), vertices[2].y);
    max.z = std::max(std::max(vertices[0].z, vertices[1].z), vertices[2].z);

    min.x = std::min(std::min(vertices[0].x, vertices[1].x), vertices[2].x);
    min.y = std::min(std::min(vertices[0].y, vertices[1].y), vertices[2].y);
    min.z = std::min(std::min(vertices[0].z, vertices[1].z), vertices[2].z);
}

void Triangle::Update(const glm::vec3& p)
{
    //for (size_t i = 0; i < 3; i++)
    //{
    //    vertices[i] = p + localVertices[i];
    //}

    //edges[0] = vertices[1] - vertices[0];
    //edges[1] = vertices[2] - vertices[0];
    //edges[2] = vertices[0] - vertices[2];

    //center = (vertices[0] + vertices[1] + vertices[2]) / 3.0f;

    //normal = glm::cross(edges[0], edges[1]);

    //max.x = std::max(std::max(vertices[0].x, vertices[1].x), vertices[2].x);
    //max.y = std::max(std::max(vertices[0].y, vertices[1].y), vertices[2].y);
    //max.z = std::max(std::max(vertices[0].z, vertices[1].z), vertices[2].z);

    //min.x = std::min(std::min(vertices[0].x, vertices[1].x), vertices[2].x);
    //min.y = std::min(std::min(vertices[0].y, vertices[1].y), vertices[2].y);
    //min.z = std::min(std::min(vertices[0].z, vertices[1].z), vertices[2].z);
}

float Triangle::udTriangle(glm::vec3 p)
{
    // prepare data    
    glm::vec3 p1 = p - vertices[0];
    glm::vec3 p2 = p - vertices[1];
    glm::vec3 p3 = p - vertices[2];


    if (std::sin(glm::dot(glm::cross(edges[0], normal), p1)) +
        std::sin(glm::dot(glm::cross(edges[1], normal), p2)) +
        std::sin(glm::dot(glm::cross(edges[2], normal), p3)) < 2.0f)
    {
        return sqrt(std::min(std::min(
            Utillities::Dot2(edges[0] * std::clamp(glm::dot(edges[0], p1) / Utillities::Dot2(edges[0]), 0.0f, 1.0f) - p1),
            Utillities::Dot2(edges[1] * std::clamp(glm::dot(edges[1], p2) / Utillities::Dot2(edges[1]), 0.0f, 1.0f) - p2)),
            Utillities::Dot2(edges[2] * std::clamp(glm::dot(edges[2], p3) / Utillities::Dot2(edges[2]), 0.0f, 1.0f) - p3)));
    }
    else
    {
        return sqrt(glm::dot(normal, p1) * glm::dot(normal, p1) / Utillities::Dot2(normal));
    }

    //return sqrt( // inside/outside test    
    //    (std::sin(glm::dot(glm::cross(edges[0], normal), p1)) +
    //        std::sin(glm::dot(glm::cross(edges[1], normal), p2)) +
    //        std::sin(glm::dot(glm::cross(edges[2], normal), p3)) < 2.0f)
    //    ?
    //    // 3 edges    
    //    std::min(std::min(
    //        Utillities::Dot2(edges[0] * std::clamp(glm::dot(edges[0], p1) / Utillities::Dot2(edges[0]), 0.0f, 1.0f) - p1),
    //        Utillities::Dot2(edges[1] * std::clamp(glm::dot(edges[1], p2) / Utillities::Dot2(edges[1]), 0.0f, 1.0f) - p2)),
    //        Utillities::Dot2(edges[2] * std::clamp(glm::dot(edges[2], p3) / Utillities::Dot2(edges[2]), 0.0f, 1.0f) - p3))
    //    :
    //    // 1 face    
    //    glm::dot(normal, p1) * glm::dot(normal, p1) / Utillities::Dot2(normal));
}

float Triangle::UdTriangle(glm::vec3 p)
{
    glm::vec3 ba = vertices[1] - vertices[0]; glm::vec3 pa = p - vertices[0];
    glm::vec3 cb = vertices[2] - vertices[1]; glm::vec3 pb = p - vertices[1];
    glm::vec3 ac = vertices[0] - vertices[2]; glm::vec3 pc = p - vertices[2];
    glm::vec3 nor = glm::cross(ba, ac);

    return sqrt(
        (sin(glm::dot(glm::cross(ba, nor), pa)) +
            sin(glm::dot(glm::cross(cb, nor), pb)) +
            sin(glm::dot(glm::cross(ac, nor), pc)) < 2.0)
        ?
        std::min(std::min(
            Utillities::Dot2(ba * std::clamp(glm::dot(ba, pa) / Utillities::Dot2(ba), 0.0f, 1.0f) - pa),
            Utillities::Dot2(cb * std::clamp(glm::dot(cb, pb) / Utillities::Dot2(cb), 0.0f, 1.0f) - pb)),
            Utillities::Dot2(ac * std::clamp(glm::dot(ac, pc) / Utillities::Dot2(ac), 0.0f, 1.0f) - pc))
        :
        dot(nor, pa) * dot(nor, pa) / Utillities::Dot2(nor));
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

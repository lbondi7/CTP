#pragma once

#include "FlowFieldObject.h"
#include "Bounds.h"
#include "ParticleSystem.h"

#include <memory>
#include <vector>

struct KdNode {

	KdNode() = default;
	~KdNode() {};


	int axis_chose = 0;
	int level = 0;
	bool isLeaf = false;
	Bounds bounds;
	std::vector<Triangle*> tris;
	std::unique_ptr<KdNode> leftNode;
	std::unique_ptr<KdNode> rightNode;
};

class KdTree
{
public:
	KdTree() = default;;
	~KdTree();


	void Init(std::vector<Triangle>& tris);

	void Init(FfObject& ffObject);

	void BuildKdNode(int prevAxis, KdNode* _node);

	void SortTrisByAxis(int axis, KdNode* _node);

	void Traverse(Particle* particle, Triangle& nearestTri);

	bool Traverse(Particle* particle, Triangle& nearestTri, KdNode* _node);

	void Traverse2(Particle* particle);

	bool Traverse2(Particle* particle, KdNode* _node);

private:

	std::unique_ptr<KdNode> firstNode;
	int maxTriCount = 3;
	glm::vec3 center;

};


#include "KdTree.h"
#include "Utillities.h"
#include "Locator.h"
#include "Timer.h"

#include <random>


KdTree::~KdTree()
{
}

void KdTree::Init(std::vector<Triangle>& tris)
{
	std::random_device rd;
	std::uniform_int_distribution<int>random_axis(0, 2);
	firstNode = std::make_unique<KdNode>();

	int axis = random_axis(rd);

	for (int i  = 0; i < tris.size(); ++i)
	{
		firstNode->tris.emplace_back(&tris[i]);
	}
	BuildKdNode(axis, firstNode.get());
}

void KdTree::Init(FfObject& ffObject)
{
	std::random_device rd;
	std::uniform_int_distribution<int>random_axis(0, 2);
	
	center = ffObject.GetTransform().position;
	firstNode = std::make_unique<KdNode>();

	int axis = random_axis(rd);

	for (int i = 0; i < ffObject.triangles.size(); ++i)
	{
		firstNode->tris.emplace_back(&ffObject.triangles[i]);
	}
	BuildKdNode(axis, firstNode.get());
}

void KdTree::BuildKdNode(int prevAxis, KdNode* _node)
{
	std::random_device rd;
	std::uniform_int_distribution<int>random_axis(0, 2);

	int axis;

	do
	{
		axis = random_axis(rd);
	} while (axis == prevAxis);

	_node->axis_chose = axis;

	_node->bounds.Load(_node->tris);

	if (_node->tris.size() > maxTriCount)
	{
		SortTrisByAxis(axis, _node);

		_node->leftNode = std::make_unique<KdNode>();
		_node->rightNode = std::make_unique<KdNode>();

		int pointofSplit = (_node->tris.size() / 2);
		_node->leftNode->tris.insert(_node->leftNode->tris.begin(), _node->tris.begin(),
			_node->tris.begin() + pointofSplit);
		_node->rightNode->tris.insert(_node->rightNode->tris.begin(), _node->tris.begin() + pointofSplit + 1,
			_node->tris.end());

		_node->leftNode->level = _node->level + 1;
		_node->rightNode->level = _node->leftNode->level;
		BuildKdNode(axis, _node->leftNode.get());
		BuildKdNode(axis, _node->rightNode.get());
	}
	else
	{
		_node->isLeaf = true;
	}
}

static bool sortTriX(const Triangle* tri1, const Triangle* tri2)
{
	return tri1->min.x < tri2->min.x;
}

static bool sortTriY(const Triangle* tri1, const Triangle* tri2)
{
	return tri1->min.y < tri2->min.y;
}

static bool sortTriZ(const Triangle* tri1, const Triangle* tri2)
{
	return tri1->min.z < tri2->min.z;
}

void KdTree::SortTrisByAxis(int axis, KdNode* _node)
{
	if(axis == 0)
		std::sort(_node->tris.begin(), _node->tris.end(), sortTriX);
	if (axis == 1)
		std::sort(_node->tris.begin(), _node->tris.end(), sortTriY);
	if (axis == 2)
		std::sort(_node->tris.begin(), _node->tris.end(), sortTriZ);
}

void KdTree::Traverse(Particle* particle, Triangle& nearestTri)
{
	Traverse(particle, nearestTri, firstNode.get());
}

bool KdTree::Traverse(Particle* particle, Triangle& nearestTri, KdNode* _node)
{
	if (_node->bounds.IsInside(particle->position))
	{
		if (_node->isLeaf)
		{
			//float nearestPoint = INFINITY;
			//float nP = INFINITY;
			//int nearestOne = 0;
			//for (size_t j = 0; j < _node->tris.size(); ++j)
			//{
			//	nP = _node->tris[j]->udTriangle(particle->position);
			//	if (nearestPoint > nP)
			//	{
			//		nearestPoint = nP;
			//		nearestOne = j;
			//	}
			//}
			//nearestTri = *_node->tris[nearestOne];
			//glm::vec3 point = _node->tris[nearestOne]->FindPoint();
			//particle->target = point;
			//particle->velocity = glm::normalize(point - particle->position) * particle->speed;
			//particle->goToTri = true;
			//return true;
		}
		else
		{
			if (Traverse(particle, nearestTri, _node->leftNode.get()))
				return true;

			if(Traverse(particle, nearestTri, _node->rightNode.get()))
				return true;
			//else
			//{
			//	float nearestPoint = INFINITY;
			//	float nP = INFINITY;
			//	int nearestOne = -1;
			//	for (auto& t: _node->tris)
			//	{
			//		nP = t->udTriangle(particle->position);
			//		if (nearestPoint > nP)
			//		{
			//			nearestPoint = nP;
			//			++nearestOne;
			//		}
			//	}
			//	nearestTri = *_node->tris[nearestOne];
			//	glm::vec3 point = _node->tris[nearestOne]->FindPoint();
			//	particle->target = point;
			//	particle->velocity = glm::normalize(point - particle->position) * particle->speed;
			//	particle->goToTri = true;
			//	return true;
			//}
		}
	}
	else if (_node->level == 0)
	{
		//particle->target = center;
		//particle->velocity = glm::normalize(center - particle->position) * particle->speed;
		//particle->goToTri = true;
	}
	return false;
}

void KdTree::Traverse2(Particle* particle)
{
	Traverse2(particle,firstNode.get());
}

bool KdTree::Traverse2(Particle* particle, KdNode* _node)
{
	if (_node->bounds.IsInside(particle->position))
	{
		if (_node->isLeaf)
		{
			int count = 0;
			glm::vec3 vel(0.0);
			for (auto& tri : _node->tris)
			{
				//auto triDist = glm::length(particle->position - tri->center);
				//if (triDist < 5.0f)
				//{
				//	auto point = tri->udTriangle(particle->position);
				//	auto triDist2 = glm::length(particle->position - point);

				//	auto dir = glm::normalize(particle->position - point);
				//	triDist2 = 5.0f - triDist2;
				//	dir = glm::normalize(particle->dir + dir);
				//	particle->dir += dir;
				//	particle->speed = triDist2;

				//	//dir = (dir * triDist2 / 10.0f);
				//	//dir *= Locator::GetTimer()->FixedDeltaTime();
				//	//particle->velocity += dir;
				//}
			}
			return true;
		}
		else
		{
			if (Traverse2(particle, _node->leftNode.get()))
				return true;

			if (Traverse2(particle, _node->rightNode.get()))
				return true;
		}
	}
	return false;
}

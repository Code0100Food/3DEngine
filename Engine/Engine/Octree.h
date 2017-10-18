#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "Globals.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Geometry/AABB.h"

#define NODE_SUBDIVISION 8

/// Class OctItem -------------------------------
//Class that contains the data and its location
template <class DATA_TYPE>
struct OctItem
{
public:

	OctItem() {}
	OctItem(const DATA_TYPE& data, const math::float3& location) : data(data), location(location) {}

public:

	math::float3 location = { 0,0,0 };
	DATA_TYPE data = NULL;

public:

	bool operator == (const OctItem& target)
	{
		return (location == location && data == data);
	}
};
/// ---------------------------------------------

/// Class OctCube -------------------------------
// This are the cubes used by the octree to subdivide the space (not mutable)
template <class DATA_TYPE>
class OctCube
{
public:

	//Constructors ========================
	OctCube(const math::AABB& aabb, uint max_objects) : aabb(aabb), max_objects(max_objects)
	{
		for (uint i = 0; i < NODE_SUBDIVISION; i++)
		{
			children[i] = nullptr;
		}
	}

	//Destructors =========================
	~OctCube()
	{

		for (int i = 0; i < NODE_SUBDIVISION; i++)
		{
			if (children[i] != nullptr)
			{
				RELEASE(children[i]);
			}
		}
		this->objects.clear();
	}

public:

	math::AABB							aabb;
	std::vector<OctItem<DATA_TYPE>>		objects;
	uint								max_objects = 0;
	bool								full = false;
	OctCube*							root = nullptr;
	OctCube*							children[NODE_SUBDIVISION];

public:

	/*// Functionality =======================
	void Draw(const SDL_Color& color)const
	{
		App->render->DrawQuad(aabb, color.r, color.g, color.b, color.a, false);


		if (!full)return;
		for (uint k = 0; k < NODE_SUBDIVISION; k++)
		{
			children[k]->Draw(color);
		}
	}

	bool Insert(DATA_TYPE data, const iPoint* point)
	{
		// If new point is not in the quad-tree AABB, return
		SDL_Point p = { point->x,point->y };
		if (!SDL_PointInRect(&p, &aabb))
		{
			return false;
		}

		if (full)
		{
			for (uint i = 0; i < NODE_SUBDIVISION; i++)
			{
				if (children[i]->Insert(data, point))
				{
					return true;
				}
			}
			return false;
		}

		// If in this node there is space for the point, pushback it
		uint size = objects.size();
		if (size < max_objects)
		{
			TreeItem<DATA_TYPE> item(data, *point);
			objects.push_back(item);
			if (size + 1 == max_objects)
			{
				full = true;
				Subdivide();
			}
			return true;
		}

		return false;
	}

	void Subdivide()
	{
		// Get subdivision size
		iPoint qSize = { (int)floor(aabb.w * 0.5), (int)floor(aabb.h * 0.5) };
		iPoint qCentre;

		//Calculate new AABB center for each child
		qCentre = { aabb.x,aabb.y };
		children[0] = new AABB({ qCentre.x,qCentre.y,qSize.x,qSize.y }, max_objects);
		children[0]->root = this;


		qCentre = { aabb.x + qSize.x,aabb.y };
		children[1] = new AABB({ qCentre.x,qCentre.y,qSize.x,qSize.y }, max_objects);
		children[1]->root = this;

		qCentre = { aabb.x,aabb.y + qSize.y };
		children[2] = new AABB({ qCentre.x,qCentre.y,qSize.x,qSize.y }, max_objects);
		children[2]->root = this;

		qCentre = { aabb.x + qSize.x,aabb.y + qSize.y };
		children[3] = new AABB({ qCentre.x,qCentre.y,qSize.x,qSize.y }, max_objects);
		children[3]->root = this;

		for (uint h = 0; h < max_objects; h++)
		{
			for (uint k = 0; k < NODE_SUBDIVISION; k++)
			{
				if (children[k]->Insert(objects[h].data, &objects[h].location)) break;
			}
		}
		objects.clear();
	}

	int CollectCandidates(std::vector<DATA_TYPE>& nodes, const SDL_Rect& rect)
	{
		uint ret = 0;

		// If range is not in the quad-tree, return
		if (!SDL_HasIntersection(&rect, &aabb)) return 0;

		// See if the points of this node are in range and pushback them to the vector
		if (full)
		{
			// Otherwise, add the points from the children
			ret += children[0]->CollectCandidates(nodes, rect);
			ret += children[1]->CollectCandidates(nodes, rect);
			ret += children[2]->CollectCandidates(nodes, rect);
			ret += children[3]->CollectCandidates(nodes, rect);
		}
		else
		{
			uint size = objects.size();
			for (uint k = 0; k < size; k++)
			{
				SDL_Point pos = { objects[k].location.x,objects[k].location.y };
				if (SDL_PointInRect(&pos, &rect))
				{
					nodes.push_back(objects[k].data);
					ret++;
				}
			}
		}

		return ret;
	}

	int CollectCandidates(std::vector<DATA_TYPE>& nodes, const Circle& circle)
	{
		uint ret = 0;

		// If range is not in the quad-tree, return
		if (!circle.Intersects(&aabb))return 0;

		// See if the points of this node are in range and pushback them to the vector
		if (full)
		{
			// Otherwise, add the points from the children
			ret += children[0]->CollectCandidates(nodes, circle);
			ret += children[1]->CollectCandidates(nodes, circle);
			ret += children[2]->CollectCandidates(nodes, circle);
			ret += children[3]->CollectCandidates(nodes, circle);
		}
		else
		{
			uint size = objects.size();
			for (uint k = 0; k < size; k++)
			{
				fPoint loc(objects[k].location.x, objects[k].location.y);

				if (circle.IsIn(&loc))
				{
					nodes.push_back(objects[k].data);
					ret++;
				}

			}
		}

		return ret;
	}

	int CollectCandidates(std::list<DATA_TYPE>& nodes, const Circle& circle)
	{
		uint ret = 0;

		// If range is not in the quad-tree, return
		if (!circle.Intersects(&aabb))return 0;

		// See if the points of this node are in range and pushback them to the vector
		if (full)
		{
			// Otherwise, add the points from the children
			ret += children[0]->CollectCandidates(nodes, circle);
			ret += children[1]->CollectCandidates(nodes, circle);
			ret += children[2]->CollectCandidates(nodes, circle);
			ret += children[3]->CollectCandidates(nodes, circle);
		}
		else
		{
			uint size = objects.size();
			for (uint k = 0; k < size; k++)
			{
				fPoint loc(objects[k].location.x, objects[k].location.y);

				if (circle.IsIn(&loc))
				{
					nodes.push_back(objects[k].data);
					ret++;
				}

			}
		}

		return ret;
	}*/
};
/// ---------------------------------------------

/// Class Octree --------------------------------
template <class DATA_TYPE>
class Octree
{
public:

	//Constructors ========================
	Octree() {};

	Octree(const math::AABB& box, uint max_objects = 0)
	{
		SetBoundaries(box);
	}

	//Destructors =========================
	~Octree()
	{
		//Clear();
	}

private:

	OctCube<DATA_TYPE>*	root = nullptr;
	uint				max_objects = 0;
	math::float3		color = { 255,255,255 };

public:

	/*// Functionality =======================
	void SetBoundaries(const SDL_Rect& r)
	{
		if (root != NULL)
		{
			root->aabb = r;
		}
		else
		{
			root = new AABB<DATA_TYPE>(r, max_objects);
		}
	}

	void SetMaxObjects(uint max)
	{
		max_objects = max;
		root->max_objects = max;
	}

	void SetDebugColor(const SDL_Color& new_color)
	{
		color = new_color;
	}

	bool Insert(DATA_TYPE data, const iPoint* newpoint)
	{
		if (root != NULL)
		{
			return root->Insert(data, newpoint);
		}
		return false;
	}

	void Draw()const
	{
		root->Draw(color);
	}

	int	CollectCandidates(std::vector<DATA_TYPE>& nodes, const SDL_Rect& r) const
	{
		int tests = 0;

		if (root != NULL && SDL_HasIntersection(&r, &root->aabb))
			tests = root->CollectCandidates(nodes, r);
		return tests;
	}

	void Clear()
	{
		if (root != NULL)
		{
			delete root;
			root = NULL;
		}
	}

	int	CollectCandidates(std::vector<DATA_TYPE>& nodes, const Circle& circle) const
	{
		int tests = 0;

		if (root != NULL && circle.Intersects(&root->aabb))
			tests = root->CollectCandidates(nodes, circle);

		return tests;
	}

	int	CollectCandidates(std::list<DATA_TYPE>& nodes, const Circle& circle) const
	{
		int tests = 0;

		if (root != NULL && circle.Intersects(&root->aabb))
			tests = root->CollectCandidates(nodes, circle);

		return tests;
	}*/
};
/// ---------------------------------------------
#endif
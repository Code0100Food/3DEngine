#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "Globals.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Geometry/AABB.h"
#include "MathGeoLib/Geometry/Frustum.h"

#define NODE_SUBDIVISION 8

/// Class OctItem -------------------------------
//Class that contains the data and its location
template <class DATA_TYPE>
struct OctItem
{
public:

	OctItem() {}
	OctItem(const DATA_TYPE& data, const math::AABB& bounding_box) : data(data), bounding_box(bounding_box) {}

public:

	math::AABB	bounding_box;
	DATA_TYPE	data = NULL;

public:

	bool operator == (const OctItem& target)
	{
		return (location == location && data == data);
	}

	bool CheckContact(math::Frustum frus)const
	{
		return !frus.VertexOutside(bounding_box);
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

	// Functionality =======================
	void Draw(float color[4])const
	{
		aabb.Draw(2.5f, color);

		if (!full)return;
		for (uint k = 0; k < NODE_SUBDIVISION; k++)
		{
			children[k]->Draw(color);
		}
	}

	bool Insert(DATA_TYPE data,const math::AABB& bb)
	{
		// If new point is not in the quad-tree AABB, return
		if (!CheckInsert(bb) || !bb.IsFinite())
		{
			return false;
		}

		if (full)
		{
			int contacts = 0;
			int index = 0;
			for (uint i = 0; i < NODE_SUBDIVISION; i++)
			{
				if (children[i]->CheckInsert(bb))
				{
					contacts++;
					index = i;
				}
			}
			if (contacts == 1)
			{
				children[index]->Insert(data, bb);
			}
			else if (contacts > 1)
			{
				OctItem<DATA_TYPE> item(data, bb);
				objects.push_back(item);
			}
			return false;
		}

		// If in this node there is space for the box, push it
		else
		{
			uint size = objects.size();
			if (size <= max_objects)
			{
				OctItem<DATA_TYPE> item(data, bb);
				objects.push_back(item);
				if (size == max_objects)
				{
					full = true;
					Subdivide();
				}
				return true;
			}
		}

		return false;
	}

	bool CheckInsert(const math::AABB& bb)const
	{
		return aabb.Contact(bb);
	}

	bool CheckContact(const math::Frustum& frus)const
	{
		return !frus.VertexOutside(aabb);
	}

	void Subdivide()
	{
		//Variables used to allocate the different divisions data
		float			cube_size = aabb.HalfSize().x;
		math::AABB		temp_ab;

		//Calculate new AABB for each child
		temp_ab.maxPoint = aabb.CenterPoint();
		temp_ab.minPoint = { temp_ab.maxPoint.x - cube_size, temp_ab.maxPoint.y - cube_size,temp_ab.maxPoint.z - cube_size };
		children[0] = new OctCube(temp_ab, max_objects);
		children[0]->root = this;

		temp_ab.maxPoint = aabb.CenterPoint();
		temp_ab.minPoint = { temp_ab.maxPoint.x + cube_size,temp_ab.maxPoint.y - cube_size,temp_ab.maxPoint.z - cube_size };
		children[1] = new OctCube(temp_ab, max_objects);
		children[1]->root = this;

		temp_ab.maxPoint = aabb.CenterPoint();
		temp_ab.minPoint = { temp_ab.maxPoint.x - cube_size,temp_ab.maxPoint.y + cube_size,temp_ab.maxPoint.z - cube_size };
		children[2] = new OctCube(temp_ab, max_objects);
		children[2]->root = this;

		temp_ab.maxPoint = aabb.CenterPoint();
		temp_ab.minPoint = { temp_ab.maxPoint.x - cube_size, temp_ab.maxPoint.y - cube_size,temp_ab.maxPoint.z + cube_size };
		children[3] = new OctCube(temp_ab, max_objects);
		children[3]->root = this;

		temp_ab.maxPoint = aabb.CenterPoint();
		temp_ab.minPoint = { temp_ab.maxPoint.x + cube_size,temp_ab.maxPoint.y + cube_size,temp_ab.maxPoint.z + cube_size };
		children[4] = new OctCube(temp_ab, max_objects);
		children[4]->root = this;

		temp_ab.maxPoint = aabb.CenterPoint();
		temp_ab.minPoint = { temp_ab.maxPoint.x + cube_size,temp_ab.maxPoint.y + cube_size,temp_ab.maxPoint.z - cube_size };
		children[5] = new OctCube(temp_ab, max_objects);
		children[5]->root = this;

		temp_ab.maxPoint = aabb.CenterPoint();
		temp_ab.minPoint = { temp_ab.maxPoint.x + cube_size,temp_ab.maxPoint.y - cube_size,temp_ab.maxPoint.z + cube_size };
		children[6] = new OctCube(temp_ab, max_objects);
		children[6]->root = this;

		temp_ab.maxPoint = aabb.CenterPoint();
		temp_ab.minPoint = { temp_ab.maxPoint.x - cube_size,temp_ab.maxPoint.y + cube_size,temp_ab.maxPoint.z + cube_size };
		children[7] = new OctCube(temp_ab, max_objects);
		children[7]->root = this;


		//Re insert the objects to check if can ve inserted in the generated childs
		std::vector<OctItem<DATA_TYPE>> objects_cpy = objects;
		objects.clear();

		for (uint h = 0; h < objects_cpy.size(); h++)
		{
			int index = 0;
			int contacts = 0;

			for (uint i = 0; i < NODE_SUBDIVISION; i++)
			{
				if (children[i]->CheckInsert(objects_cpy[h].bounding_box))
				{
					contacts++;
					index = i;
				}
			}
			if (contacts == 1)
			{
				children[index]->Insert(objects_cpy[h].data, objects_cpy[h].bounding_box);
			}
			else if (contacts > 1)
			{
				OctItem<DATA_TYPE> item(objects_cpy[h].data, objects_cpy[h].bounding_box);
				objects.push_back(item);
			}
		}

		//Clear the objects copy vector
		objects_cpy.clear();
	}

	void CollectCandidates(math::Frustum& frustum, std::queue<DATA_TYPE>& queue)
	{
		if (!CheckContact(frustum))return;

		if (full)
		{
			for (uint k = 0; k < NODE_SUBDIVISION; k++)children[k]->CollectCandidates(frustum, queue);
		}

		for (uint k = 0; k < objects.size(); k++)
		{
			if(objects[k].CheckContact(frustum))
			{
				queue.push(objects[k].data);
			}
		}
		
	}
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
		Clear();
	}

private:

	OctCube<DATA_TYPE>*	root = nullptr;
	uint				max_objects = 0;

public:

	// Functionality =======================
	void SetBoundaries(const math::AABB& r)
	{
		if (root != NULL)
		{
			root->aabb = r;
		}
		else
		{
			root = new OctCube<DATA_TYPE>(r, max_objects);
		}
	}

	void SetMaxObjects(uint max)
	{
		max_objects = max;
		root->max_objects = max;
	}


	bool Insert(const DATA_TYPE data, const math::AABB& new_bb)
	{
		if (root != NULL)
		{
			return root->Insert(data, new_bb);
		}
		return false;
	}


	void Draw()const
	{
		float color[4] = { 0.4f,0.8f,0.2f,1.0f };
		if (root != NULL)root->Draw(color);
	}

	void CollectCandidates(math::Frustum& frustum, std::queue<DATA_TYPE>& queue)
	{
		if (root != NULL && !frustum.VertexOutside(root->aabb))
		{
			root->CollectCandidates(frustum, queue);
		}
	}

	void Clear()
	{
		if (root != NULL)
		{
			RELEASE(root);
		}
	}

	void Reset()
	{
		if (root != NULL && root->children[0] != nullptr)
		{
			for (uint k = 0; k < NODE_SUBDIVISION; k++)RELEASE(root->children[k]);
			root->full = false;
		}
	}
};
/// ---------------------------------------------
#endif
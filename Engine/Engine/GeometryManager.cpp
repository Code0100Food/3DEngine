#include "GeometryManager.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

// Constructors =================================
GeometryManager::GeometryManager(const char * _name, MODULE_ID _id, bool _config_menu, bool _enabled) :Module(_name, _id, _enabled)
{

}

// Destructors ==================================
GeometryManager::~GeometryManager()
{

}

// Game Loop ====================================
bool GeometryManager::Start()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
	
	const aiScene* scene = aiImportFile("Assets/warrior.FBX", aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		uint size = scene->mNumMeshes;
		for (uint k = 0; k < size; k++)
		{
			//Pointer to the current mesh
			aiMesh* m = scene->mMeshes[k];
			
			//Create a new mesh
			Mesh_* mesh = (Mesh_*)CreatePrimitive(PRIMITIVE_TYPE::PRIMITIVE_MESH);
			
			//Load vertex data
			uint num_vertex = mesh->mesh.num_vertices = m->mNumVertices;
			mesh->mesh.vertices = new float[num_vertex * 3];
			memcpy(mesh->mesh.vertices, m->mVertices, sizeof(float) * num_vertex * 3);
			
			LOG("Created Mesh with %i vertices", num_vertex);

			//Load index data
			if (m->HasFaces())
			{
				mesh->mesh.num_indices = m->mNumFaces * 3;
				mesh->mesh.indices = new uint[mesh->mesh.num_indices];
				for (uint h = 0; h < m->mNumFaces; h++)
				{
					if (m->mFaces[h].mNumIndices != 3)
					{
						LOG("[error] Geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&mesh->mesh.indices[h * 3], m->mFaces[h].mIndices, 3 * sizeof(uint));
					}
				}
			}

			//Initialize the loaded mesh
			mesh->Initialize();

		}

		//Release the scene
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene");
	}

	return true;
}

bool GeometryManager::Draw()
{
	//Enable the vertex and elements flags
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);

	std::list<Primitive_*>::const_iterator geom = geometry_list.begin();
	while (geom != geometry_list.end())
	{
		geom._Ptr->_Myval->Draw();

		geom++;
	}

	//Reset the buffers focus
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Disable the vertex and elements flags
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);

	return true;
}

bool GeometryManager::CleanUp()
{
	std::list<Primitive_*>::const_iterator geom = geometry_list.begin();
	while (geom != geometry_list.end())
	{
		RELEASE(geom._Ptr->_Myval);

		geom++;
	}
	geometry_list.clear();

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

// Functionality ================================
Primitive_* GeometryManager::CreatePrimitive(PRIMITIVE_TYPE type)
{
	Primitive_* new_primitive = nullptr;
	
	switch (type)
	{
	case PRIMITIVE_POINT:
		break;
	case PRIMITIVE_LINE:
		break;
	case PRIMITIVE_PLANE:
		break;
	case PRIMITIVE_CUBE:
		new_primitive = new Cube_();
		break;
	case PRIMITIVE_SPHERE:
		new_primitive = new Sphere_();
		break;
	case PRIMITIVE_CYLINDER:
		new_primitive = new Cylinder_();
		break;
	case PRIMITIVE_RAY:
		break;
	case PRIMITIVE_CAPSULE:
		new_primitive = new Capsule_();
		break;
	case PRIMITIVE_FRUSTUM:
		new_primitive = new Frustrum_();
		break;
	case PRIMITIVE_MESH:
		new_primitive = new Mesh_();
		break;
	}

	//Add the new primitive at the geometry list
	geometry_list.push_back(new_primitive);

	return new_primitive;
}
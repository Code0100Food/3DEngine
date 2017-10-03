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
	
	//Load warrior fbx
	//LoadScene("Assets/arbol.fbx");

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

bool GeometryManager::LoadScene(const char * folder)
{
	const aiScene* scene = aiImportFile(folder, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		uint size = scene->mNumMeshes;
		for (uint k = 0; k < size; k++)
		{
			LOG("Created Mesh with:");
			//Pointer to the current mesh
			aiMesh* m = scene->mMeshes[k];
			
			//Create a new mesh
			Mesh_* mesh = (Mesh_*)CreatePrimitive(PRIMITIVE_TYPE::PRIMITIVE_MESH);
						
			//Load vertex data
			uint num_vertex = mesh->num_vertices = m->mNumVertices;
			mesh->vertices = new float[num_vertex * 3];
			memcpy(mesh->vertices, m->mVertices, sizeof(float) * num_vertex * 3);
	
			LOG("- %i vertices", num_vertex);

			//Load normals data
			/*if (m->HasNormals())
			{
				mesh->mesh.face_normals = new float[num_vertex * 3];
				memcpy(mesh->mesh.face_normals, m->mNormals, sizeof(float) * num_vertex * 3);
				LOG("- %i normals", num_vertex);
				//mesh->SetRenderFlags(DRAW_VERTEX | DRAW_FACE_NORMALS);
			}
			else LOG("- No normals");*/

			//Load index data
			if (m->HasFaces())
			{
				mesh->num_indices = m->mNumFaces * 3;
				mesh->indices = new uint[mesh->num_indices];
				for (uint h = 0; h < m->mNumFaces; h++)
				{
					if (m->mFaces[h].mNumIndices != 3)
					{
						LOG("[error] Geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&mesh->indices[h * 3], m->mFaces[h].mIndices, 3 * sizeof(uint));
					}
				}
				LOG("- %i indices", mesh->num_indices);
			}

			//Load colors data
			/*uint tot_colors = 0;
			mesh->mesh.colors = new float[num_vertex * 4];
			for (uint h = 0; h < num_vertex; h++)
			{
				if (m->HasVertexColors(h))
				{
					memcpy(&mesh->mesh.colors[h * 4], m->mColors, sizeof(float) * 4);
					tot_colors++;
				}
			}
			if (tot_colors > 0)
			{
				LOG("- %i Vertex Colors", tot_colors);
			}
			else LOG("- No Vertex Colors");*/
			
			//Load texture coords
			/*uint tot_tex_coords = 0;
			mesh->mesh.texture_coords = new float[num_vertex * 2];
			for (uint h = 0; h < num_vertex; h++)
			{
				if (m->HasTextureCoords(h))
				{
					memcpy(&mesh->mesh.texture_coords[h * 2], m->mTextureCoords, sizeof(float) * 2);
				}
			}
			if (tot_tex_coords > 0)
			{
				LOG("- %i Texture Coords", tot_tex_coords);
			}
			else LOG("- No Texture Coords");*/

			//Initialize the loaded mesh
			mesh->Initialize();

		}

		//Release the scene
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene");
		return false;
	}
	
	return true;
}

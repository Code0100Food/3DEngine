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

bool GeometryManager::Awake(const JSON_Object * data_root)
{
	show_grid = json_object_get_boolean(data_root, "show_grid");

	show_meshes = json_object_get_boolean(data_root, "show_meshes");

	mesh_lines_width = json_object_get_number(data_root, "mesh_lines_width");

	JSON_Array* mesh_color_array = json_object_get_array(data_root, "mesh_color");
	mesh_color[0] = json_array_get_number(mesh_color_array, 0);
	mesh_color[1] = json_array_get_number(mesh_color_array, 1);
	mesh_color[2] = json_array_get_number(mesh_color_array, 2);
	mesh_color[3] = json_array_get_number(mesh_color_array, 3);

	JSON_Array* vertex_normals_color_array = json_object_get_array(data_root, "vertex_normals_color");
	vertex_normals_color[0] = json_array_get_number(vertex_normals_color_array, 0);
	vertex_normals_color[1] = json_array_get_number(vertex_normals_color_array, 1);
	vertex_normals_color[2] = json_array_get_number(vertex_normals_color_array, 2);
	vertex_normals_color[3] = json_array_get_number(vertex_normals_color_array, 3);

	show_primitives = json_object_get_boolean(data_root, "show_primitives");

	JSON_Array* primitive_color_array = json_object_get_array(data_root, "primitive_color");
	primitive_color[0] = json_array_get_number(vertex_normals_color_array, 0);
	primitive_color[1] = json_array_get_number(vertex_normals_color_array, 1);
	primitive_color[2] = json_array_get_number(vertex_normals_color_array, 2);
	primitive_color[3] = json_array_get_number(vertex_normals_color_array, 3);

	return true;
}

// Game Loop ====================================
bool GeometryManager::Start()
{
	//Initialize grid
	grid.constant = 0;
	grid.normal = { 0, 1, 0 };
	grid.axis = true;

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
	
	return true;
}

bool GeometryManager::Draw()
{
	//Blit ground grid
	if (show_grid)grid.Render();

	//Enable the vertex and elements flags
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);

	//Blit debug primitives
	std::list<Primitive_*>::const_iterator geom = primitives_list.begin();
	while (geom != primitives_list.end())
	{
		geom._Ptr->_Myval->Draw();

		geom++;
	}

	//Blit meshes
	std::list<Model_*>::const_iterator mesh = models_list.begin();
	while (mesh != models_list.end())
	{
		mesh._Ptr->_Myval->Draw();

		mesh++;
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
	//Clean Primitives
	std::list<Primitive_*>::const_iterator geom = primitives_list.begin();
	while (geom != primitives_list.end())
	{
		RELEASE(geom._Ptr->_Myval);

		geom++;
	}
	primitives_list.clear();

	//Clean meshes
	std::list<Model_*>::const_iterator mesh = models_list.begin();
	while (mesh != models_list.end())
	{
		RELEASE(mesh._Ptr->_Myval);

		mesh++;
	}
	models_list.clear();


	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

void GeometryManager::BlitConfigInfo()
{
	ImGui::Checkbox("Show Grid", &show_grid);
}

void GeometryManager::SaveConfigInfo(JSON_Object * data_root)
{
	json_object_set_boolean(data_root, "show_grid", show_grid);

	json_object_set_boolean(data_root, "show_meshes", show_meshes);

	json_array_t*_array = json_object_get_array(data_root, "mesh_color");
	json_array_replace_number(_array, 0, mesh_color[0]);
	json_array_replace_number(_array, 1, mesh_color[1]);
	json_array_replace_number(_array, 2, mesh_color[2]);
	json_array_replace_number(_array, 3, mesh_color[3]);
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
	}

	//Add the new primitive at the geometry list
	primitives_list.push_back(new_primitive);

	return new_primitive;
}

Mesh_ * GeometryManager::CreateMesh()
{
	Mesh_* m = new Mesh_();
	//models_list.push_back(m);
	return m;
}

bool GeometryManager::LoadScene(const char * folder)
{

	Model_* new_model = new Model_(folder);
	models_list.push_back(new_model);

	return true;

	/*const aiScene* scene = aiImportFile(folder, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		uint size = scene->mNumMeshes;
		for (uint k = 0; k < size; k++)
		{
			LOG("Created Mesh with:");
			//Pointer to the current mesh
			aiMesh* m = scene->mMeshes[k];
			
			//Create a new mesh
			Mesh_* mesh = CreateMesh();
						
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
			else LOG("- No normals");

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
			else LOG("- No Texture Coords");

			//Initialize the loaded mesh
			mesh->Initialize();
			*/
		/*}

		//Release the scene
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene");
		return false;
	}
	*/
	return true;
}

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

	primitive_lines_width = json_object_get_number(data_root, "primitive_lines_width");

	JSON_Array* primitive_color_array = json_object_get_array(data_root, "primitive_color");
	primitive_color[0] = json_array_get_number(primitive_color_array, 0);
	primitive_color[1] = json_array_get_number(primitive_color_array, 1);
	primitive_color[2] = json_array_get_number(primitive_color_array, 2);
	primitive_color[3] = json_array_get_number(primitive_color_array, 3);

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
	
	Cylinder_* c = (Cylinder_*)CreatePrimitive(PRIMITIVE_TYPE::PRIMITIVE_CYLINDER);
	c->geometry->r = 3;
	c->geometry->l.a = { 0,4,0 };
	c->geometry->l.b = { 0,0,0 };
	c->SetDivisions(6);
	c->Initialize();

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
	if (show_primitives)
	{
		//Set Primitives render states
		glLineWidth(primitive_lines_width);
		glColor4f(primitive_color[0], primitive_color[1], primitive_color[2], primitive_color[3]);

		std::list<Primitive_*>::const_iterator geom = primitives_list.begin();
		while (geom != primitives_list.end())
		{
			geom._Ptr->_Myval->Draw();

			geom++;
		}
	}

	if (show_meshes)
	{
		//Set meshes render states
		glLineWidth(mesh_lines_width);
		glColor4f(mesh_color[0], mesh_color[1], mesh_color[2], mesh_color[3]);

		//Blit meshes
		std::list<Model_*>::const_iterator mesh = models_list.begin();
		while (mesh != models_list.end())
		{
			mesh._Ptr->_Myval->Draw();

			mesh++;
		}
	}

	glLineWidth(1.0f);
	glColor4f(1, 1, 1, 1);


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
	ImGui::Checkbox("Show Primitives", &show_primitives);
	ImGui::Checkbox("Show Meshes", &show_meshes);
	ImGui::SliderFloat("Mesh Lines Width", &mesh_lines_width, 0.0, 10.0, "%.1f");
	ImGui::SliderFloat4("Mesh Color", mesh_color, 0.0, 1.0, "%.2f");
	ImGui::SliderFloat("Primitive Lines Width", &primitive_lines_width, 0.0, 10.0, "%.1f");
	ImGui::SliderFloat4("Primitive Color", primitive_color, 0.0, 1.0, "%.2f");
	ImGui::SliderFloat4("Vertex Normals Color", vertex_normals_color, 0.0, 1.0, "%.2f");
}

void GeometryManager::SaveConfigInfo(JSON_Object * data_root)
{
	json_object_set_boolean(data_root, "show_grid", show_grid);
		
	json_object_set_boolean(data_root, "show_meshes", show_meshes);

	json_object_set_number(data_root, "mesh_lines_width", mesh_lines_width);

	json_array_t*_array = json_object_get_array(data_root, "mesh_color");
	json_array_replace_number(_array, 0, mesh_color[0]);
	json_array_replace_number(_array, 1, mesh_color[1]);
	json_array_replace_number(_array, 2, mesh_color[2]);
	json_array_replace_number(_array, 3, mesh_color[3]);

	_array = json_object_get_array(data_root, "vertex_normals_color");
	json_array_replace_number(_array, 0, vertex_normals_color[0]);
	json_array_replace_number(_array, 1, vertex_normals_color[1]);
	json_array_replace_number(_array, 2, vertex_normals_color[2]);
	json_array_replace_number(_array, 3, vertex_normals_color[3]);

	json_object_set_boolean(data_root, "show_primitives", show_primitives);

	json_object_set_number(data_root, "primitive_lines_width", primitive_lines_width);

	_array = json_object_get_array(data_root, "primitive_color");
	json_array_replace_number(_array, 0, primitive_color[0]);
	json_array_replace_number(_array, 1, primitive_color[1]);
	json_array_replace_number(_array, 2, primitive_color[2]);
	json_array_replace_number(_array, 3, primitive_color[3]);
}

// Functionality ================================
void GeometryManager::ShowSceneObjects()
{
	show_scene_objects = !show_scene_objects;
}

bool GeometryManager::GetObjWindowState() const
{
	return show_scene_objects;
}

void GeometryManager::BlitObjectsWindow()
{
	ImGui::Begin("Test", &show_scene_objects);
	
	std::list<Primitive_*>::const_iterator geom = primitives_list.begin();
	while (geom != primitives_list.end())
	{
		geom._Ptr->_Myval->Draw();

		geom++;
	}
	
	ImGui::End();
}

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

bool GeometryManager::LoadScene(const char * folder)
{
	//Temporal for the first assignment
	if (models_list.size() >= 1)
	{
		RELEASE(models_list.back());
		models_list.pop_back();
	}

	Model_* new_model = new Model_(folder);
	models_list.push_back(new_model);

	return true;
}

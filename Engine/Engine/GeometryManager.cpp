#include "GeometryManager.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"

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

	mesh_lines_width = json_object_get_number(data_root, "mesh_lines_width");

	JSON_Array* _array = json_object_get_array(data_root, "mesh_color");
	mesh_color[0] = json_array_get_number(_array, 0);
	mesh_color[1] = json_array_get_number(_array, 1);
	mesh_color[2] = json_array_get_number(_array, 2);
	mesh_color[3] = json_array_get_number(_array, 3);

	_array = json_object_get_array(data_root, "vertex_normals_color");
	vertex_normals_color[0] = json_array_get_number(_array, 0);
	vertex_normals_color[1] = json_array_get_number(_array, 1);
	vertex_normals_color[2] = json_array_get_number(_array, 2);
	vertex_normals_color[3] = json_array_get_number(_array, 3);

	_array = json_object_get_array(data_root, "face_normals_color");
	face_normals_color[0] = json_array_get_number(_array, 0);
	face_normals_color[1] = json_array_get_number(_array, 1);
	face_normals_color[2] = json_array_get_number(_array, 2);
	face_normals_color[3] = json_array_get_number(_array, 3);

	_array = json_object_get_array(data_root, "bounding_box_color");
	bounding_box_color[0] = json_array_get_number(_array, 0);
	bounding_box_color[1] = json_array_get_number(_array, 1);
	bounding_box_color[2] = json_array_get_number(_array, 2);
	bounding_box_color[3] = json_array_get_number(_array, 3);

	_array = json_object_get_array(data_root, "selected_bounding_box_color");
	selected_bounding_box_color[0] = json_array_get_number(_array, 0);
	selected_bounding_box_color[1] = json_array_get_number(_array, 1);
	selected_bounding_box_color[2] = json_array_get_number(_array, 2);
	selected_bounding_box_color[3] = json_array_get_number(_array, 3);

	primitive_lines_width = json_object_get_number(data_root, "primitive_lines_width");

	_array = json_object_get_array(data_root, "primitive_color");
	primitive_color[0] = json_array_get_number(_array, 0);
	primitive_color[1] = json_array_get_number(_array, 1);
	primitive_color[2] = json_array_get_number(_array, 2);
	primitive_color[3] = json_array_get_number(_array, 3);

	//Initialize grid
	grid = (Grid_*)new Grid_();
	grid->axis = json_object_get_boolean(data_root,"grid_axis");
	grid->divisions = json_object_get_number(data_root, "grid_divisions");
	
	_array = json_object_get_array(data_root, "grid_color");
	grid->color.r = json_array_get_number(_array, 0);
	grid->color.g = json_array_get_number(_array, 1);
	grid->color.b = json_array_get_number(_array, 2);
	grid->color.a = json_array_get_number(_array, 3);
	
	return true;
}

// Game Loop ====================================
bool GeometryManager::Start()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
	
	return true;
}

bool GeometryManager::Draw()
{
	//Enable the vertex and elements flags
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);

	if (show_grid)
	{
		App->renderer3D->DisableGLRenderFlags();
		grid->Draw();
		App->renderer3D->EnableGLRenderFlags();
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
	if (grid != nullptr)RELEASE(grid);

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

void GeometryManager::BlitConfigInfo()
{
	ImGui::Text("Primitives Configuration");
	ImGui::SliderFloat("Primitive Lines Width", &primitive_lines_width, 0.0, 10.0, "%.1f");
	ImGui::SliderFloat4("Primitive Color", primitive_color, 0.0, 1.0, "%.2f");
	
	ImGui::Separator();

	ImGui::Text("Meshes Configuration");
	ImGui::SliderFloat("Mesh Lines Width", &mesh_lines_width, 0.1, 10.0, "%.1f");
	ImGui::SliderFloat4("Mesh Color", mesh_color, 0.0, 1.0, "%.2f");
	ImGui::SliderFloat4("Vertex Normals Color", vertex_normals_color, 0.0, 1.0, "%.2f");
	ImGui::SliderFloat4("Face Normals Color", face_normals_color, 0.0, 1.0, "%.2f");
	ImGui::SliderFloat4("Bounding Box Color", bounding_box_color, 0.0, 1.0, "%.2f");
	ImGui::SliderFloat4("Selected Bounding Box Color", selected_bounding_box_color, 0.0, 1.0, "%.2f");

	ImGui::Separator();
	
	ImGui::Text("Grid Configuration");
	ImGui::Checkbox("Show Grid", &show_grid);
	ImGui::Checkbox("Grid Axis", &grid->axis);
	ImGui::SliderInt("Grid Divisions", (int*)(&grid->divisions), 1, 100);
	ImGui::SliderFloat4("Grid Color", &grid->color, 0.0f, 1.0f, "%.1f");
}

void GeometryManager::SaveConfigInfo(JSON_Object * data_root)
{
	json_object_set_boolean(data_root, "show_grid", show_grid);
		
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

	_array = json_object_get_array(data_root, "face_normals_color");
	json_array_replace_number(_array, 0, face_normals_color[0]);
	json_array_replace_number(_array, 1, face_normals_color[1]);
	json_array_replace_number(_array, 2, face_normals_color[2]);
	json_array_replace_number(_array, 3, face_normals_color[3]);

	_array = json_object_get_array(data_root, "bounding_box_color");
	json_array_replace_number(_array, 0, bounding_box_color[0]);
	json_array_replace_number(_array, 1, bounding_box_color[1]);
	json_array_replace_number(_array, 2, bounding_box_color[2]);
	json_array_replace_number(_array, 3, bounding_box_color[3]);

	_array = json_object_get_array(data_root, "selected_bounding_box_color");
	json_array_replace_number(_array, 0, selected_bounding_box_color[0]);
	json_array_replace_number(_array, 1, selected_bounding_box_color[1]);
	json_array_replace_number(_array, 2, selected_bounding_box_color[2]);
	json_array_replace_number(_array, 3, selected_bounding_box_color[3]);

	json_object_set_number(data_root, "primitive_lines_width", primitive_lines_width);

	_array = json_object_get_array(data_root, "primitive_color");
	json_array_replace_number(_array, 0, primitive_color[0]);
	json_array_replace_number(_array, 1, primitive_color[1]);
	json_array_replace_number(_array, 2, primitive_color[2]);
	json_array_replace_number(_array, 3, primitive_color[3]);

	json_object_set_number(data_root, "grid_divisions", grid->divisions);
	json_object_set_boolean(data_root, "grid_axis", grid->axis);
	_array = json_object_get_array(data_root, "grid_color");
	json_array_replace_number(_array, 0, grid->color.r);
	json_array_replace_number(_array, 1, grid->color.g);
	json_array_replace_number(_array, 2, grid->color.b);
	json_array_replace_number(_array, 3, grid->color.a);

}
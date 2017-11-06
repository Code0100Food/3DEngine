#include "Globals.h"

#include <math.h>

#include "ModuleWindow.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ModuleAudio.h"
#include "FileSystem.h"
#include "ModuleInput.h"
#include "GeometryManager.h"
#include "ModuleScene.h"

// Constructors =================================
ModuleCamera3D::ModuleCamera3D(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) : Module(_name, _id, _config_menu, _enabled)
{

}

// Destructors ==================================
ModuleCamera3D::~ModuleCamera3D()
{

}

// Game Loop ====================================
bool ModuleCamera3D::Awake(const JSON_Object * data_root)
{
	//Load camera location
	json_array_t* _array = json_object_get_array(data_root, "camera_location");
	editor_camera_frustrum.pos.x = json_array_get_number(_array, 0);
	editor_camera_frustrum.pos.y = json_array_get_number(_array, 1);
	editor_camera_frustrum.pos.z = json_array_get_number(_array, 2);
	//Load focus point
	_array = json_object_get_array(data_root, "focus_point");
	focus_point.x = json_array_get_number(_array, 0);
	focus_point.y = json_array_get_number(_array, 1);
	focus_point.z = json_array_get_number(_array, 2);
	//Load up vector
	_array = json_object_get_array(data_root, "up_vec");
	editor_camera_frustrum.up.x = json_array_get_number(_array, 0);
	editor_camera_frustrum.up.y = json_array_get_number(_array, 1);
	editor_camera_frustrum.up.z = json_array_get_number(_array, 2);
	//Load front vector
	_array = json_object_get_array(data_root, "front_vec");
	editor_camera_frustrum.front.x = json_array_get_number(_array, 0);
	editor_camera_frustrum.front.y = json_array_get_number(_array, 1);
	editor_camera_frustrum.front.z = json_array_get_number(_array, 2);
	//Load camera distance
	editor_camera_frustrum.farPlaneDistance = json_object_get_number(data_root, "far_plane_dist");
	//Load camera velocities
	camera_z_mov_vel = json_object_get_number(data_root, "camera_z_mov_vel");
	angle_to_rotate = json_object_get_number(data_root, "rot_around_angle");
	strafe_vel = json_object_get_number(data_root, "strafe_vel");
	wasd_vel = json_object_get_number(data_root, "wasd_vel");

	config_menu = true;

	return true;
}

bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	camera_z_mov_vel = 0.5f;

	editor_camera_frustrum.type = math::FrustumType::PerspectiveFrustum;

	editor_camera_frustrum.nearPlaneDistance = 1.0f;

	editor_camera_frustrum.verticalFov = 60 * DEGTORAD;
	editor_camera_frustrum.horizontalFov = (2 * math::Atan(math::Tan(editor_camera_frustrum.verticalFov / 2) * App->window->GetAspectRatio()));

	return ret;
}

update_status ModuleCamera3D::Update(float dt)
{
	camera_dist = (focus_point - editor_camera_frustrum.pos).Length();
	
	//Camera only accept input if the mouse is over the viewport
	if (App->renderer3D->GetMouseOnWorkspace())
	{
		//Camera zoom in/out
		if (App->input->GetMouseZ() == 1 && (focus_point - editor_camera_frustrum.pos).Length() > 0.1f)
		{
			editor_camera_frustrum.pos += editor_camera_frustrum.front * camera_z_mov_vel;
		}
		else if (App->input->GetMouseZ() == -1)
		{
			editor_camera_frustrum.pos -= editor_camera_frustrum.front * camera_z_mov_vel;
		}

		//Camera rotate around point
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
		{
			int dx = App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			if (dx != 0)
			{
				if (dx > 1)dx = 1;
				if(dx < -1)dx = -1;
								
				rotate_quaternion.SetFromAxisAngle({ 0,1,0 }, angle_to_rotate * dx * DEGTORAD);
				editor_camera_frustrum.Transform(rotate_quaternion);

				if ((focus_point - editor_camera_frustrum.pos).Length() > camera_dist || (focus_point - editor_camera_frustrum.pos).Length() < camera_dist)
				{
					editor_camera_frustrum.pos = focus_point - (editor_camera_frustrum.front * camera_dist);
				}
	
			}

			if (dy != 0)
			{
				if (dy > 1)dy = 1;
				if (dy < -1)dy = -1;

				rotate_quaternion.SetFromAxisAngle(math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up), angle_to_rotate * dy * DEGTORAD);
				editor_camera_frustrum.Transform(rotate_quaternion);
				if ((focus_point - editor_camera_frustrum.pos).Length() > camera_dist || (focus_point - editor_camera_frustrum.pos).Length() < camera_dist)
				{
					editor_camera_frustrum.pos = focus_point - (editor_camera_frustrum.front * camera_dist);
				}
			}
		}

		

		//Mouse picking
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT)
		{
			//Get the coords normalized and in screen percent
			mouse_x_normalized = (float)(App->input->GetMouseX() - App->renderer3D->GetSceneImagePos().x) / (App->renderer3D->GetSceneImageSize().x);
			mouse_y_normalized = (float)(App->input->GetMouseY() - App->renderer3D->GetSceneImagePos().y) / (App->renderer3D->GetSceneImageSize().y);

			//Tansform position from range [0,1] to range [-1,1]
			mouse_x_normalized = -((mouse_x_normalized * 2) - 1);
			mouse_y_normalized = -((mouse_y_normalized * 2) - 1);

			mouse_picking = editor_camera_frustrum.UnProjectFromNearPlane(mouse_x_normalized, mouse_y_normalized).ToLineSegment(editor_camera_frustrum.farPlaneDistance);

			//Check the line segment against all aabb's
			CheckAllAABB();
			
			//Check the line segment against triangles of the aabb's objects
			if (!objects_to_pick.empty())
			{
				App->scene->SetSelectedGameObject(CheckTriangles());		
			}
		}

		//Camera stafe 
		if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
		{
			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			if (dx != 0)
			{
				float DeltaX = (float)dx * strafe_vel;

				editor_camera_frustrum.pos -= math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up) * DeltaX;
				focus_point -= math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up) * DeltaX;
			}
			if (dy != 0)
			{
				float DeltaY = (float)dy * strafe_vel;

				editor_camera_frustrum.pos -= editor_camera_frustrum.up * DeltaY;
				focus_point -= editor_camera_frustrum.up * DeltaY;
			}

		}

		//Camera wasd movement
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				editor_camera_frustrum.pos -= editor_camera_frustrum.front * wasd_vel;
				focus_point -= editor_camera_frustrum.front * wasd_vel;

			}
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				editor_camera_frustrum.pos += editor_camera_frustrum.front * wasd_vel;
				focus_point += editor_camera_frustrum.front * wasd_vel;
			}
			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				editor_camera_frustrum.pos += math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up) * wasd_vel;
				focus_point += math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up) * wasd_vel;
			}
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				editor_camera_frustrum.pos -= math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up) * wasd_vel;
				focus_point -= math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up) * wasd_vel;
			}
			if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT)
			{
				editor_camera_frustrum.pos += editor_camera_frustrum.up * wasd_vel;
				focus_point += editor_camera_frustrum.up * wasd_vel;
			}
			if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
			{
				editor_camera_frustrum.pos -= editor_camera_frustrum.up * wasd_vel;
				focus_point -= editor_camera_frustrum.up * wasd_vel;
			}
		}

		//Focus current object
		else if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		{
			GameObject* target = App->scene->GetSelectedGameObject();

			if (target != nullptr)
			{
				LookAtGameObject(target);
			}
			else
			{
				LOG("[error] Invalid focus Target!");
				LookAt({ 0,0,0 });
			}
		}
	}

	return UPDATE_CONTINUE;
}

bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

void ModuleCamera3D::BlitConfigInfo()
{
	//Camera location ui
	ImGui::InputFloat3("Camera Location", editor_camera_frustrum.pos.ptr(), 2);
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change main camera location.");

	//View vector ui
	//ImGui::InputFloat3("View Vector", editor_camera_frustrum.front.ptr(), 2);
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change main camera view vector.");

	if (ImGui::DragFloat("Far Plane Distance", &editor_camera_frustrum.farPlaneDistance, 1.0, NULL, NULL, "%.1f"))
	{
		App->audio->PlayFxForInput(FX_ID::SLICE_TICK_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Changes The frustrum far plane distance");

	if (ImGui::DragFloat("Near Plane Distance", &editor_camera_frustrum.nearPlaneDistance, 0.01f, NULL, NULL, "%.1f"))
	{
		App->audio->PlayFxForInput(FX_ID::SLICE_TICK_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Changes The frustrum near plane distance");

	ImGui::Separator();

	//Camera Velocities ui
	ImGui::Text("Sensibilities");
	ImGui::SliderFloat("Zoom Sensibility", &camera_z_mov_vel, 0.1f, 10.0f, "%.1f");
	ImGui::SliderFloat("Rotate Around Sensibility", &angle_to_rotate, 0.1f, 360, "%.1f");
	ImGui::SliderFloat("Stafe Sensibility", &strafe_vel, 0.1f, 10.0f, "%.1f");
	ImGui::SliderFloat("WASD Sensibility", &wasd_vel, 0.1f, 10.0f, "%.1f");

	float new_fov = editor_camera_frustrum.verticalFov * RADTODEG;
	if (ImGui::DragFloat("Vertical FOV", &new_fov, 1.0, 1.0, 179))
	{
		SetVerticalFov(new_fov);
	}

	ImGui::Separator();
	ImGui::Text("Mouse Pick Normal Location:");
	ImGui::SameLine();
	ImGui::Text("%.4f, %.4f", mouse_x_normalized, mouse_y_normalized);
}

void ModuleCamera3D::SaveConfigInfo(JSON_Object * data_root)
{
	//Save camera location
	json_array_t* _array = json_object_get_array(data_root, "camera_location");
	json_array_replace_number(_array, 0, editor_camera_frustrum.pos.x);
	json_array_replace_number(_array, 1, editor_camera_frustrum.pos.y);
	json_array_replace_number(_array, 2, editor_camera_frustrum.pos.z);
	//Save focus point
	_array = json_object_get_array(data_root, "focus_point");
	json_array_replace_number(_array, 0, focus_point.x);
	json_array_replace_number(_array, 1, focus_point.y);
	json_array_replace_number(_array, 2, focus_point.z);
	//Save up vector
	_array = json_object_get_array(data_root, "up_vec");
	json_array_replace_number(_array, 0, editor_camera_frustrum.up.x);
	json_array_replace_number(_array, 1, editor_camera_frustrum.up.y);
	json_array_replace_number(_array, 2, editor_camera_frustrum.up.z);
	//Save front vector
	_array = json_object_get_array(data_root, "front_vec");
	json_array_replace_number(_array, 0, editor_camera_frustrum.front.x);
	json_array_replace_number(_array, 1, editor_camera_frustrum.front.y);
	json_array_replace_number(_array, 2, editor_camera_frustrum.front.z);
	//Save far plane distance
	json_object_set_number(data_root, "far_plane_dist", editor_camera_frustrum.farPlaneDistance);
	//Save camera velocities
	json_object_set_number(data_root, "camera_z_mov_vel", camera_z_mov_vel);
	json_object_set_number(data_root, "rot_around_angle", angle_to_rotate);
	json_object_set_number(data_root, "strafe_vel", strafe_vel);
	json_object_set_number(data_root, "wasd_vel", wasd_vel);
}

// Functionality ================================

void ModuleCamera3D::Look(const math::float3& look_here, bool RotateAroundLookingPoint)
{
	editor_camera_frustrum.front = (look_here - editor_camera_frustrum.pos).Normalized();
	math::float3 tmp = math::Cross({ 0,1,0 }, editor_camera_frustrum.front).Normalized();

	math::float3 res = math::Cross(editor_camera_frustrum.front, tmp).Normalized();
	if (abs(editor_camera_frustrum.up.x) - abs(res.x) + abs(editor_camera_frustrum.up.y) - abs(res.y) + abs(editor_camera_frustrum.up.z) - abs(res.z) < 0.005f)return;
	else editor_camera_frustrum.up = res;
}

void ModuleCamera3D::SetVerticalFov(float angle_in_deg)
{
	editor_camera_frustrum.verticalFov = angle_in_deg * DEGTORAD;
	editor_camera_frustrum.horizontalFov = (2 * math::Atan(math::Tan(editor_camera_frustrum.verticalFov / 2) * App->window->GetAspectRatio()));
}

void ModuleCamera3D::LookAt(const math::float3 &Spot)
{
	focus_point = Spot;
	editor_camera_frustrum.pos = Spot - (5 * editor_camera_frustrum.front);
}

void ModuleCamera3D::LookAtGameObject(GameObject * obj)
{
	ComponentTransform* trans_cmp = (ComponentTransform*)obj->FindComponent(COMP_TRANSFORMATION);
	if (trans_cmp == nullptr)
	{
		LOG("[error] Invalid focus Target!");
		return;
	}
	else if (obj->GetBoundingBoxDiagonalSize() <= 0)
	{
		focus_point = trans_cmp->GetPosition();
		Look(focus_point, true);
		return;
	}

	focus_point = trans_cmp->GetPosition();
	Look(focus_point, true);

	editor_camera_frustrum.pos = focus_point - (editor_camera_frustrum.front * obj->GetBoundingBoxDiagonalSize() * CAMERA_DIST_ON_FOCUS);
}

float* ModuleCamera3D::GetViewMatrix() const
{
	return editor_camera_frustrum.ViewMatrix().ptr();
}

float* ModuleCamera3D::GetViewMatrixTransposed() const
{
	return editor_camera_frustrum.WorldMatrix().ptr();
}

float* ModuleCamera3D::GetProjectionMatrix() const
{
	return editor_camera_frustrum.ViewProjMatrix().ptr();
}

float* ModuleCamera3D::GetProjectionMatrixTransposed() const
{
	return editor_camera_frustrum.ViewProjMatrix().Transposed().ptr();
}

float ModuleCamera3D::GetFrustrumNearPlaneDistance() const
{
	return editor_camera_frustrum.nearPlaneDistance;
}

float ModuleCamera3D::GetFrustrumFarPlaneDistance() const
{
	return editor_camera_frustrum.farPlaneDistance;
}

math::float3 ModuleCamera3D::GetPosition() const
{
	return editor_camera_frustrum.pos;
}

void ModuleCamera3D::CheckAllAABB() 
{
	GameObject* scene_root = App->scene->GetRoot();

	if (scene_root)
	{
		for (std::vector<GameObject*>::iterator item = scene_root->GetChilds()->begin(); item != scene_root->GetChilds()->end(); item++)
		{
			check_objects.push((*item));
		}
	}

	//Iterate all game objects and find if collides with the linesegment
	while (!check_objects.empty())
	{
		float collision_distance;
		float out_collision_distance;

		//If intersects we will add its children to the queue and add the distance to the map
		if (mouse_picking.Intersects(*check_objects.front()->GetBoundingBox(), collision_distance, out_collision_distance))
		{
			//Add the gameobject to the next checking
			std::pair<float, GameObject*> object_to_triangle_check;

			object_to_triangle_check.first = collision_distance;
			object_to_triangle_check.second = check_objects.front();

			objects_to_pick.insert(object_to_triangle_check);

			for (std::vector<GameObject*>::iterator item = check_objects.front()->GetChilds()->begin(); item != check_objects.front()->GetChilds()->end(); item++)
			{
				check_objects.push((*item));
			}
		}

		//Quit object
		check_objects.pop();
	}

}

GameObject* ModuleCamera3D::CheckTriangles()
{
	//Gameobject candidate to be the selected one
	GameObject* new_selected_gameobject = nullptr;

	float triangle_distance = 100.0f;
	float closest_triangle_distance = triangle_distance;

	for (std::map<float, GameObject*>::iterator item = objects_to_pick.begin(); item != objects_to_pick.end(); item++)
	{
		//Get the transform of the gameobject
		ComponentTransform* tmp_trans = (ComponentTransform*)(*item).second->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);

		//Transform the linesegment to local coordinates
		mouse_picking_local_space = mouse_picking;
		mouse_picking_local_space.Transform(tmp_trans->GetInheritedTransform().Inverted());

		//Get the triangles of the mesh
		ComponentMesh* tmp_mesh = (ComponentMesh*)(*item).second->FindMeshComponent();
		
		if (tmp_mesh != nullptr)
		{
			for (int i = 0; i < (tmp_mesh->GetIndexSize() - 2); i += 3)
			{
				triangle_to_test.a = tmp_mesh->GetVertexPosAt(tmp_mesh->GetIndexAt(i));
				triangle_to_test.b = tmp_mesh->GetVertexPosAt(tmp_mesh->GetIndexAt(i + 1));
				triangle_to_test.c = tmp_mesh->GetVertexPosAt(tmp_mesh->GetIndexAt(i + 2));

				//If we find the triangle we select the gameobject and we consider is the object to select
				if (mouse_picking_local_space.Intersects(triangle_to_test, &triangle_distance, nullptr))
				{
					if (triangle_distance < closest_triangle_distance)
					{
						closest_triangle_distance = triangle_distance;
						new_selected_gameobject = (*item).second;
					}
				}
			}
		}
		
	}
	objects_to_pick.clear();
	return new_selected_gameobject;
}

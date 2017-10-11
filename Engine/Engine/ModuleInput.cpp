#define _CRT_SECURE_NO_WARNINGS

#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleConsole.h"
#include "ModuleWindow.h"
#include "imgui/imgui_impl_sdl.h"
#include "InputManager.h"
#include "GeometryManager.h"
#include "ModuleTextures.h"
#include "ModuleImgui.h"
#include "ModuleCamera3D.h"

#define MAX_KEYS 300

// Constructors =================================
ModuleInput::ModuleInput(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) : Module(_name, _id, _config_menu, _enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * NUM_MOUSE_BUTTONS);
	memset(controller_axis, JOYSTICK_NOTHING, sizeof(JOYSTICK_STATE) * NUM_CONTROLLER_AXIS);
}

// Destructors ==================================
ModuleInput::~ModuleInput()
{
	RELEASE_ARRAY(keyboard);
}

// Game Loop ====================================
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) < 0)
	{
		LOG("[error] SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool ModuleInput::Start()
{
	//Identify & Open game controller
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			gamecontroller = SDL_GameControllerOpen(i);
			if (gamecontroller == nullptr)LOG("Gamepad not opened %s", SDL_GetError());
		}
	}

	return true;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	static SDL_Event event;

	// Keyboard keys ------------------
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
			{
				keyboard[i] = KEY_DOWN;
			}
			else
			{
				keyboard[i] = KEY_REPEAT;
			}
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
			{
				keyboard[i] = KEY_UP;
			}
			else
			{
				keyboard[i] = KEY_IDLE;
			}
		}
	}

	// Mouse keys -----------
	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;
	mouse_motion_x = mouse_motion_y = 0;

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (buttons & SDL_BUTTON(i))
		{
			if (mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}



	// Controller keys ------
	for (int i = 0; i < NUM_CONTROLLER_BUTTONS; ++i)
	{
		if (controller_buttons[i] == KEY_DOWN || controller_buttons[i] == KEY_REPEAT)
		{
		controller_buttons[i] = KEY_REPEAT;
		App->input_manager->SendControllerInputEvent(i, INPUT_STATE::INPUT_REPEAT);
		}

		if (controller_buttons[i] == KEY_UP)
		{
			controller_buttons[i] = KEY_IDLE;
		}
	}

	// Controller joysticks -
	for (int i = 0; i < NUM_CONTROLLER_AXIS; ++i)
	{
		if (controller_axis[i] == JOYSTICK_STATE::JOYSTICK_NEGATIVE)
		{
		App->input_manager->SendControllerJoystickEvent(i, INPUT_STATE::JSTICK_NEGATIVE);
		}

		if (controller_axis[i] == JOYSTICK_STATE::JOYSTICK_POSITIVE)
		{
		App->input_manager->SendControllerJoystickEvent(i, INPUT_STATE::JSTICK_POSITIVE);
		}
	}


	bool quit = false;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSdl_ProcessEvent(&event);

		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;

		case SDL_KEYDOWN:
			if (event.key.keysym.scancode == SDL_SCANCODE_GRAVE)
			{
				App->console->SwapConsoleState();
			}

		case SDL_MOUSEWHEEL:
			mouse_z = event.wheel.y;
			break;

		case SDL_MOUSEBUTTONDOWN:
			mouse_buttons[event.button.button - 1] = KEY_DOWN;
			//LOG("Mouse button %d down", event.button.button-1);
			break;

		case SDL_MOUSEBUTTONUP:
			mouse_buttons[event.button.button - 1] = KEY_UP;
			//LOG("Mouse button %d up", event.button.button-1);
			break;

		case SDL_MOUSEMOTION:
			mouse_x = event.motion.x / SCREEN_SIZE;
			mouse_y = event.motion.y / SCREEN_SIZE;

			mouse_motion_x = event.motion.xrel / SCREEN_SIZE;
			mouse_motion_y = event.motion.yrel / SCREEN_SIZE;
			break;

		case SDL_WINDOWEVENT:
		{
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				App->renderer3D->OnResize(event.window.data1, event.window.data2);
			}
		}
		break;

		case SDL_CONTROLLERAXISMOTION:
			if (event.caxis.value < -DEAD_ZONE)
			{
				controller_axis[event.caxis.axis] = JOYSTICK_STATE::JOYSTICK_NEGATIVE;
			}
			else
			{
				if (event.caxis.value > DEAD_ZONE)
				{
					if (event.caxis.axis != SDL_CONTROLLER_AXIS_TRIGGERLEFT && event.caxis.axis != SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
						controller_axis[event.caxis.axis] = JOYSTICK_STATE::JOYSTICK_POSITIVE;
					else
					{
						if (event.caxis.value > TRIGGER_ZONE)
							controller_axis[event.caxis.axis] = JOYSTICK_STATE::JOYSTICK_POSITIVE;
					}
				}

				else
				{
					controller_axis[event.caxis.axis] = JOYSTICK_STATE::JOYSTICK_NOTHING;

					if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
					{
						App->input_manager->SendControllerJoystickEvent(event.caxis.axis, INPUT_STATE::JSTICK_NONE);
					}

					if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
					{
						App->input_manager->SendControllerJoystickEvent(event.caxis.axis, INPUT_STATE::JSTICK_NONE);
					}
				}

			}
			break;

		case SDL_CONTROLLERBUTTONDOWN:
			//LOG("BOTON: %i", event.cbutton.button);
			controller_buttons[event.cbutton.button] = KEY_DOWN;
			App->input_manager->SendControllerInputEvent(event.cbutton.button, INPUT_STATE::INPUT_DOWN);
			break;

		case SDL_CONTROLLERBUTTONUP:
			controller_buttons[event.cbutton.button] = KEY_UP;
			App->input_manager->SendControllerInputEvent(event.cbutton.button, INPUT_STATE::INPUT_UP);
			break;


		case SDL_CONTROLLERDEVICEADDED:
			if (SDL_IsGameController(event.cdevice.which))
			{
				gamecontroller = SDL_GameControllerOpen(event.cdevice.which);
			}
			break;

		case SDL_CONTROLLERDEVICEREMOVED:
			if (gamecontroller)
			{
				SDL_GameControllerClose(gamecontroller);
			}
			break;

		case SDL_DROPFILE:
			string tmp = event.drop.file;
			int pos = tmp.length();

			if (tmp[pos - 1] == 'g' && tmp[pos - 2] == 'n' && tmp[pos - 3] == 'p')
			{
				App->textures->LoadCustomTexture(event.drop.file);
				App->textures->SetCustomMode();
				break;
			}

			App->geometry->LoadScene(event.drop.file);
			App->camera->LookAtModel(App->geometry->GetSelectedModel());
			break;
		}
	}

	if (quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
	{
		App->imgui->CallExitWindow();
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::BlitConfigInfo()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("Mouse Data");
	ImGui::BulletText("Mouse pos: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2, 0.7, 0.7, 1.0), "(%g, %g)", io.MousePos.x, io.MousePos.y);
	ImGui::BulletText("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (io.MouseDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2, 0.7, 0.7, 1.0), "b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
	ImGui::BulletText("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseClicked(i)) { ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2, 0.7, 0.7, 1.0), "b%d", i); }
	ImGui::BulletText("Mouse dbl-clicked:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDoubleClicked(i)) { ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2, 0.7, 0.7, 1.0), "b%d", i); }
	ImGui::BulletText("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseReleased(i)) { ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2, 0.7, 0.7, 1.0), "b%d", i); }
	ImGui::BulletText("Mouse wheel: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2, 0.7, 0.7, 1.0), " %.1f", io.MouseWheel);
	ImGui::Separator();
	ImGui::Text("Keys Data");
	ImGui::BulletText("Keys down:");      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (io.KeysDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2, 0.7, 0.7, 1.0), "%d (%.02f secs)", i, io.KeysDownDuration[i]); }
	ImGui::BulletText("Keys pressed:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyPressed(i)) { ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2, 0.7, 0.7, 1.0), "%d", i); }
	ImGui::BulletText("Keys release:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyReleased(i)) { ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2, 0.7, 0.7, 1.0), "%d", i); }
}

// Functionality ================================
KEY_STATE ModuleInput::GetKey(int id) const
{
	return keyboard[id];
}

KEY_STATE ModuleInput::GetMouseButton(int id) const
{
	return mouse_buttons[id];
}

int ModuleInput::GetMouseX() const
{
	return mouse_x;
}

int ModuleInput::GetMouseY() const
{
	return mouse_y;
}

int ModuleInput::GetMouseZ() const
{
	return mouse_z;
}

int ModuleInput::GetMouseXMotion() const
{
	return mouse_motion_x;
}

int ModuleInput::GetMouseYMotion() const
{
	return mouse_motion_y;
}

void ModuleInput::ResetInputMaps()
{
	// Keyboard keys ------------------
	for (uint i = 0; i < MAX_KEYS; ++i)
	{
		keyboard[i] = KEY_IDLE;
	}

	// Mouse keys -----------
	for (uint i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		mouse_buttons[i] = KEY_IDLE;
	}

	// Controller keys ------
	for (uint i = 0; i < NUM_CONTROLLER_BUTTONS; ++i)
	{
		controller_buttons[i] = KEY_IDLE;
	}
}

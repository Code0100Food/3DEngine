#include "InputManager.h"

#include "Application.h"
#include "ModuleInput.h"

/// Suitable_Input_Event --------------
// Constructors =============
Suitable_Input_Event::Suitable_Input_Event()
{

}

Suitable_Input_Event::Suitable_Input_Event(INPUT_EVENT input_event, APP_CONTEXT app_context) :input_event(input_event), app_context(app_context)
{

}

Suitable_Input_Event::Suitable_Input_Event(const Suitable_Input_Event & copy) : input_event(copy.input_event), app_context(copy.app_context)
{

}

// Destructors ==============
Suitable_Input_Event::~Suitable_Input_Event()
{

}

// Operators ================
bool Suitable_Input_Event::operator<(const Suitable_Input_Event & target) const
{
	return bool(this->input_event < target.input_event);
}
/// -----------------------------------

// Constructors =================================
InputManager::InputManager()
{
	id = M_INPUT_MANAGER;
	name = "InputManager";
	config_menu = true;
}

// Destructors ==================================
InputManager::~InputManager()
{

}


//Game Loop =====================================
// Called when before render is available
bool InputManager::Awake(const JSON_Object* data_root)
{
	//Get initial object
	const JSON_Object* key_ptr = json_object_get_object(data_root, "key0");

	//Iterate all the objects
	uint k = 1;
	while (key_ptr != NULL)
	{
		//Scancodes enums
		SDL_Scancode				scancode = SDL_SCANCODE_UNKNOWN;
		SDL_GameControllerButton	controller_scancode = SDL_CONTROLLER_BUTTON_INVALID;

		//Check if the programmed input is for the keyboard or the controller
		// and get the scancode

		//Load all the object data
		const char* str = json_object_get_string(key_ptr,"id");
		bool num = json_object_get_boolean(key_ptr, "num") > 0 ? true : false;
		uint controller_id = json_object_get_number(key_ptr, "controller_id");
		const char* controller_id_str = json_object_get_string(key_ptr, "controller_id");
		const char* event_str = json_object_get_string(key_ptr, "event");
		const char* app_context_str = json_object_get_string(key_ptr, "app_context");
		bool joy = json_object_get_boolean(key_ptr, "joy")> 0 ? true : false;

		if (str != nullptr)
		{
			scancode = SDL_GetScancodeFromName(str);
		}
		else
		{
			if (!num)controller_scancode = SDL_GameControllerGetButtonFromString(controller_id_str);
			else controller_scancode = (SDL_GameControllerButton)controller_id;
		}

		// Keyboard case
		if (scancode != SDL_SCANCODE_UNKNOWN)
		{
			//Map pair where new key is allocated
			std::pair<int, Suitable_Input_Event> new_key;

			//Build pair with the loaded data
			new_key.first = scancode;
			new_key.second = Suitable_Input_Event(StrToInputEvent(event_str), App->StrToAppContext(app_context_str));
			if (new_key.second.input_event == UNKNOWN_INPUT)
			{
				LOG("Error Loading Input Key!");
			}
			else
			{
				keyboard_events_map.insert(new_key);
			}
		}
		//Controller case
		if (controller_scancode != SDL_CONTROLLER_BUTTON_INVALID || joy)
		{
			//Check if is a button or joystick event
			if (joy)
			{
				//Map pair where new key is allocated
				std::pair<std::pair<int, int>, Suitable_Input_Event> new_key;

				//Build pair with the loaded data
				new_key.first = StrToControllerJoyID(controller_id_str);
				new_key.second = Suitable_Input_Event(StrToInputEvent(event_str), App->StrToAppContext(app_context_str));
				if (new_key.second.input_event == UNKNOWN_INPUT)
				{
					LOG("Error Loading Controller Input Key!");
				}
				else
				{
					controller_joysticks_events_map.insert(new_key);
				}

			}
			else
			{
				//Map pair where new key is allocated
				std::pair<int, Suitable_Input_Event> new_key;

				//Build pair with the loaded data
				new_key.first = controller_scancode;
				new_key.second = Suitable_Input_Event(StrToInputEvent(event_str), App->StrToAppContext(app_context_str));
				if (new_key.second.input_event == UNKNOWN_INPUT)
				{
					LOG("Error Loading Controller Input Key!");
				}
				else
				{
					controller_events_map.insert(new_key);
				}
			}

		}

		//Generate boject name
		char bff[8];
		itoa(k, bff, 10);
		std::string num_str = bff;
		std::string obj_name = "key" + num_str;
		//Find a object with the name generated
		key_ptr = json_object_get_object(data_root, obj_name.c_str());
		k += 1;
	}

	return true;
}


// Called after all Updates
update_status InputManager::PostUpdate(float dt)
{
	current_events.clear();
	return UPDATE_CONTINUE;
}

// Called before quitting
bool InputManager::CleanUp()
{
	current_events.clear();
	keyboard_events_map.clear();
	controller_events_map.clear();

	return true;
}

void InputManager::BlitConfigInfo()
{
	//Show all the programmed keyboard events
	ImGui::Text("Keyboard Events:");
	
	//Search the event in the keyboard map
	std::map<int, Suitable_Input_Event>::const_iterator key_event = keyboard_events_map.begin();
	while (key_event != keyboard_events_map.end())
	{
		ImGui::BulletText(SDL_GetScancodeName((SDL_Scancode)key_event._Ptr->_Myval.first));
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.2, 0.8, 0.2, 1.0), InputEventToStr(key_event._Ptr->_Myval.second.input_event));
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.6, 0.6, 0.8, 1.0), App->AppContextToStr(key_event._Ptr->_Myval.second.app_context));
		
		key_event++;
	}
	ImGui::Separator();

	//Show all the programmed controller events
	ImGui::Text("Controller Events");
	//Search the event in the keyboard map
	std::map<int, Suitable_Input_Event>::const_iterator controller_event = controller_events_map.begin();
	while (controller_event != controller_events_map.end())
	{
		ImGui::BulletText(SDL_GetScancodeName((SDL_Scancode)controller_event._Ptr->_Myval.first));
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.2, 0.8, 0.2, 1.0), InputEventToStr(controller_event._Ptr->_Myval.second.input_event));
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.6, 0.6, 0.8, 1.0), App->AppContextToStr(controller_event._Ptr->_Myval.second.app_context));

		controller_event++;
	}
}

//Functionality ==================================
INPUT_EVENT InputManager::StrToInputEvent(const char * str) const
{
	if (strcmp(str, "colliders_debug") == 0)		return INPUT_EVENT::COLLIDERS_DEBUG_MODE;
	if (strcmp(str, "ui_debug") == 0)				return INPUT_EVENT::UI_DEBUG_MODE;
	if (strcmp(str, "escape") == 0)					return INPUT_EVENT::ESCAPE;
	if (strcmp(str, "go_left") == 0)				return INPUT_EVENT::GO_LEFT;
	if (strcmp(str, "go_right") == 0)				return INPUT_EVENT::GO_RIGHT;
	if (strcmp(str, "jump") == 0)					return INPUT_EVENT::JUMP;
	if (strcmp(str, "crouch") == 0)					return INPUT_EVENT::CROUCH;
	if (strcmp(str, "interact") == 0)				return INPUT_EVENT::INTERACT;
	if (strcmp(str, "attack") == 0)					return INPUT_EVENT::ATTACK;
	if (strcmp(str, "focus_next") == 0)				return INPUT_EVENT::FOCUS_NEXT;
	if (strcmp(str, "focus_prev") == 0)				return INPUT_EVENT::FOCUS_PREV;
	if (strcmp(str, "accept") == 0)					return INPUT_EVENT::ACCEPT;
	if (strcmp(str, "add_value") == 0)				return INPUT_EVENT::ADD_VALUE;
	if (strcmp(str, "rest_value") == 0)				return INPUT_EVENT::REST_VALUE;
	return UNKNOWN_INPUT;
}

const char*	InputManager::InputEventToStr(INPUT_EVENT _event)const
{
	switch (_event)
	{
	case UNKNOWN_INPUT:		return "unknown";		break;
	case ADD_VALUE:			return "Add Value";		break;
	case REST_VALUE:		return "Rest Value";	break;
	}
}

std::pair<int, int> InputManager::StrToControllerJoyID(const char * str) const
{
	std::pair<int, int> ret = { SDL_CONTROLLER_AXIS_INVALID,JSTICK_NONE };
	if (strcmp(str, "c_axis_left_x_right") == 0)	ret = { SDL_CONTROLLER_AXIS_LEFTX ,JSTICK_POSITIVE };
	if (strcmp(str, "c_axis_left_x_left") == 0)		ret = { SDL_CONTROLLER_AXIS_LEFTX ,JSTICK_NEGATIVE };
	if (strcmp(str, "c_axis_left_y_up") == 0)		ret = { SDL_CONTROLLER_AXIS_LEFTY ,JSTICK_POSITIVE };
	if (strcmp(str, "c_axis_left_y_down") == 0)		ret = { SDL_CONTROLLER_AXIS_LEFTY ,JSTICK_NEGATIVE };
	/*if (strcmp(str, "c_axis_right_x") == 0)			return SDL_CONTROLLER_AXIS_RIGHTX;
	if (strcmp(str, "c_axis_right_y") == 0)			return SDL_CONTROLLER_AXIS_RIGHTY;
	if (strcmp(str, "c_axis_trigger_left") == 0)	return SDL_CONTROLLER_AXIS_TRIGGERLEFT;
	if (strcmp(str, "c_axis_trigger_left") == 0)	return SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
	if (strcmp(str, "c_axis_max") == 0)				return SDL_CONTROLLER_AXIS_MAX;*/
	return ret;
}

void InputManager::SendKeyboardInputEvent(int id, INPUT_STATE state)
{
	//Search the event in the map
	std::map<int, Suitable_Input_Event>::const_iterator key_event = keyboard_events_map.begin();


	while (key_event != keyboard_events_map.end())
	{

		//If the event is found we add it to the current events map
		if (key_event->first == id && key_event->second.app_context == App->app_context)
		{
			std::pair<Suitable_Input_Event, INPUT_STATE> key_pressed;
			key_pressed.first = key_event->second;
			key_pressed.second = state;
			current_events.insert(key_pressed);
			break;
		}
		key_event++;
	}
}

void InputManager::SendControllerInputEvent(int id, INPUT_STATE state)
{
	//Search the event in the map
	std::map<int, Suitable_Input_Event>::const_iterator key_event = controller_events_map.find(id);


	while (key_event != controller_events_map.end())
	{

		//If the event is found we add it to the current events map
		if (key_event->first == id && key_event->second.app_context == App->app_context)
		{
			std::pair<Suitable_Input_Event, INPUT_STATE> key_pressed;
			key_pressed.first = key_event->second;
			key_pressed.second = state;

			std::map<Suitable_Input_Event, INPUT_STATE>::const_iterator cur_event = current_events.begin();
			while (cur_event != current_events.end())
			{
				if (cur_event._Ptr->_Myval.first.input_event == key_pressed.first.input_event && cur_event._Ptr->_Myval.first.app_context == App->app_context)break;

				cur_event++;
			}

			if (cur_event != current_events.end())
			{
				if (state == INPUT_STATE::INPUT_DOWN && cur_event._Ptr->_Myval.second == INPUT_STATE::INPUT_NONE)cur_event._Ptr->_Myval.second = INPUT_STATE::INPUT_DOWN;
				else cur_event._Ptr->_Myval.second = state;
			}
			else
			{
				current_events.insert(key_pressed);
			}

			break;
		}
		key_event++;
	}
}

void InputManager::SendControllerJoystickEvent(int id, INPUT_STATE state)
{
	//Search the event in the map
	std::map<std::pair<int, int>, Suitable_Input_Event>::const_iterator key_event = controller_joysticks_events_map.begin();

	while (key_event != controller_joysticks_events_map.end())
	{

		//If the event is found we add it to the current events map
		if (key_event->first.first == id  && key_event->first.second == state && key_event->second.app_context == App->app_context)
		{
			std::pair<Suitable_Input_Event, INPUT_STATE> key_pressed;
			key_pressed.first = key_event->second;
			key_pressed.second = INPUT_REPEAT;

			//Check the event to avoid double input
			std::map<Suitable_Input_Event, INPUT_STATE>::const_iterator cur_event = current_events.begin();
			while (cur_event != current_events.end())
			{
				if (cur_event._Ptr->_Myval.first.input_event == key_pressed.first.input_event && cur_event._Ptr->_Myval.first.app_context == App->app_context)break;

				cur_event++;
			}
			//If the event is found just update the state
			if (cur_event == current_events.end())
			{
				current_events.insert(key_pressed);
			}
			else
			{
				if (cur_event._Ptr->_Myval.second == INPUT_STATE::INPUT_NONE)cur_event._Ptr->_Myval.second = INPUT_STATE::INPUT_REPEAT;
			}
			break;
		}
		key_event++;
	}
}

INPUT_STATE InputManager::GetEvent(INPUT_EVENT _event)
{
	if (!current_events.empty())
	{
		std::map<Suitable_Input_Event, INPUT_STATE>::const_iterator item = current_events.begin();
		while (item != current_events.end())
		{
			if (item._Ptr->_Myval.first.input_event == _event && item._Ptr->_Myval.first.app_context == App->app_context)break;

			item++;
		}

		if (item != current_events.end())
		{
			return item->second;
		}
	}

	return INPUT_NONE;
}
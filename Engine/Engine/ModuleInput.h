#ifndef _MODULE_INPUT_H_
#define _MODULE_INPUT_H_

#include "Module.h"
#include "Globals.h"
#include "imgui/imgui.h"
#define MAX_MOUSE_BUTTONS 5

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:

	ModuleInput(bool start_enabled = true);
	~ModuleInput();

public:

	bool			Init();
	update_status	PreUpdate(float dt);
	bool			CleanUp();

public:

	int last_key_pressed = -1;

	char GetLastKeyStr()const
	{
		if (last_key_pressed == -1)return '/0';
		/*char bf[2];
		itoa(last_key_pressed, bf, 10);*/

		return '0' + last_key_pressed;
	}

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

private:

	KEY_STATE*	keyboard = nullptr;
	KEY_STATE	mouse_buttons[MAX_MOUSE_BUTTONS];
	int			mouse_x = 0;
	int			mouse_y = 0;
	int			mouse_z = 0;
	int			mouse_x_motion = 0;
	int			mouse_y_motion = 0;
	int			mouse_z_motion = 0;

};

#endif // !_MODULE_INPUT_H_



#ifndef _MODULE_INPUT_H_
#define _MODULE_INPUT_H_

#include "Module.h"
#include "Globals.h"
#include "imgui/imgui.h"
#include "SDL\include\SDL_gamecontroller.h"

#define NUM_MOUSE_BUTTONS 5
#define NUM_CONTROLLER_BUTTONS 15
#define NUM_CONTROLLER_AXIS 6
#define DEAD_ZONE 10000
#define TRIGGER_ZONE 32000

enum EVENT_WINDOW
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

enum JOYSTICK_STATE
{
	JOYSTICK_POSITIVE,
	JOYSTICK_NEGATIVE,
	JOYSTICK_NOTHING
};

class ModuleInput : public Module
{
public:

	ModuleInput(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleInput();

public:

	bool			Init()final;
	bool			Start();
	update_status	PreUpdate(float dt)final;
	bool			CleanUp()final;
	void			BlitConfigInfo()final;

private:

	bool		windowEvents[WE_COUNT];
	KEY_STATE*	keyboard = nullptr;

	// Mouse data -----------
	KEY_STATE	mouse_buttons[NUM_MOUSE_BUTTONS];
	int			mouse_motion_x = 0;
	int			mouse_motion_y = 0;
	int			mouse_x = 0;
	int			mouse_y = 0;
	int			mouse_z = 0;

	// Controller data ------
	SDL_GameController*	gamecontroller = nullptr;
	KEY_STATE			controller_buttons[NUM_CONTROLLER_BUTTONS];
	JOYSTICK_STATE		controller_axis[NUM_CONTROLLER_AXIS];

public:

	//Get methods
	KEY_STATE	GetKey(int id) const;
	KEY_STATE	GetMouseButton(int id) const;
	int			GetMouseX() const;
	int			GetMouseY() const;
	int			GetMouseZ() const;
	int			GetMouseXMotion()const;
	int			GetMouseYMotion()const;

	//Scripting internal calls
	static bool	GetKeyDown(int id);

	// Reset all input states
	void		ResetInputMaps();

};

#endif // !_MODULE_INPUT_H_



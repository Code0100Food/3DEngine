#include "imgui/imgui.h"
#include <stdlib.h> 
#include <ctype.h>
#include <stdio.h>
#include <string>
#include "Globals.h"
#include <string>
#include <vector>

#ifndef _BASIC_CONSOLE_H_
#define _BASIC_CONSOLE_H_

// Demonstrating creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, here we are using a more C++ like approach of declaring a class to hold the data and the functions.

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

class BasicConsole
{
public:

	BasicConsole();
	~BasicConsole();

public:

	std::string				static_buffer;
	char					InputBuf[256];
	ImVector<char*>			Items;
	bool					ScrollToBottom;
	ImVector<char*>			History;
	int						HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
	ImVector<const char*>	Commands;
	uint					callback_flag = 0;

	//Commands data
	std::vector<std::string>	commands_str;
	std::vector<std::string>	commands_desc_str;

public:

	// Portable helpers
	static int   Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
	static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
	static char* Strdup(const char *str) { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }

	void					ClearLog();
	void					AddLog(const char* fmt, ...);
	void					_Draw(const char* title, bool* p_open);
	void					ExecCommand(const char* command_line);
	static int				_TextEditCallbackStub(ImGuiTextEditCallbackData* data); // In C++11 you are better off using lambdas for this sort of forwarding callbacks
	int						_TextEditCallback(ImGuiTextEditCallbackData* data);
		
};

static int	_TextEditCallbackStubss(ImGuiTextEditCallbackData* data); // In C++11 you are better off using lambdas for this sort of forwarding callbacks
static void ShowExampleAppConsole(bool* p_open);

#endif // !_BASIC_CONSOLE_H_
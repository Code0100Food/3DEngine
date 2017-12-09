#ifndef _SCRIPT_MANAGER_H_
#define _SCRIPT_MANAGER_H_

class  ScriptManager
{
public:

	ScriptManager();
	~ScriptManager();

private:

	/*Data*/

public:

	/*Methods*/
	const char* dll_path = nullptr;
	const char* Compile(const char* path, const char* output);

};
#endif // !_SCRIPT_MANAGER_H_

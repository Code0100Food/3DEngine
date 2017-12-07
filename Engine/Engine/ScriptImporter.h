#ifndef _SCRIPT_IMPORTER_H_
#define _SCRIPT_IMPORTER_H_

#include "Importer.h"
#include <string>

class ResourceScript;

class ScriptImporter : public Importer
{
public:

	ScriptImporter();

private:

	mutable std::string usable_str;

public:

	uint Import(const char* path);
	bool ReImport(ResourceScript* to_reload);

};
#endif // !_SCRIPT_IMPORTER_H_

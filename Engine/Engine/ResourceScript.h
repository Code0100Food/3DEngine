#ifndef _RESOURCE_SCRIPT_H_
#define _RESOURCE_SCRIPT_H_

#include "Resource.h"

class ResourceScript : public Resource
{
public:

	ResourceScript();
	~ResourceScript();

private:

	char* buffer = nullptr;

public:

	//Set Methods -----------
	void SetBuffer(const char* bf);

	//Get Methods -----------
	char* GetBuffer()const;

	//Functionality ---------
	//UI
	void BlitUI()const;
	void BlitComplexUI();
};
#endif // !_RESOURCE_SCRIPT_H_

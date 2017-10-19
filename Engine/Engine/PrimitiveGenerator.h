#ifndef _PRIMITIVE_GENERATOR_H_
#define _PRIMITIVE_GENERATOR_H_

#include "Globals.h"
#include "ComponentMesh.h"

class PrimitiveGenerator
{
public:

	PrimitiveGenerator();
	~PrimitiveGenerator();

protected:

	uint divisions = 0;

public:

	//Set Methods -----------
	void SetDivisions(uint div);

	//Functionality ---------
	virtual std::pair<std::vector<uint>, std::vector<Vertex>> Generate() = 0;

};
#endif // !_PRIMITIVE_GENERATOR_H_

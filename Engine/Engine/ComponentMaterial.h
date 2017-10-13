#ifndef _COMPONENT_MATERIAL_H_
#define _COMPONENT_MATERIAL_H_

#include "Component.h"
<<<<<<< HEAD
#include <vector>
#include "Mesh_.h"

class ComponentMaterial : public Component
{

	friend class ComponentMesh;

=======
#include <string>
#include <vector>

struct Texture
{
	uint		id = 0;
	std::string type;
	std::string path;

	int		tex_width = 0;
	int		tex_height = 0;
};

class ComponentMaterial : public Component
{
>>>>>>> origin/master
public:

	ComponentMaterial();
	ComponentMaterial(const ComponentMaterial& cpy);
	~ComponentMaterial();

private:

	std::vector<Texture> textures;

<<<<<<< HEAD
public:

	//Set Methods -----------
	void SetTextures(std::vector<Texture> textures);

	//Functionality ---------
	void BlitComponentInspector();

};

#endif
=======
};

#endif // !_COMPONENT_MATERIAL_H_

>>>>>>> origin/master

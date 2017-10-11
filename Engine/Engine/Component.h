#ifndef _COMPONENT_H_
#define _COMPONENT_H_

class Component
{
public:
	
	Component();
	Component(const Component& cpy);
	~Component();

public:

	virtual bool Update();

private:

	bool actived = true;

public:

	//Set Methods -----------
	void SetActiveState(bool act);

};
#endif // !_COMPONENT_H_

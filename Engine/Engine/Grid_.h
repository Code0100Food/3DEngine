#ifndef  _GRID_H_
#define  _GRID_H_

#include "Primitive_.h"

class Grid_ : public Primitive_
{
public:

	Grid_();
	Grid_(const Grid_& cpy);
	~Grid_();

public:

	void Draw();

};
#endif // ! _GRID_H_

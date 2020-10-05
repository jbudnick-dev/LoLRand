

#ifndef MYBUTTON_H_
#define MYBUTTON_H_

#include <SDL.h>

//myButton is a custom button with ability to set position, handle mouse or keyboard events, and return its 
//rectangle representation
class myButton
{
public:
	myButton(int xIn, int yIn, int wIn, int hIn);
	void setPosition(int xIn, int yIn);
	bool handleEvent(SDL_Event* e);
	SDL_Rect getRect();

private:
	int x,y,w,h;
};




#endif /* MYBUTTON_H_ */

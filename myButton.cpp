#include "myButton.h"


myButton::myButton(int xIn, int yIn, int wIn, int hIn)
{
	x = xIn;
	y = yIn;
	w = wIn;
	h = hIn;
}

void myButton::setPosition(int xIn, int yIn)
{
	x = xIn;
	y = yIn;
}

bool myButton::handleEvent(SDL_Event* e)
{

	bool inside = false;

	if(e->type == SDL_MOUSEBUTTONDOWN)
	{
		int mX, mY;
		SDL_GetMouseState(&mX, &mY);

		inside = true;

		if(mX < x)
			inside = false;
		if(mY < y)
			inside = false;
		if(mX > x + w)
			inside = false;
		if(mY > y + h)
			inside = false;

	}
	return inside;
}

SDL_Rect myButton::getRect()
{
	SDL_Rect buttonRect = {x,y,w,h};

	return buttonRect;
}

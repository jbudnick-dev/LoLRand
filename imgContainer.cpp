#include "imgContainer.h"

imgContainer::imgContainer(std::string pathIn, SDL_Texture* textIn, SDL_Rect rectIn)
{
	path = pathIn;
	imgTexture = textIn;
	imgRect = rectIn;
}

imgContainer::~imgContainer()
{
	SDL_DestroyTexture(imgTexture);
	imgTexture = NULL;
}

std::string& imgContainer::getPath()
{
	return path;
}

void imgContainer::setPath(std::string pathIn)
{
	path = pathIn;
}

SDL_Texture* imgContainer::getTexture()
{
	return imgTexture;
}

void imgContainer::setTexture(SDL_Texture* textIn)
{
	SDL_DestroyTexture(imgTexture);
	imgTexture = NULL;
	imgTexture = textIn;
}

SDL_Rect& imgContainer::getRect()
{
	return imgRect;
}

bool imgContainer::handleEvent(SDL_Event* e)
{

	bool inside = false;

	if(e->type == SDL_MOUSEBUTTONDOWN)
	{
		int mX, mY;
		SDL_GetMouseState(&mX, &mY);

		inside = true;

		if(mX < imgRect.x)
			inside = false;
		if(mY < imgRect.y)
			inside = false;
		if(mX > imgRect.x + imgRect.w)
			inside = false;
		if(mY > imgRect.y + imgRect.h)
			inside = false;

	}
	return inside;
}

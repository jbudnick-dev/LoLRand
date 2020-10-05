

#ifndef IMGCONTAINER_H_
#define IMGCONTAINER_H_

#include <SDL.h>
#include <string>

//imgContainer is a container class that holds a file path, texture, and rectangle representation.
//It provides functionality to get or set filepath, get or set texture, and handle mouse or keyboard events.
class imgContainer{

public:
	imgContainer(std::string pathIn,SDL_Texture* textIn, SDL_Rect rectIn);
	~imgContainer();
	std::string& getPath();
	void setPath(std::string);
	SDL_Texture* getTexture();
	void setTexture(SDL_Texture*);
	SDL_Rect& getRect();
	bool handleEvent(SDL_Event* e);

private:
	std::string path;
	SDL_Texture* imgTexture;
	SDL_Rect imgRect;
};



#endif /* IMGCONTAINER_H_ */

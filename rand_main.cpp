#define __USE_MINGW_ANSI_STDIO 0



#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "resources.h"
#include "myButton.h"
#include "imgContainer.h"


//Screen dimension constants
const int SCREEN_WIDTH =  955;
const int SCREEN_HEIGHT = 457;

//Champs in game
const int CHAMP_COUNT = 141;
//Items in game
const int ITEM_COUNT = 72;
//Boot items in game
const int BOOT_COUNT = 7;
//Summoner spells in game
const int SUM_COUNT = 9;
//# of buttons to draw
const int BUTT_NUM = 6;
//matrix representing the rune systsem.the first index represents the overarching tree(precision, domination, ect)
//second index represents row of specified tree, values represent number of rune choices on given row.
const int RUNE_CAP[5][4] = { {4,3,3,3}, {4,3,3,4}, {3,3,3,3}, {3,3,3,3}, {3,3,3,3}};
bool containsSmite = false;

//Champ quote sound
Mix_Chunk *champ = NULL;

//Starts up SDL and creates window
bool init();
//Frees media and shuts down SDL
void close();



//function definitions

//Renders given button with given texture
//Precondition: inputs are non-null
//PostCondiotion: button gets drawn via renderer
void renderButton(myButton&, SDL_Texture*);


//Insures no duplicats in a given container
//Precondition: None
//Postcondition: returns index of duplicate item, or -1 if no duplicates.
int noDuplicates(imgContainer conIn[], const int CONSIZE);


//Loads a texture
//Precondition: Inputs are non-null
//PostCondiotion: Returns loaded SDL_Texture*
SDL_Texture* loadTexture(std::string type, int typeNum, std::string& strIn);


//Specifically loads a buttons texture
//Precondition: 0 <= typeNum <= 3
//PostCondiotion: returns SDL_Texture*
SDL_Texture* loadButtonTexture(int typeNum);

//Specifically loads rune textures
//Precondition: inputs are non-null
//PostCondiotion: returns SDL_Texture*
SDL_Texture* loadRuneTexture(std::string& pathIn, bool primary);

//Sets the file paths of the secondary rune textures
//Precondition: None
//PostCondiotion:secRuneContainer paths are updated
void setSecPath();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;


//Texture Renderer
SDL_Renderer* gRenderer = NULL;

//Initializing imgContainer objects with default values

//Champion imgContainer
imgContainer champImg = {"root/champ/0.png",NULL, {32,32,120,120}};

//Array containing all item imgContainers
imgContainer itemImgContainer[] = { {"root/item/0.png", NULL,{ 175, 88, 64,64}}, {"root/item/0.png", NULL,{ itemImgContainer[0].getRect().x +94, 88, 64,64}},
		{"root/item/0.png", NULL,{ itemImgContainer[1].getRect().x +94, 88, 64,64}},{"root/item/0.png", NULL,{ itemImgContainer[2].getRect().x +94, 88, 64,64}},
		{"root/item/0.png", NULL, { itemImgContainer[3].getRect().x +94, 88, 64,64}}, {"root/item/0.png", NULL,{ itemImgContainer[4].getRect().x +94, 88, 64,64}}};

//Array containing both summoner spell imgContainers
imgContainer sumImgContainer[] = { {"root/sum/0.png", NULL, { itemImgContainer[5].getRect().x + 120, 88,64,64}},
		{"root/sum/1.png", NULL, {sumImgContainer[0].getRect().x + 94, 88,64, 64}}};

//Array containing primary rune tree imgContainers
imgContainer priRuneContainer[] = { {"root/rune/0/00.png", NULL, { 310, 250, 64, 64}}, {"root/rune/0/10.png", NULL, {priRuneContainer[0].getRect().x + 94, 250, 64, 64}},
		{"root/rune/0/20.png", NULL, {priRuneContainer[1].getRect().x + 94, 250,64,64}}, {"root/rune/0/30.png", NULL, {priRuneContainer[2].getRect().x + 94,250,64,64}}};

//Array containing secondary rune tree imgContainers
imgContainer secRuneContainer[]= { {"root/rune/1/10.png", NULL, {priRuneContainer[3].getRect().x + 110, 250,64,64}}, {"root/rune/1/33.png", NULL, {secRuneContainer[0].getRect().x + 94,250,64,64}}};

//Role imgContainer (top, jungle, ect)
imgContainer posContainer = {"root/pos/0.png", NULL, {150, 250, 64, 64}};


//initializing an array of button textures with the size being specified above
SDL_Texture* buttonTexture[BUTT_NUM];

//initializing buttons
myButton champButton = {champImg.getRect().x +10,175,100,25};
myButton itemButton = { itemImgContainer[5].getRect().x - 270, 175, 100,25};
myButton sumButton = { sumImgContainer[1].getRect().x - 66, 175,100,25};
myButton runeButton = { priRuneContainer[3].getRect().x - 33, 337, 100,25};
myButton posButton = { posContainer.getRect().x-18, 337, 100,25};
myButton bigReroll = { 450, 400, 115,25};

bool init()
{
	std::srand(time(NULL));
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "LoL Randomizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if(gRenderer == NULL)
				printf("Renderer could not be created! SDL ERROR: %s\n", SDL_GetError());

			else{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );

				Mix_Volume(-1, 10);
				champ = Mix_LoadWAV("root/sound/pop.ogg");
				Mix_PlayChannel(-1, champ,0);
				

				//RENDER TEXTURES FIRST TIME

				SDL_SetWindowIcon(gWindow, IMG_Load("root/icon.ico"));
				
				//Render champion texture
				champImg.setTexture(loadTexture("champ", CHAMP_COUNT, champImg.getPath()));


				//Render boot texture
				itemImgContainer[0].setTexture(loadTexture("boot", BOOT_COUNT, itemImgContainer[0].getPath()));
				
				//Render item textures
				for(int x = 1; x < 6; ++x)
					itemImgContainer[x].setTexture(loadTexture("ITEM", ITEM_COUNT, itemImgContainer[x].getPath()));
				
				//Render summoner spell textures
				for(int x = 0; x < 2; ++x)
					sumImgContainer[x].setTexture(loadTexture("sum", SUM_COUNT, sumImgContainer[x].getPath()));
				
				//Check for duplicates in items
				int dup = noDuplicates(itemImgContainer, 6);
				while(dup != -1)
				{
					itemImgContainer[dup].setTexture(loadTexture("item", ITEM_COUNT, itemImgContainer[dup].getPath()));
					dup = noDuplicates(itemImgContainer, 6);
				}

				//check for duplicates in summoner spells
				dup = noDuplicates(sumImgContainer, 2);
				while(dup != -1)
				{
					sumImgContainer[dup].setTexture(loadTexture("sum", SUM_COUNT, sumImgContainer[dup].getPath()));
					dup = noDuplicates(sumImgContainer, 2);
				}
				
				//Render rune textures
				for(int x = 0; x < 4; ++x)
					priRuneContainer[x].setTexture(loadRuneTexture(priRuneContainer[x].getPath(), true));
				setSecPath();
				for(int x = 0; x < 2; ++x)
					secRuneContainer[x].setTexture(loadRuneTexture(secRuneContainer[x].getPath(), false));
				
				//Render role texture
				posContainer.setTexture(loadTexture("pos", 5, posContainer.getPath()));

				for(int x = 0; x < 6; ++x)
				{
					SDL_DestroyTexture( buttonTexture[x] );
					buttonTexture[x] = NULL;
					buttonTexture[x] = loadButtonTexture(0);
				}
				buttonTexture[4] = loadButtonTexture(2);

			}
		}


	}

	return success;
}

SDL_Texture* loadTexture(std::string type, int typeNum, std::string& strIn)
{
	bool checkSmite = false;

	//check if the filepath represents a smite texture
	if(strIn.substr(5,5) == "smite")
		checkSmite = true;

	SDL_Texture* final = NULL;
	SDL_Surface* tempSurface = NULL;

	strIn = "root/" + type + "/" + std::to_string(std::rand() % typeNum) + ".png";

	//If loading a champion texture
	if(type == "champ")
	{
		int subL = 3;
		std::string temp = strIn.substr(11,subL);
		
		//Retrieve the champion quote filepath
		while(temp.find(".") != std::string::npos)
		{
			--subL;
			temp = strIn.substr(11,subL);
		}
		//Load champion quote
		champ = Mix_LoadWAV(("root/sound/" +temp + ".ogg").c_str());

		if(champ == NULL)
			std::cout <<"woops" << Mix_GetError();
	}
	
	//forces smite to be taken if you have a smite item
	if(!containsSmite && type == "item")
	{
		if(std::rand()% 100 <= 5)
		{
			strIn = "root/smite/" +std::to_string(std::rand() % 8) + ".png";
			if(( sumImgContainer[0].getPath() != "root/gSmite/0.png" && sumImgContainer[1].getPath() != "root/gSmite/0.png")
					&& sumImgContainer[0].getPath() != "root/sum/8.png" && sumImgContainer[1].getPath() != "root/sum/8.png")
			{
				int slot = std::rand() %2;
				sumImgContainer[slot].setTexture(loadTexture("gSmite", 1, sumImgContainer[slot].getPath()));
			}
			containsSmite = true;
		}
	}

	if(typeNum == 1)
		strIn = "root/" +type + "/0.png";

	if(checkSmite)
		if(strIn.substr(5,5) != "smite")
			containsSmite = false;

	tempSurface = IMG_Load(strIn.c_str());


	if(tempSurface == NULL)
		std::cout << "Unable to load img "  << strIn << std::endl;
	else
	{
		final = SDL_CreateTextureFromSurface(gRenderer, tempSurface);
		if(final == NULL)
			std::cout << "texture is null: " << SDL_GetError();
		SDL_FreeSurface(tempSurface);
	}
	return final;
}

SDL_Texture* loadButtonTexture(int typeNum)
{
	SDL_Texture* final = NULL;
	SDL_Surface* tempSurface = NULL;
	std::string temp = "root/button/" + std::to_string(typeNum) + ".png";
	tempSurface = IMG_Load(temp.c_str());

	if(tempSurface == NULL)
		std::cout << "Unable to load img but"  << std::endl;
	else
	{
		final = SDL_CreateTextureFromSurface(gRenderer, tempSurface);
		if(final == NULL)
			std::cout << "texture is null: " << SDL_GetError();
		SDL_FreeSurface(tempSurface);
	}
	return final;
}

SDL_Texture* loadRuneTexture(std::string& pathIn, bool primary)
{
	SDL_Texture* final = NULL;
	SDL_Surface* tempSurface = NULL;
	const int TREELVL = atoi(pathIn.substr(12,1).c_str());
	int treeType;

	if(TREELVL == 0)
		treeType = std::rand() %5;
	else
		if(primary)
			treeType = atoi(priRuneContainer[0].getPath().substr(10,1).c_str());


	if(primary)
		pathIn = "root/rune/" + std::to_string(treeType) + "/" + std::to_string(TREELVL) + std::to_string(std::rand() % RUNE_CAP[treeType][TREELVL]) + ".png";


	tempSurface = IMG_Load(pathIn.c_str());
	//std::cout << pathIn << "\n";
	if(tempSurface == NULL)
		std::cout << "Unable to load img "   << pathIn<< std::endl;
	else
	{
		final = SDL_CreateTextureFromSurface(gRenderer, tempSurface);
		if(final == NULL)
			std::cout << "texture is null: " << SDL_GetError();
		SDL_FreeSurface(tempSurface);
	}

	return final;
}

void setSecPath()
{
	int treeType = std::rand() %5;
	while(treeType ==  atoi(priRuneContainer[0].getPath().substr(10,1).c_str()))
		treeType = std::rand() %5;

	int temp1 = (std::rand() %3 ) +1, temp2 = (std::rand() %3 ) +1;
	while(temp1 == temp2)
		temp2 = (std::rand() %3 ) +1;
	secRuneContainer[0].setPath("root/rune/" + std::to_string(treeType) + "/" + std::to_string(temp1) + std::to_string(std::rand() % RUNE_CAP[treeType][temp1]) + ".png");
	secRuneContainer[1].setPath("root/rune/" + std::to_string(treeType) + "/" + std::to_string(temp2) + std::to_string(std::rand() % RUNE_CAP[treeType][temp2]) + ".png");
	//		std::cout << secRuneContainer[0].getPath() << "    " <<  secRuneContainer[1].getPath()<<"\n" ;
}




void close()
{
	//Free loaded image
	//SDL_DestroyTexture( gTexture );
	//gTexture= NULL;

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow( gWindow );
	gRenderer = NULL;
	gWindow = NULL;

	Mix_FreeChunk(champ);
	champ = NULL;

	//Quit SDL subsystems
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}



void renderButton(myButton& buttIn, SDL_Texture* textIn)
{
	SDL_Rect temp = buttIn.getRect();

	SDL_RenderCopy(gRenderer, textIn, NULL, &temp);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF );
	SDL_RenderDrawRect(gRenderer, &temp);

	SDL_Rect tempRect = {buttIn.getRect().x -1, buttIn.getRect().y -1, buttIn.getRect().w+2 ,
			buttIn.getRect().h+2};
	SDL_RenderDrawRect(gRenderer, &(tempRect));
}

int noDuplicates(imgContainer conIn[], const int CONSIZE)
{
	for(int x = (CONSIZE == 6); x < CONSIZE; ++x)
		for(int y = x+1; y < CONSIZE; ++y)
		{
			if(conIn[x].getPath() == conIn[y].getPath())
				return y;

			if(CONSIZE == 2)
			{
				if(conIn[x].getPath().substr(5,6) == "gSmite" || conIn[x].getPath().substr(9,1) == "8")
					if(conIn[y].getPath().substr(5,6) =="gSmite" || conIn[y].getPath().substr(9,1) == "8")
						return y;
			}
		}


	return -1;
}

int main( int argc, char* args[] )
{

	bool buttonClicked[] = {false, false, false, false, false, false};
	bool fullRoll = false;

	int clickedReset = 450;
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize LOL!\n" );
	}
	else
	{
		//		Mix_Volume(-1, 25);
		Mix_PlayChannel(-1, champ,0);

		{
			//Main loop flag
			bool quit = false;
			//Event handler
			SDL_Event e;

			//While application is running
			while( !quit )
			{
				//button clicking animation
				if(buttonClicked[0] || buttonClicked[1] || buttonClicked[2] || buttonClicked[3] || buttonClicked [4] || buttonClicked[5])
					--clickedReset;
				if(clickedReset == 0)
				{
					for(int x = 0; x < 6; ++x)
						if(buttonClicked[x])
						{
							if(x == 4)
							{
								buttonClicked[x] = false;
								SDL_DestroyTexture( buttonTexture[x] );
								buttonTexture[x] = NULL;
								buttonTexture[x] = loadButtonTexture(2);
								break;
							}
							buttonClicked[x] = false;
							SDL_DestroyTexture( buttonTexture[x] );
							buttonTexture[x] = NULL;
							buttonTexture[x] = loadButtonTexture(buttonClicked[x]);

						}
					clickedReset = 450;
				}


				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}else if(e.type == SDL_MOUSEBUTTONDOWN)
					{
						//Reroll all
						if(bigReroll.handleEvent(&e))
						{
							buttonClicked[4] = true;
							SDL_DestroyTexture( buttonTexture[4] );
							buttonTexture[4] = NULL;
							buttonTexture[4] = loadButtonTexture(3);

							fullRoll = true;
						}
						//Reroll champion
						if(champButton.handleEvent(&e) || fullRoll)
						{
							if(!fullRoll)
							{
								buttonClicked[0] = true;
								SDL_DestroyTexture( buttonTexture[0] );
								buttonTexture[0] = NULL;
								buttonTexture[0] = loadButtonTexture(buttonClicked[0]);
							}

							champImg.setTexture(loadTexture("champ", CHAMP_COUNT, champImg.getPath()));
							Mix_HaltChannel(-1);
							Mix_PlayChannel(-1, champ,0);
						}
						//replay champion quote on click
						if(champImg.handleEvent(&e))
						{
							Mix_HaltChannel(-1);
							Mix_PlayChannel(-1, champ,0);
						}
						
						//reroll items
						if(itemButton.handleEvent(&e) || fullRoll)
						{
							if(!fullRoll)
							{
								buttonClicked[1] = true;
								SDL_DestroyTexture( buttonTexture[1] );
								buttonTexture[1] = NULL;
								buttonTexture[1] = loadButtonTexture(buttonClicked[1]);
							}

							itemImgContainer[0].setTexture(loadTexture("boot", BOOT_COUNT, itemImgContainer[0].getPath()));
							for(int x = 1; x < 6; ++x)
								itemImgContainer[x].setTexture(loadTexture("item", ITEM_COUNT, itemImgContainer[x].getPath()));

							int dup = noDuplicates(itemImgContainer, 6);
							while(dup != -1)
							{
								itemImgContainer[dup].setTexture(loadTexture("item", ITEM_COUNT, itemImgContainer[dup].getPath()));
								dup = noDuplicates(itemImgContainer, 6);
							}

						}
						
						//reroll summoner spells
						if(sumButton.handleEvent(&e) || fullRoll)
						{
							if(!fullRoll)
							{
								buttonClicked[2] = true;
								SDL_DestroyTexture( buttonTexture[2] );
								buttonTexture[2] = NULL;
								buttonTexture[2] = loadButtonTexture(buttonClicked[2]);
							}

							for(int x = 0; x < 2; ++x)
								sumImgContainer[x].setTexture(loadTexture("sum", SUM_COUNT, sumImgContainer[x].getPath()));


							if(containsSmite)
							{
								int slot = std::rand() %2;
								sumImgContainer[slot].setTexture(loadTexture("gSmite", 1, sumImgContainer[slot].getPath()));
							}


							int dup = noDuplicates(sumImgContainer, 2);

							while(dup != -1)
							{
								sumImgContainer[dup].setTexture(loadTexture("sum", SUM_COUNT, sumImgContainer[dup].getPath()));
								dup = noDuplicates(sumImgContainer, 2);
							}
						}
						
						//reroll runes
						if(runeButton.handleEvent(&e) || fullRoll)
						{
							if(!fullRoll)
							{
								buttonClicked[3] = true;
								SDL_DestroyTexture( buttonTexture[3] );
								buttonTexture[3] = NULL;
								buttonTexture[3] = loadButtonTexture(buttonClicked[3]);
							}

							for(int x = 0; x < 4; ++x)
								priRuneContainer[x].setTexture(loadRuneTexture(priRuneContainer[x].getPath(), true));

							setSecPath();
							for(int x = 0; x < 2; ++x)
								secRuneContainer[x].setTexture(loadRuneTexture(secRuneContainer[x].getPath(), false));
						}
						
						//reroll roles
						if(posButton.handleEvent(&e) || fullRoll)
						{
							if(!fullRoll)
							{
								buttonClicked[5] = true;
								SDL_DestroyTexture( buttonTexture[5] );
								buttonTexture[5] = NULL;
								buttonTexture[5] = loadButtonTexture(buttonClicked[5]);
							}

							posContainer.setTexture(loadTexture("pos", 5, posContainer.getPath()));
						}

					}
				}


				//Render textures to screen

				//render champion
				SDL_RenderCopy( gRenderer, champImg.getTexture(), NULL, &(champImg.getRect()) );
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF );
				
				//render items
				for(int x = 0; x < 6; ++x)
				{
					SDL_RenderCopy(gRenderer, itemImgContainer[x].getTexture(), NULL, &(itemImgContainer[x].getRect()));
					SDL_Rect tempRect = {itemImgContainer[x].getRect().x -1, itemImgContainer[x].getRect().y -1, itemImgContainer[x].getRect().h +2,
							itemImgContainer[x].getRect().w+2};

					SDL_RenderDrawRect(gRenderer, &(itemImgContainer[x].getRect()));
					SDL_RenderDrawRect(gRenderer, &(tempRect));
				}
				
				//render summoner spells
				for(int x = 0; x < 2; ++x)
				{

					SDL_RenderCopy(gRenderer, sumImgContainer[x].getTexture(), NULL, &(sumImgContainer[x].getRect()));
					SDL_Rect tempRect = {sumImgContainer[x].getRect().x -1, sumImgContainer[x].getRect().y -1, sumImgContainer[x].getRect().h +2,
							sumImgContainer[x].getRect().w+2};

					SDL_RenderDrawRect(gRenderer, &(sumImgContainer[x].getRect()));
					SDL_RenderDrawRect(gRenderer, &(tempRect));
				}

				//render runes
				for(int x =0; x < 4; ++x)
				{
					SDL_RenderCopy(gRenderer, priRuneContainer[x].getTexture(), NULL, &(priRuneContainer[x].getRect()));
					SDL_Rect tempRect = {priRuneContainer[x].getRect().x -1, priRuneContainer[x].getRect().y -1, priRuneContainer[x].getRect().h +2,
							priRuneContainer[x].getRect().w+2};

					SDL_RenderDrawRect(gRenderer, &(priRuneContainer[x].getRect()));
					SDL_RenderDrawRect(gRenderer, &(tempRect));
				}
				
				for(int x =0; x < 2; ++x)
				{
					SDL_RenderCopy(gRenderer, secRuneContainer[x].getTexture(), NULL, &(secRuneContainer[x].getRect()));
					SDL_Rect tempRect = {secRuneContainer[x].getRect().x -1, secRuneContainer[x].getRect().y -1, secRuneContainer[x].getRect().h +2,
							secRuneContainer[x].getRect().w+2};
					SDL_RenderDrawRect(gRenderer, &(secRuneContainer[x].getRect()));
					SDL_RenderDrawRect(gRenderer, &(tempRect));
				}
				
				//render role
				SDL_RenderCopy(gRenderer, posContainer.getTexture(), NULL, &(posContainer.getRect()));
				
				//render boxes around role
				SDL_Rect tempRect = {posContainer.getRect().x -1, posContainer.getRect().y -1, posContainer.getRect().h +2,
						posContainer.getRect().w+2};
				SDL_RenderDrawRect(gRenderer, &(posContainer.getRect()));
				SDL_RenderDrawRect(gRenderer, &(tempRect));

			


				//Draw boxes surrounding runes
				for(int x = 0; x < 3; ++x)
				{
					SDL_RenderDrawLine(gRenderer, priRuneContainer[x].getRect().x + priRuneContainer[x].getRect().w,
							priRuneContainer[x].getRect().y + (priRuneContainer[x].getRect().h/2),
							priRuneContainer[x+1].getRect().x,
							priRuneContainer[x+1].getRect().y + (priRuneContainer[x+1].getRect().h/2));
					SDL_RenderDrawLine(gRenderer, priRuneContainer[x].getRect().x + priRuneContainer[x].getRect().w,
							priRuneContainer[x].getRect().y-1 + (priRuneContainer[x].getRect().h/2),
							priRuneContainer[x+1].getRect().x,
							priRuneContainer[x+1].getRect().y-1 + (priRuneContainer[x+1].getRect().h/2));
				}



				//Draw boxes surrounding runes
				SDL_RenderDrawLine(gRenderer, secRuneContainer[0].getRect().x + secRuneContainer[0].getRect().w,
						secRuneContainer[0].getRect().y + (secRuneContainer[0].getRect().h/2),
						secRuneContainer[1].getRect().x,
						secRuneContainer[1].getRect().y + (secRuneContainer[1].getRect().h/2));
				SDL_RenderDrawLine(gRenderer, secRuneContainer[0].getRect().x + secRuneContainer[0].getRect().w,
						secRuneContainer[0].getRect().y-1 + (secRuneContainer[0].getRect().h/2),
						secRuneContainer[1].getRect().x,
						secRuneContainer[1].getRect().y-1 + (secRuneContainer[1].getRect().h/2));


				
				renderButton(champButton, buttonTexture[0]);
				renderButton(itemButton, buttonTexture[1]);
				renderButton(sumButton,buttonTexture[2]);
				renderButton(runeButton, buttonTexture[3]);
				renderButton(bigReroll, buttonTexture[4]);
				renderButton(posButton, buttonTexture[5]);


				SDL_SetWindowIcon(gWindow, IMG_Load("root/icon.ico"));
				SDL_SetRenderDrawColor(gRenderer, 240, 240, 240, 0xFF );
				SDL_RenderPresent(gRenderer);
				SDL_RenderClear(gRenderer);
				fullRoll = false;
			}
		}

		//Free resources and close SDL
		close();


	}
	return 0;
}

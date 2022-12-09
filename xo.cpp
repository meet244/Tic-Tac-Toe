#include <SDL.h>
#undef main
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
using namespace std;

int b[3][3] = { {'3','4','5'},{'6','7','8'},{'9','10','11'} };
bool draw = false;
bool comp = false;
bool replay = false;
bool quit = false;
bool first = true;

Mix_Music* play(string s,int pause) {


	if (s == "bot") {
		s = "bot.mp3";
	}
	else if (s == "draw") {
		s = "draw.mp3";
	}
	else if (s == "win") {
		s = "win.mp3";
	}
	else if (s == "beep") {
		s = "beep.mp3";
	}
	else if (s == "click") {
		s = "click.mp3";
	}
	else if (s == "start") {
		s = "start.mp3";
	}

	int result = 0;
	int flags = MIX_INIT_MP3;

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		printf("Failed to init SDL\n");
		exit(1);
	}

	if (flags != (result = Mix_Init(flags))) {
		printf("Could not initialize mixer (result: %d).\n", result);
		printf("Mix_Init: %s\n", Mix_GetError());
		exit(1);
	}

	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
	Mix_Music* music = Mix_LoadMUS(s.c_str());
	Mix_PlayMusic(music, 1);

	//while (!SDL_QuitRequested()) {
	SDL_Delay(pause);
	//}

	Mix_FreeMusic(music);
	//SDL_Quit();
	return music;
}

bool gameOver() {
	for (int i = 0; i < 3; i++) {
		if (b[i][0] == b[i][1] && b[i][0] == b[i][2] || b[0][i] == b[1][i] && b[0][i] == b[2][i]) {
			draw = false;
			return true;
		}
	}

	if (b[0][0] == b[1][1] && b[0][0] == b[2][2] || b[0][2] == b[1][1] && b[0][2] == b[2][0]) {
		draw = false;
		return true;
	}

	int full = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (b[i][j] == 1 || b[i][j] == 0) { // X==1  O==2
				full++;
				if (full == 9) {
					draw = true;
					return true;
				}
			}
		}
	}

	draw = true;
	return false;
}

void end(SDL_Renderer* rend, string s) {
	SDL_Surface* surface;

	surface = IMG_Load(s.c_str());

	SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);

	SDL_FreeSurface(surface);

	SDL_Rect dest;

	SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);

	dest.x = 0;
	dest.y = 0;

	SDL_RenderCopy(rend, tex, NULL, &dest);
	SDL_RenderPresent(rend);

	if (s == "endd.png") {
		play("draw",1000);
	}
	else {
		play("win",1200);
	}

	int close = 0;

	while (!close) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {

			case SDL_QUIT:
				quit = true;
				close = 1;
				break;

			case SDL_MOUSEBUTTONDOWN:
				int x, y;
				x = event.button.x;
				y = event.button.y;

				if (y > 450) {
					play("click", 300);
					if (x < 300) {
						replay = true;
						close = 1;
					}
					else {
						replay = false;
						close = 1;
					}
				}
				break;
			}
		}
	}

	SDL_RenderClear(rend);
	return;
}

SDL_Texture* add(SDL_Renderer* rend,int x,int y, string s) {
	
	// creates a surface to load an image into the main memory
	SDL_Surface* surface;

	// please provide a path for your image
	//surface = IMG_Load("OO.png");
	surface = IMG_Load(s.c_str());

	// loads image to our graphics hardware memory.
	SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);

	// clears main-memory
	SDL_FreeSurface(surface);

	// let us control our image position
	// so that we can move it with our keyboard.
	SDL_Rect dest;

	// connects our texture with dest to control position
	SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);

	// adjust height and width of our image box.
	dest.w /= 3;
	dest.h /= 3;

	// sets initial x-position of object
	dest.x = x+20;

	// sets initial y-position of object
	dest.y = y+15;

	//SDL_RenderClear(rend);
	SDL_RenderCopy(rend, tex, NULL, &dest);
	//SDL_RenderPresent(rend);
	return tex;
}

SDL_Texture* cpu(SDL_Renderer* rend) {
	int freex[8] = { 9,9,9,9,9,9,9,9};
	int freey[8] = { 9,9,9,9,9,9,9,9};
	int cnt = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (b[i][j] != 1 && b[i][j] != 0) {
				//add(rend, i * 200, j * 200, "OO.png");
				freex[cnt] = i;
				freey[cnt] = j;
				cnt++;
			}
		}
	}
	srand(time(NULL)); // Seed the time
	int randNum = rand() % ((cnt-1)-0 + 1) + 0;
	
	int x = freex[randNum];
	int y = freey[randNum];
	b[x][y] = 0;

	return add(rend, x * 200, y * 200, "OO.png");
}

void start(SDL_Renderer* rend) {
	again:

	b[0][0] = '3';
	b[0][1] = '4';
	b[0][2] = '5';
	b[1][0] = '6';
	b[1][1] = '7';
	b[1][2] = '8';
	b[2][0] = '9';
	b[2][1] = '10';
	b[2][2] = '11';

	SDL_Texture* texs[9];

	SDL_Surface* surface;

	surface = IMG_Load("board.png");

	SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);

	SDL_FreeSurface(surface);

	SDL_Rect dest;

	SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);

	dest.x = 0;
	dest.y = 0;

	SDL_RenderCopy(rend, tex, NULL, &dest);
	SDL_RenderPresent(rend);

	int close = 0;
	int turn = 1;
	int count = 0;

	while (!close) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {

			case SDL_QUIT:
				quit = true;
				close = 1;
				break;

			case SDL_MOUSEBUTTONDOWN:
				int x, y;
				x = event.button.x / 200;
				y = event.button.y / 200;

				if (b[x][y] == 0 || b[x][y] == 1) {
					continue;
				}
				if (turn == 1) {
					texs[count] = add(rend, x * 200, y * 200, "XX.png");
					b[x][y] = 1;
					turn = 2;
				}
				else {
					texs[count] = add(rend, x * 200, y * 200, "OO.png");
					b[x][y] = 0;
					turn = 1;
				}
				play("beep",300);
				count++;
				if (gameOver()) {
					close = 1;
				}
				else {
					if (comp) {
						SDL_RenderPresent(rend);
						texs[count] = cpu(rend);
						SDL_Delay(300);
						turn = 1;
						count++;
						if (gameOver()) {
							close = 1;
						}
					}
				}
				break;
			}
		}
		SDL_RenderPresent(rend);
	}

	// destroy texture
	for (int i = 0; i != count; i++) {
		SDL_DestroyTexture(texs[i]);
	}
	SDL_DestroyTexture(tex);


	SDL_RenderClear(rend);
	if (!quit) {
		if (draw) {
			end(rend, "endd.png");
		}
		else if (turn == 2) {
			end(rend, "endx.png");
		}
		else if (turn == 1) {
			end(rend, "endo.png");
		}
		if (replay) {
			replay = false;
			goto again;
		}
	}
	return;
}

void begain(SDL_Renderer* rend) {
	OnceMore:

	SDL_Surface* surface;

	surface = IMG_Load("start.png");

	SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);

	SDL_FreeSurface(surface);

	SDL_Rect dest;

	SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);

	dest.x = 0;
	dest.y = 0;

	SDL_RenderCopy(rend, tex, NULL, &dest);
	SDL_RenderPresent(rend);

	if (first) {
		first = false;
		play("start", 2400);
	}

	int close = 0;

	while (!close) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {

			case SDL_QUIT:
				// handling of close button
				close = 1;
				break;

			case SDL_MOUSEBUTTONDOWN:
				int x, y;
				x = event.button.x;
				y = event.button.y;

				if (y > 300) {
					play("click",300);
					if (x<300) {
						comp = false;
						SDL_RenderClear(rend);
						SDL_RenderPresent(rend);
						start(rend);
						
					}
					else {
						comp = true;
						SDL_RenderClear(rend);
						SDL_RenderPresent(rend);
						start(rend);
					}
					if (quit) {
						close = 1;
						break;
					}
					goto OnceMore;
					/*
					SDL_RenderClear(rend);
					SDL_Surface* surface;

					surface = IMG_Load("start.png");

					SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);

					// clears main-memory
					SDL_FreeSurface(surface);

					SDL_Rect dest;

					// connects our texture with dest to control position
					SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);

					dest.x = 0;
					dest.y = 0;

					SDL_RenderCopy(rend, tex, NULL, &dest);
					SDL_RenderPresent(rend);*/
				}

				break;
			}
		}
	}

	return;
}

int main(int argc, char* argv[])
{
	// returns zero on success else non-zero
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	SDL_Window* win = SDL_CreateWindow("TIC TAC TOE GAME  --By MEET", // creates a window
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		600, 600, 0);

	Uint32 render_flags = SDL_RENDERER_ACCELERATED;

	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

	begain(rend);

	SDL_Quit();

	return 0;
}

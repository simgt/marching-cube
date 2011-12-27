#include <SDL.h>
#include <vector>

int main (int, char**) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE);
    
	SDL_Event event = {0};
 
    while (event.type != SDL_QUIT) {
        SDL_WaitEvent(&event);
        
    }
 
    SDL_Quit();
 
    return 0;
}
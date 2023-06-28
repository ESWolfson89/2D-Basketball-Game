#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "globals.h"
#include "point.h"

static const point texture_size[NUM_TOTAL_TEXTURES] =
{
    point((double)WINDOW_WIDTH,(double)WINDOW_HEIGHT),
    point(19.0,19.0),
    point(63.0,96.0),
    point(33.0,48.0),
    point(33.0,48.0)
};

class gfx_engine
{
    public:
        gfx_engine();
        ~gfx_engine();
        bool initSDL();
        void freeSDL();
        void addGFXObject(obj_type, point, point, int, double, SDL_RendererFlip);
        void addBitmapCharacter(SDL_Color,int,point);
        void addBitmapString(SDL_Color,std::string,point);
        void clearScreen();
        void updateScreen();
        void drawRectangle(SDL_Color,point,point);
    private:
        SDL_Window *screen;
        SDL_Renderer *renderer;
        SDL_Texture *background_texture;
        SDL_Texture *player1_texture;
        SDL_Texture *player2_texture;
        SDL_Texture *basketball_texture;
        SDL_Texture *net_texture;
        SDL_Texture *text_bitmap;
};

#endif

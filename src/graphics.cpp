#include "graphics.h"

gfx_engine::gfx_engine()
{
    screen = NULL;
    renderer = NULL;
    background_texture = NULL;
    basketball_texture = NULL;
    player1_texture = NULL;
    player2_texture = NULL;
    net_texture = NULL;
    text_bitmap = NULL;
}

gfx_engine::~gfx_engine()
{
    freeSDL();
}

// initialize SDL2
bool gfx_engine::initSDL()
{
    // initialize all of SDL2's utilities/mechanisms etc...
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
        return false;

    // initialize SDL_Window instance screen
    screen = SDL_CreateWindow("Basketball Game", 10, 10,
                              1000, 600, SDL_WINDOW_SHOWN);

    if(screen == NULL)
        return false;

    // initialize the renderer
    renderer = SDL_CreateRenderer(screen, -1, 0);

    if(renderer == NULL)
        return false;

    player1_texture = IMG_LoadTexture(renderer, "redguy1.png");

    if(player1_texture == NULL)
        return false;

    player2_texture = IMG_LoadTexture(renderer, "yellowguy1.png");

    if(player2_texture == NULL)
        return false;

    // initialize the font bitmap texture
    background_texture = IMG_LoadTexture(renderer, "backgroundsprite.png");

    if(background_texture == NULL)
        return false;

    basketball_texture = IMG_LoadTexture(renderer, "basketballsprite.png");

    if(basketball_texture == NULL)
        return false;

    net_texture = IMG_LoadTexture(renderer, "netsprite.png");

    if(net_texture == NULL)
        return false;

    text_bitmap = IMG_LoadTexture(renderer, "bitmapfont.png");

    if(text_bitmap == NULL)
        return false;

    //SDL_RenderSetScale(renderer,1.0f,1.0f);

    return true;
}


// free/destroy graphics ptr objects
void gfx_engine::freeSDL()
{
    if (background_texture != NULL)
    {
        SDL_DestroyTexture(background_texture);
        background_texture = NULL;
    }

    if (player1_texture != NULL)
    {
        SDL_DestroyTexture(player1_texture);
        player1_texture = NULL;
    }

    if (player2_texture != NULL)
    {
        SDL_DestroyTexture(player2_texture);
        player2_texture = NULL;
    }

    if (basketball_texture != NULL)
    {
        SDL_DestroyTexture(basketball_texture);
        basketball_texture = NULL;
    }

    if (net_texture != NULL)
    {
        SDL_DestroyTexture(net_texture);
        net_texture = NULL;
    }

    if (text_bitmap != NULL)
    {
        SDL_DestroyTexture(text_bitmap);
        text_bitmap = NULL;
    }

    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }

    if (screen != NULL)
    {
        SDL_DestroyWindow(screen);
        screen = NULL;
    }

    std::cout << "In destructor\n";

    SDL_Quit();
}

void gfx_engine::addGFXObject(obj_type t_val, point loc, point area, int frame, double angle, SDL_RendererFlip orientation)
{
    SDL_Rect rect;
    SDL_Rect crop;

    rect.x = (int)loc.x();
    rect.y = (int)loc.y();
    rect.w = crop.w = (int)area.x();
    rect.h = crop.h = (int)area.y();
    crop.x = (int)area.x()*frame;
    crop.y = 0;

    switch(t_val)
    {
        case(OBJ_BACKGROUND):
            SDL_RenderCopy(renderer,background_texture,NULL,&rect);
            break;
        case(OBJ_PLAYER1):
            SDL_RenderCopyEx(renderer,player1_texture,&crop,&rect,0.0,NULL,orientation);
            break;
        case(OBJ_PLAYER2):
            SDL_RenderCopyEx(renderer,player2_texture,&crop,&rect,0.0,NULL,orientation);
            break;
        case(OBJ_BASKETBALL):
            SDL_RenderCopyEx(renderer,basketball_texture,NULL,&rect,angle,NULL,SDL_FLIP_NONE);
            break;
        case(OBJ_NET):
            SDL_RenderCopyEx(renderer,net_texture,&crop,&rect,0.0,NULL,orientation);
            break;
        default:
            break;
    }
}

// print a string (sval) to the screen one bitmap character at a time
// at location (x,y) with color col.
void gfx_engine::addBitmapString(SDL_Color col, std::string string_val, point loc)
{
    for (int i = 0; i < (int)string_val.size(); ++i)
    {
        addBitmapCharacter(col, (int)string_val[i],point(loc.x()+(i*TILE_WIDTH),loc.y()));
    }
}

// render portion of font_bitmap to screen with color col
void gfx_engine::addBitmapCharacter(SDL_Color col, int ascii_val, point loc)
{
    // boundary checking
    if (ascii_val < 0 || ascii_val > 255)
        return;

    SDL_Rect rect;
    SDL_Rect crop;

    rect.x = (int)loc.x();
    rect.y = (int)loc.y();
    rect.w = crop.w = TILE_WIDTH;
    rect.h = crop.h = TILE_HEIGHT;

    // crop out bitmap tile given aval, TILEWID, and TILEHGT
    // BITMAPROWS and BITMAPCOLS should stay at 16
    crop.x = (ascii_val % TEXT_BITMAP_COLS) * TILE_WIDTH;
    crop.y = (int)(ascii_val / TEXT_BITMAP_ROWS) * TILE_HEIGHT;

    //Render foreground character to screen (via parsing the bitmap font)
    SDL_SetTextureColorMod(text_bitmap,col.r,col.g,col.b);
    SDL_RenderCopy(renderer,text_bitmap,&crop,&rect);
}


void gfx_engine::drawRectangle(SDL_Color c, point loc, point area)
{
    SDL_Rect rect = {(int)loc.x(),(int)loc.y(),(int)area.x(),(int)area.y()};
    SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,0);
    SDL_RenderFillRect(renderer,&rect);
}

// clear screen to black
void gfx_engine::clearScreen()
{
    SDL_RenderClear(renderer);
}

// update display
void gfx_engine::updateScreen()
{
    SDL_RenderPresent(renderer);
}

#include "game.h"

int main(int argc, char* argv[])
{
    // seed random number generator
    srand(time(NULL));
    // create instance of game obj (contains all program data)
    game game_obj;
    // execute program
    game_obj.run();
    // When "gfx_engine" instance goes out of scope, its
    // destructor is called freeing all SDL textures, the
    // renderer, window, etc... (implicitly)
    return 0;
}

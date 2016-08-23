#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include <string>
#include <iostream>
#include "SDL/SDL_ttf.h"
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1024;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 10;
const int FOO_WIDTH = 64;
const int FOO_HEIGHT = 205;
const int FOO_RIGHT = 0;
const int FOO_LEFT = 1;
SDL_Surface *foo = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *background = NULL;
SDL_Surface *background2 = NULL;
SDL_Surface *message = NULL;
SDL_Surface *message2 = NULL;
SDL_Event event;
TTF_Font *font = NULL;//The font that's going to be used
TTF_Font *font2 = NULL;
SDL_Color white = { 255, 255, 255 };
SDL_Color black = { 0, 0, 0 };
SDL_Rect clipsRight[ 4 ];
SDL_Rect clipsLeft[ 4 ];
Mix_Music *music = NULL;

class Foo
{
    private:
    public:
    int offSet;
    int velocity;
    int frame;
    int status;
    Foo();
    void handle_events();
    void move();
    void show(int);
    void show2();
};

class Timer
{
    private:
    int startTicks;
    int pausedTicks;
    bool paused;
    bool started;

    public:
    Timer();
    void start();
    void stop();
    void pause();
    void unpause();
    int get_ticks();
    bool is_started();
    bool is_paused();
};

SDL_Surface *load_image( std::string filename )
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;
    loadedImage = IMG_Load( filename.c_str() );
    if( loadedImage != NULL )
    {
        optimizedImage = SDL_DisplayFormat( loadedImage );
        SDL_FreeSurface( loadedImage );
        if( optimizedImage != NULL )
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
    }
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface( source, clip, destination, &offset );
}

void set_clips()
{
    clipsRight[ 0 ].x = 0;
    clipsRight[ 0 ].y = 0;
    clipsRight[ 0 ].w = FOO_WIDTH;
    clipsRight[ 0 ].h = FOO_HEIGHT;

    clipsRight[ 1 ].x = FOO_WIDTH;
    clipsRight[ 1 ].y = 0;
    clipsRight[ 1 ].w = FOO_WIDTH;
    clipsRight[ 1 ].h = FOO_HEIGHT;

    clipsRight[ 2 ].x = FOO_WIDTH * 2;
    clipsRight[ 2 ].y = 0;
    clipsRight[ 2 ].w = FOO_WIDTH;
    clipsRight[ 2 ].h = FOO_HEIGHT;

    clipsRight[ 3 ].x = FOO_WIDTH * 3;
    clipsRight[ 3 ].y = 0;
    clipsRight[ 3 ].w = FOO_WIDTH;
    clipsRight[ 3 ].h = FOO_HEIGHT;

    clipsLeft[ 0 ].x = 0;
    clipsLeft[ 0 ].y = FOO_HEIGHT;
    clipsLeft[ 0 ].w = FOO_WIDTH;
    clipsLeft[ 0 ].h = FOO_HEIGHT;

    clipsLeft[ 1 ].x = FOO_WIDTH;
    clipsLeft[ 1 ].y = FOO_HEIGHT;
    clipsLeft[ 1 ].w = FOO_WIDTH;
    clipsLeft[ 1 ].h = FOO_HEIGHT;

    clipsLeft[ 2 ].x = FOO_WIDTH * 2;
    clipsLeft[ 2 ].y = FOO_HEIGHT;
    clipsLeft[ 2 ].w = FOO_WIDTH;
    clipsLeft[ 2 ].h = FOO_HEIGHT;

    clipsLeft[ 3 ].x = FOO_WIDTH * 3;
    clipsLeft[ 3 ].y = FOO_HEIGHT;
    clipsLeft[ 3 ].w = FOO_WIDTH;
    clipsLeft[ 3 ].h = FOO_HEIGHT;
}

bool init()
{
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
        return false;
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
    if( screen == NULL )
        return false;
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
        return false;
    if( TTF_Init() == -1 )
        return false;
    SDL_WM_SetCaption( "Mario: Ray Tracing", NULL );
    return true;
}

bool load_files()
{
    foo = load_image( "foo.png" );
    background = load_image( "bg.png" );
    background2 = load_image( "end.png" );
    music = Mix_LoadMUS( "beat2.wav" );
    font = TTF_OpenFont( "lazy2.otf", 40 );
    font2 = TTF_OpenFont( "lazy2.otf", 72 );
    return true;
}

void clean_up()
{
    SDL_FreeSurface( foo );
    Mix_FreeMusic( music );
    SDL_FreeSurface( message );
    SDL_FreeSurface( message2 );
    TTF_CloseFont( font );
    TTF_CloseFont( font2 );
    TTF_Quit();
    SDL_Quit();
}

Foo::Foo()
{
    offSet = 0;
    velocity = 0;
    frame = 0;
    status = FOO_RIGHT;
}

void Foo::handle_events()
{
    if( event.type == SDL_KEYDOWN )
    {
        switch( event.key.keysym.sym )
        {
            case SDLK_RIGHT: velocity += FOO_WIDTH / 4; break;
            case SDLK_LEFT: velocity -= FOO_WIDTH / 4; break;
        }
    }
    else if( event.type == SDL_KEYUP )
    {
        switch( event.key.keysym.sym )
        {
            case SDLK_RIGHT: velocity -= FOO_WIDTH / 4; break;
            case SDLK_LEFT: velocity += FOO_WIDTH / 4; break;
        }
    }
}

void Foo::move()
{
    offSet += velocity;
    if( ( offSet < 0 ) || ( offSet + FOO_WIDTH > SCREEN_WIDTH ) )
        offSet -= velocity;
}

void Foo::show(int bgx)
{
    if( velocity < 0 )
    {
        status = FOO_LEFT;
        frame++;    
    }
    else if( velocity > 0 )
    {
        status = FOO_RIGHT;
        frame++;
    }
    frame++;    
    if( frame >= 4 )
        frame = 0;
    int offsety = 90;
    if ((bgx < -1300) && bgx > -1580) 
    {
        offsety += (-1300-bgx)/2;
        frame = 0;
    }
    else if (bgx <= -1580 && bgx > -1860)
    {
        offsety += (bgx+1860)/2;
        frame = 0;
        if (offsety < 90)
            offsety = 90;
    }
    if( status == FOO_RIGHT )
        apply_surface( 150, SCREEN_HEIGHT - FOO_HEIGHT - offsety, foo, screen, &clipsRight[ frame ] );
    else if( status == FOO_LEFT )
        apply_surface( 150, SCREEN_HEIGHT - FOO_HEIGHT - offsety, foo, screen, &clipsLeft[ frame ] );
}

void Foo::show2()
{
    if( velocity < 0 )
    {
        status = FOO_LEFT;
        frame++;    
    }
    else if( velocity > 0 )
    {
        status = FOO_RIGHT;
        frame++;
    }
    else
        frame = 0;
    if( frame >= 4 )
        frame = 0;
    if( status == FOO_RIGHT )
        apply_surface( offSet, SCREEN_HEIGHT - FOO_HEIGHT - 90, foo, screen, &clipsRight[ frame ] );
    else if( status == FOO_LEFT )
        apply_surface( offSet, SCREEN_HEIGHT - FOO_HEIGHT - 90, foo, screen, &clipsLeft[ frame ] );
}

Timer::Timer()
{
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

void Timer::start()
{
    started = true;
    paused = false;
    startTicks = SDL_GetTicks();
}

void Timer::stop()
{
    started = false;
    paused = false;
}

void Timer::pause()
{
    if( ( started == true ) && ( paused == false ) )
    {
        paused = true;
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unpause()
{
    if( paused == true )
    {
        paused = false;
        startTicks = SDL_GetTicks() - pausedTicks;
        pausedTicks = 0;
    }
}

int Timer::get_ticks()
{
    if( started == true )
    {
        if( paused == true )
            return pausedTicks;
        else
            return SDL_GetTicks() - startTicks;
    }
    return 0;
}

bool Timer::is_started()
{
    return started;
}

bool Timer::is_paused()
{
    return paused;
}

int main( int argc, char* args[] )
{
    bool quit = false;
    if( init() == false )
        return 1;
    if( load_files() == false )
        return 1;
    set_clips();
    int bgX = 0, bgY = 0;
    Timer fps;
    Foo walk;
    int loop = 0;
    message2 = TTF_RenderText_Solid( font2, "Game over! KapSid Clears the level!!", black );
    if( message2 == NULL )
        return 1;
    if( Mix_PlayingMusic() == 0 )
        if( Mix_PlayMusic( music, -1 ) == -1 )
            return 1;
    while( quit == false && loop <3)
    {
        fps.start();
        walk.velocity = 0;
        walk.move();
        bgX -= 15;
        if( bgX <= -background->w )
        {
            bgX = 0;
            loop++;
        }
        if (loop<2)
        {
            apply_surface( bgX, bgY, background, screen);
            apply_surface( bgX + background->w, bgY, background, screen );
        }
        else if (loop<3)
        {
            apply_surface( bgX, bgY, background, screen);
            apply_surface( bgX + background->w, bgY, background2, screen );   
        }
        else
            apply_surface( bgX, bgY, background2, screen);
        message = TTF_RenderText_Solid( font, "In this assignment, he had to escape from the evil rays shot by Adrishya", black );
        apply_surface( 200, 450, message, screen );
        if(loop == 0)
            message = TTF_RenderText_Solid( font, "Score: 0", black );
        else if(loop ==1)
            message = TTF_RenderText_Solid( font, "Score: 1", black );
        else if(loop == 2)
            message = TTF_RenderText_Solid( font, "Score: 2", black );
        else 
            message = TTF_RenderText_Solid( font, "Score: 3", black );
        apply_surface( 1600, 50, message, screen );
        walk.show(bgX);
        if( SDL_Flip( screen ) == -1 )
            return 1;
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
    }
    walk.offSet = 150;
    while( quit == false && walk.offSet < 1088)
    {
        fps.start();
        walk.velocity = FOO_WIDTH/4;
        walk.move();
        message = TTF_RenderText_Solid( font, "Score: 3", black );
        apply_surface( bgX, bgY, background2, screen);
        apply_surface( bgX + background->w, bgY, background2, screen );
        apply_surface( 1600, 50, message, screen );
        apply_surface( 300, 400, message2, screen );
        walk.show2();
        if( SDL_Flip( screen ) == -1 )
            return 1;
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
    }
    clean_up();
    return 0;
}

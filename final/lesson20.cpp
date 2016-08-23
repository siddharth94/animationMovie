#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <unistd.h>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1024;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 10;
const int FOO_WIDTH = 64;
const int FOO_HEIGHT = 205;
const int FOO_RIGHT = 0;
const int FOO_LEFT = 1;
SDL_Surface *foo = NULL;
SDL_Surface *nimbu = NULL;
SDL_Surface *bomb = NULL;
SDL_Surface *boom = NULL;
SDL_Surface *background = NULL;
SDL_Surface *cs360 = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *message = NULL;
SDL_Event event;
SDL_Rect clipsRight[ 4 ];
SDL_Rect clipsLeft[ 4 ];
Mix_Music *music = NULL;
TTF_Font *font = NULL;//The font that's going to be used
SDL_Color white = { 255, 255, 255 };
SDL_Color black = { 0, 0, 0 };

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
    void show();
    void show_bomb();
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
        {
            if (filename == "foo.png")
                SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
            else if (filename == "newnimbu.png")
                SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0xFF, 0xFF, 0xFF ) );
            else if (filename == "bomb.png" || filename == "boom2.png")
                SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0x00, 0x00, 0x00 ) );
        }
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
    SDL_WM_SetCaption( "Final ShowDown", NULL );
    return true;
}

bool load_files()
{
    foo = load_image( "foo.png" );
    nimbu = load_image("newnimbu.png");
    bomb = load_image("bomb.png");
    boom = load_image("boom2.png");
    background = load_image( "bg2.jpg" );
    cs360 = load_image( "cs3601.png" );
    music = Mix_LoadMUS( "beat.wav" );
    font = TTF_OpenFont( "lazy2.otf", 40 );
    return true;
}

void clean_up()
{
    SDL_FreeSurface( foo );
    Mix_FreeMusic( music );
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
}

void Foo::show()
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
    {
        apply_surface( offSet, SCREEN_HEIGHT - FOO_HEIGHT - 120, foo, screen, &clipsRight[ frame ] );
        apply_surface( SCREEN_WIDTH - offSet, SCREEN_HEIGHT - FOO_HEIGHT - 240, nimbu, screen);
    }
    else if( status == FOO_LEFT )
    {
        apply_surface( offSet, SCREEN_HEIGHT - FOO_HEIGHT -120, foo, screen, &clipsLeft[ frame ] );
        apply_surface( SCREEN_WIDTH - offSet, SCREEN_HEIGHT - FOO_HEIGHT - 240, nimbu, screen);
    }
}

void Foo::show_bomb()
{
    apply_surface( 1020, offSet, bomb, screen);
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
    if( Mix_PlayingMusic() == 0 )
        if( Mix_PlayMusic( music, -1 ) == -1 )
            return 1;
    set_clips();
    Timer fps;
    Foo walk;
    Foo mbomb;
    while( quit == false && walk.offSet < 720)
    {
        fps.start();
        walk.velocity = FOO_WIDTH/4;
        walk.move();
        apply_surface( 0, 0, background, screen);
        message = TTF_RenderText_Solid( font, "In the final battle, KapSid has to face captain Nimbu", black );
        apply_surface( 200, 250, message, screen );
        walk.show();
        if( SDL_Flip( screen ) == -1 )
            return 1;
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
    }
    while( quit == false && mbomb.offSet < 640)
    {
        fps.start();
        mbomb.velocity = FOO_WIDTH/4;
        mbomb.move();
        apply_surface( 0, 0, background, screen);
        message = TTF_RenderText_Solid( font, "In the final battle, KapSid has to face captain Nimbu", black );
        apply_surface( 200, 250, message, screen );
        message = TTF_RenderText_Solid( font, "To show the skills acquired over the span of semester,", black );
        apply_surface( 200, 285, message, screen );
        message = TTF_RenderText_Solid( font, "KapSid built a ballistic missile. May the Force be with him!", black );
        apply_surface( 200, 320, message, screen );
        apply_surface( walk.offSet, SCREEN_HEIGHT - FOO_HEIGHT - 120, foo, screen, &clipsRight[ 0 ] );
        apply_surface( SCREEN_WIDTH - walk.offSet, SCREEN_HEIGHT - FOO_HEIGHT - 240, nimbu, screen);
        mbomb.show_bomb();
        if( SDL_Flip( screen ) == -1 )
            return 1;
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
    }
    fps.start();
    apply_surface( 820, mbomb.offSet - 540  , boom, screen);
    if( SDL_Flip( screen ) == -1 )
        return 1;
    if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
        SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
    sleep(2);

    fps.start();
    apply_surface( 10, 10, cs360, screen);
    message = TTF_RenderText_Solid( font, "Its End-Sem Time!", black );
    apply_surface( 500, 820, message, screen );
    if( SDL_Flip( screen ) == -1 )
        return 1;
    if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
        SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
    sleep(2);

    clean_up();
    return 0;
}

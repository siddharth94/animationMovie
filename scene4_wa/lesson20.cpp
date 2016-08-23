#include <bits/stdc++.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"

class Point
{
public:
    double x,y;
    Point(double x1, double y1)
    {
        x = x1;
        y = y1;
    };
    Point()
    {
        x = 0.0;
        y = 0.0;
    };
    Point operator=(const Point pt);
    bool operator==(const Point pt);
    Point operator+(const Point pt) const;
    Point operator*(double m) const;
};

bool Point::operator==(const Point pt)
{
    if (x == pt.x && y == pt.y)
        return true;
    return false;
}
 
Point Point::operator=(const Point pt)
{
    x = pt.x;
    y = pt.y;
    return *this;
}

Point Point::operator+(const Point pt) const
{
    Point temp;
    temp.x = x + pt.x;
    temp.y = y + pt.y;
    return temp;
}

Point Point::operator*(double m) const
{
    Point temp;
    temp.x = x*m;
    temp.y = y*m;
    return temp;
}

std::vector< std::vector<Point> > v, final_points;
std::vector< Point > tomatoes;
int tcount=0;
int countv=0;
Point dp[100][1005];

Point deBoor(int k, int degree, int i, double x, double* knots, std::vector<Point> ctrlPoints)
{
    if( k == 0)
        return ctrlPoints[i];
    else
    {    
        double alpha = (x-knots[i])/(knots[i+degree+1-k]-knots[i]);
        if (dp[k-1][i-1] == Point())
            dp[k-1][i-1] = deBoor(k-1,degree, i-1, x, knots, ctrlPoints);
        if (dp[k-1][i] == Point())
            dp[k-1][i] = deBoor(k-1,degree, i, x, knots, ctrlPoints);
        Point p1 = dp[k-1][i-1];
        Point p2 = dp[k-1][i];
        return (p1*(1-alpha )+p2*alpha );
    }
}

int WhichInterval(double x, double *knot, int ti)
{
    for(int i=1;i<ti-1;i++)
    {
        if(x<knot[i])
            return(i-1);
        else if(x == knot[ti-1])
           return(ti-1);
    }
    return -1;
}

void compute(std::vector<Point> v, int degree=3)
{
    int control=v.size();
    if(control<=degree) return;
    else
    {
        double t, knots[degree+control+1];

        for (int i=0; i<degree+1; i++)
            knots[i] = 0;
        for(int i=degree+1;i<control+1;i++)
            knots[i]=((float)(i-degree+1))/(control+1-degree);    
        for (int i=0; i<degree+1; i++)
            knots[control+i+1] = 1;

        int imax=1000;
        for(int i=0;i<imax;i++)
        {
            memset(dp, 0, sizeof(dp));
            t = ((float)i)/imax;
            int l = WhichInterval(t, knots, degree+control+1);

            final_points[countv].push_back(deBoor(degree, degree, l, t, knots, v));
        }
    }
}

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1024;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 10;
const int FOO_WIDTH = 64;
const int FOO_HEIGHT = 205;
const int FOO_RIGHT = 0;
const int FOO_LEFT = 1;
SDL_Surface *foo = NULL;
SDL_Surface *tomato = NULL;
SDL_Surface *oops = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *message = NULL;
SDL_Event event;
SDL_Rect clipsRight[ 4 ];
SDL_Rect clipsLeft[ 4 ];
Mix_Chunk *music = NULL;
TTF_Font *font = NULL;//The font that's going to be used
SDL_Color white = { 255, 255, 255 };
SDL_Color black = { 0, 0, 0 };
class Foo
{
    private:
    public:
    int offSetx;
    int offSety;
    int velocity;
    int frame;
    int status;
    Foo();
    void handle_events();
    void move(int);
    void show();
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
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
            if (filename == "tomato1.jpg" || filename == "oops1.jpg")
                SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0xFF, 0xFF, 0xFF ) );
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
    SDL_WM_SetCaption( "Tomatoes", NULL );
    return true;
}

bool load_files()
{
    foo = load_image( "foo.png" );
    tomato = load_image( "tomato1.jpg" );
    oops = load_image( "oops1.jpg" );
    music = Mix_LoadWAV( "beat.wav" );
    font = TTF_OpenFont( "lazy2.otf", 40 );
    return true;
}

void clean_up()
{
    SDL_FreeSurface( foo );
    SDL_Quit();
}

Foo::Foo()
{
    offSetx = 0;
    offSety = 0;
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

void Foo::move(int i)
{
    offSetx = final_points[countv][i].x;
    offSety = final_points[countv][i].y;
}
bool flag = false;

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
        apply_surface( offSetx, offSety - 120, foo, screen, &clipsRight[ frame ] );
        for (int i=0; i<tomatoes.size(); i++)
        {
            apply_surface(1*tomatoes[i].x, 1.1*tomatoes[i].y, tomato, screen);
            if (abs(tomatoes[i].x - offSetx) < 80)
            {
                apply_surface( 850, 10, oops, screen);
                flag = true;
            }
        }
    }
    else if( status == FOO_LEFT )
    {
        apply_surface( offSetx, offSety, foo, screen, &clipsLeft[ frame ] );
        for (int i=0; i<tomatoes.size(); i++)
        {
            apply_surface( 1*tomatoes[i].x, 1.1*tomatoes[i].y, tomato, screen);
            if (abs(tomatoes[i].x - offSetx) < 80)
            {
                apply_surface( 850, 10, oops, screen);
                flag = true;
            }
        }
    }
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
    std::vector<Point> v1;
    v.push_back(v1);

    for (int i=0; i<13; i++)
    {
        int x,y;
        std::cin >> x >> y;
        Point p(x,y);
        v[0].push_back(p);
    }
    final_points.push_back(v1);
    compute(v[countv]);
    //for (int i=0; i<final_points[0].size(); i++)
    //{
        //std::cout << final_points[0][i].x << " " << final_points[0][i].y << "\n";
    //}
    //Quit flag
    bool quit = false;
    if( init() == false )
        return 1;
    if( load_files() == false )
        return 1;
    set_clips();
    Timer fps;
    Foo walk;
    int i=0;
    while( quit == false && i<final_points[0].size())
    {
        if (final_points[0][i].x > v[0][tcount].x+50)
        {
            tomatoes.push_back(Point (v[0][tcount].x, v[0][tcount].y));
            tcount++;   
        }
        fps.start();
        walk.velocity = FOO_WIDTH;
        walk.move(i);
        if (flag)
            if(Mix_Playing(-1) == 0)
                if( Mix_PlayChannel( -1, music, 0 ) == -1 )
                    return 1;
        flag = false;
        SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
        message = TTF_RenderText_Solid( font, "In the prefinal mission, both Adrishya and iSHAAN try to stop KapSid", black );
        apply_surface( 200, 250, message, screen );
        message = TTF_RenderText_Solid( font, "when they had individually failed", black );
        apply_surface( 200, 285, message, screen );
        message = TTF_RenderText_Solid( font, "Let's see what destiny has in store for him!", black );
        apply_surface( 200, 320, message, screen );
        walk.show();
        if( SDL_Flip( screen ) == -1 )
            return 1;
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
        i += 5;
    }
    clean_up();
    return 0;
}

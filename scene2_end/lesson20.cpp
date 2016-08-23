#include <bits/stdc++.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

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
bool vis[12];

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

void compute(std::vector<Point> v, int degree=1)
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
SDL_Surface *screen = NULL;
SDL_Surface *background = NULL;
SDL_Surface *ip1 = NULL;
SDL_Surface *ip2 = NULL;
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

int prex, prey;
char str[12][20];
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
    SDL_WM_SetCaption( "iSHAAN's Dungeon", NULL );
    if( TTF_Init() == -1 )
        return false;
    return true;
}

bool load_files()
{
    foo = load_image( "foo.png" );
    background = load_image("bg2.jpg");
    ip1 = load_image("ip1.jpg");
    ip2 = load_image("ip22.jpg");
    font = TTF_OpenFont( "lazy2.otf", 40 );
    font2 = TTF_OpenFont( "lazy2.otf", 72 );
    music = Mix_LoadMUS( "beat.wav" );
    return true;
}

void clean_up()
{
    SDL_FreeSurface( foo );
    SDL_FreeSurface( message );
    SDL_FreeSurface( message2 );
    TTF_CloseFont( font );
    TTF_CloseFont( font2 );
    TTF_Quit();
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

void Foo::show()
{
    if( prex > offSetx )
    {
        status = FOO_LEFT;
        frame++;
    }
    else if( prex < offSetx )
    {
        status = FOO_RIGHT;
        frame++;
    }
    else
        frame = 0;
    prex = offSetx;
    if( frame >= 4 )
        frame = 0;
    if( status == FOO_RIGHT )
    {
        apply_surface( offSetx, offSety - 120, foo, screen, &clipsRight[ frame ] );
        int count = 0;
        for (int i=0; i<v[0].size(); i++)
        {
            if (vis[i]==1)
            {
                count++;
                continue;
            }
            if (message==NULL)
            {
                std::cerr<<"null";
                exit(0);
            }
            if (i%2)
                apply_surface( v[0][i].x, v[0][i].y - 10, ip1, screen);
            else
                apply_surface( v[0][i].x, v[0][i].y - 10, ip2, screen);

            if(abs(offSetx-v[0][i].x)<20 && abs(offSety-v[0][i].y)<20)
                vis[i]=1;
        }
        if(count>0)
            count--;
        if(count>10)
            count=10;
        message = TTF_RenderText_Solid( font, str[count], white );
        apply_surface( 1600, 50, message, screen );


    }
    else if( status == FOO_LEFT )
    {
        apply_surface( offSetx, offSety - 120, foo, screen, &clipsLeft[ frame ] );
        int count=0;
        for (int i=0; i<v[0].size(); i++)
        {
            if (vis[i]==1)
            {
                count++;
                continue;
            }
            if (i%2)
                apply_surface( v[0][i].x, v[0][i].y - 10, ip1, screen);
            else
                apply_surface( v[0][i].x, v[0][i].y - 10, ip2, screen);

            if(abs(offSetx-v[0][i].x)<20 && abs(offSety-v[0][i].y)<20)
                vis[i]=1;
        }
        if(count>0)
            count--;
        if(count>10)
            count=10;
        message = TTF_RenderText_Solid( font, str[count], white );
        if(message==NULL)
            exit(0);
        apply_surface( 1600, 50, message, screen );
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
    prex = 9999;
    prey = 9999;
    std::vector<Point> v1;
    v.push_back(v1);
    memset(vis, 0, sizeof(vis));
    for (int i=0; i<12; i++)
    {
        str[i][0]='i';
        str[i][1]='P';
        str[i][2]='h';
        str[i][3]='o';
        str[i][4]='n';
        str[i][5]='e';
        str[i][6]=':';
        str[i][7]=' ';
        str[i][8]=i/10 + 48;
        str[i][9]=i%10 + 48;
        str[i][10]='\0';
        int x,y;
        std::cin >> x >> y;
        Point p(x,y);
        v[0].push_back(p);
    }
    final_points.push_back(v1);
    compute(v[countv]);
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
    int i=0;
    while( quit == false && i<final_points[0].size())
    {
        fps.start();
        walk.velocity = FOO_WIDTH;
        walk.move(i);
        apply_surface( 0, 0, background, screen);
        message = TTF_RenderText_Solid( font, "iSHAAN's Dungeon", white );
        apply_surface( 80, 50, message, screen );
        message = TTF_RenderText_Solid( font, "His second task was to clear master iSHAAN's dungeon by collecting all his iPhones", white );
        apply_surface( 80, 100, message, screen );
        walk.show();
        if( SDL_Flip( screen ) == -1 )
            return 1;
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
        {
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
        }
        i += 5;
    }
    clean_up();

    return 0;
}

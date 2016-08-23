/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include <bits/stdc++.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

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
        // cout << i << " in deboor" << endl;
        double alpha = (x-knots[i])/(knots[i+degree+1-k]-knots[i]);
        if (dp[k-1][i-1] == Point())
            dp[k-1][i-1] = deBoor(k-1,degree, i-1, x, knots, ctrlPoints);
        if (dp[k-1][i] == Point())
            dp[k-1][i] = deBoor(k-1,degree, i, x, knots, ctrlPoints);
        Point p1 = dp[k-1][i-1];
        Point p2 = dp[k-1][i];
        // cout << p1.x << " " << p1.y << " " << p2.x << " " << p2.y << " " << alpha <<  endl;
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
        // cout<<"Minimum "<<degree+1<<" points required to generate curve!";
    else
    {
        double t, knots[degree+control+1];

        for (int i=0; i<degree+1; i++)
            knots[i] = 0;
        for(int i=degree+1;i<control+1;i++)
            knots[i]=((float)(i-degree+1))/(control+1-degree);    
        for (int i=0; i<degree+1; i++)
            knots[control+i+1] = 1;
        //knot vector ready

        int imax=1000;
        for(int i=0;i<imax;i++)
        {
            memset(dp, 0, sizeof(dp));
            t = ((float)i)/imax;
            int l = WhichInterval(t, knots, degree+control+1);

            final_points[countv].push_back(deBoor(degree, degree, l, t, knots, v));
            // cout<<i<<" "<<final_points[countv][i].x<<" "<<final_points[countv][i].y<<endl;
        }
    }
}

//Screen attributes
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1024;
const int SCREEN_BPP = 32;

//The frames per second
const int FRAMES_PER_SECOND = 10;

//The dimenstions of the stick figure
const int FOO_WIDTH = 64;
const int FOO_HEIGHT = 205;

//The direction status of the stick figure
const int FOO_RIGHT = 0;
const int FOO_LEFT = 1;

//The surfaces
SDL_Surface *foo = NULL;
SDL_Surface *tomato = NULL;
SDL_Surface *oops = NULL;
SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;

//The event structure
SDL_Event event;

//The areas of the sprite sheet
SDL_Rect clipsRight[ 4 ];
SDL_Rect clipsLeft[ 4 ];

//The stick figure
class Foo
{
    private:
    public:
    //The offset
    int offSetx;
    int offSety;

    //Its rate of movement
    int velocity;

    //Its current frame
    int frame;

    //Its animation status
    int status;

    //Initializes the variables
    Foo();

    //Handles input
    void handle_events();

    //Moves the stick figure
    void move(int);

    //Shows the stick figure
    void show();
};

//The timer
class Timer
{
    private:
    //The clock time when the timer started
    int startTicks;

    //The ticks stored when the timer was paused
    int pausedTicks;

    //The timer status
    bool paused;
    bool started;

    public:
    //Initializes variables
    Timer();

    //The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    //Gets the timer's time
    int get_ticks();

    //Checks the status of the timer
    bool is_started();
    bool is_paused();
};

SDL_Surface *load_image( std::string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
            if (filename == "tomato1.jpg" || filename == "oops1.jpg")
                SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0xFF, 0xFF, 0xFF ) );
        }
    }

    //Return the optimized surface
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

void set_clips()
{
    //Clip the sprites
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
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "Animation Test", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the sprite sheet
    foo = load_image( "foo.png" );
    tomato = load_image( "tomato1.jpg" );
    oops = load_image( "oops1.jpg" );
    background = load_image( "bg2.jpg" );

    //If there was a problem in loading the sprite
    if( foo == NULL )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Free the surface
    SDL_FreeSurface( foo );

    //Quit SDL
    SDL_Quit();
}

Foo::Foo()
{
    //Initialize movement variables
    offSetx = 0;
    offSety = 0;
    velocity = 0;

    //Initialize animation variables
    frame = 0;
    status = FOO_RIGHT;
}

void Foo::handle_events()
{
    //If a key was pressed
    if( event.type == SDL_KEYDOWN )
    {
        //Set the velocity
        switch( event.key.keysym.sym )
        {
            case SDLK_RIGHT: velocity += FOO_WIDTH / 4; break;
            case SDLK_LEFT: velocity -= FOO_WIDTH / 4; break;
        }
    }
    //If a key was released
    else if( event.type == SDL_KEYUP )
    {
        //Set the velocity
        switch( event.key.keysym.sym )
        {
            case SDLK_RIGHT: velocity -= FOO_WIDTH / 4; break;
            case SDLK_LEFT: velocity += FOO_WIDTH / 4; break;
        }
    }
}

void Foo::move(int i)
{
    //Move
    offSetx = final_points[countv][i].x;
    offSety = final_points[countv][i].y;
}

void Foo::show()
{
    //If Foo is moving left
    if( velocity < 0 )
    {
        //Set the animation to left
        status = FOO_LEFT;

        //Move to the next frame in the animation
        frame++;
    }
    //If Foo is moving right
    else if( velocity > 0 )
    {
        //Set the animation to right
        status = FOO_RIGHT;

        //Move to the next frame in the animation
        frame++;
    }
    //If Foo standing
    else
    {
        //Restart the animation
        frame = 0;
    }

    //Loop the animation
    if( frame >= 4 )
    {
        frame = 0;
    }

    //Show the stick figure
    if( status == FOO_RIGHT )
    {
        // std::cerr << " " << offSetx << " " << offSety << " tight\n";
        apply_surface( offSetx, offSety - 120, foo, screen, &clipsRight[ frame ] );
        for (int i=0; i<tomatoes.size(); i++)
        {
            apply_surface( tomatoes[i].x, 1.1*tomatoes[i].y, tomato, screen);
            if (abs(tomatoes[i].x - offSetx) < 100)
                apply_surface( 850, 10, oops, screen);
        }
    }
    else if( status == FOO_LEFT )
    {
        std::cerr << "left\n";
        apply_surface( offSetx, offSety, foo, screen, &clipsLeft[ frame ] );
    }
}

Timer::Timer()
{
    //Initialize the variables
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

void Timer::start()
{
    //Start the timer
    started = true;

    //Unpause the timer
    paused = false;

    //Get the current clock time
    startTicks = SDL_GetTicks();
}

void Timer::stop()
{
    //Stop the timer
    started = false;

    //Unpause the timer
    paused = false;
}

void Timer::pause()
{
    //If the timer is running and isn't already paused
    if( ( started == true ) && ( paused == false ) )
    {
        //Pause the timer
        paused = true;

        //Calculate the paused ticks
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unpause()
{
    //If the timer is paused
    if( paused == true )
    {
        //Unpause the timer
        paused = false;

        //Reset the starting ticks
        startTicks = SDL_GetTicks() - pausedTicks;

        //Reset the paused ticks
        pausedTicks = 0;
    }
}

int Timer::get_ticks()
{
    //If the timer is running
    if( started == true )
    {
        //If the timer is paused
        if( paused == true )
        {
            //Return the number of ticks when the timer was paused
            return pausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            return SDL_GetTicks() - startTicks;
        }
    }

    //If the timer isn't running
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

    // for (int i=0; i<13; i++)
    // {
    //     int x,y;
    //     std::cin >> x >> y;
    //     Point p(x,y);
    //     v[0].push_back(p);
    // }
    final_points.push_back(v1);
    // compute(v[countv]);
    // for (int i=0; i<final_points[0].size(); i++)
    // {
    //     std::cout << final_points[0][i].x << " " << final_points[0][i].y << "\n";
    // }
    //Quit flag
    bool quit = false;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }

    //Clip the sprite sheet
    set_clips();

    //The frame rate regulator
    Timer fps;

    //The stick figure
    Foo walk;

    int i=0;

    //While the user hasn't quit
    while( quit == false && i<1)
    {
        // if (final_points[0][i].x > v[0][tcount].x+50)
        // {
        //     tomatoes.push_back(Point (v[0][tcount].x, v[0][tcount].y));
        //     tcount++;   
        // }
        //Start the frame timer
        fps.start();

        walk.velocity = FOO_WIDTH;

        //Move the stick figure
        walk.move(i);

        //Fill the screen white
        // SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
        apply_surface( 0, 0, background, screen);

        //Show the stick figure on the screen
        walk.show();

        //Update the screen
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }

        //Cap the frame rate
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
        {
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
        }
        
    }

    //Clean up
    clean_up();

    return 0;
}

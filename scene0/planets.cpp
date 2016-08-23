# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
# include <bits/stdc++.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

GLfloat rot, Vx, Vy, Vz;
Mix_Chunk *sound = NULL;
void createPlanet(float radius, float orbit_radius, float rotation_speed, float revolution_speed, float orbit_plane_rotation_z, int moons = 0)
{
    glPushMatrix();

    glRotatef(orbit_plane_rotation_z, 0.0f, 0.0f, 1.0f);
    glRotatef(rot*revolution_speed, 0.0f, 1.0f, 0.0f);
    float angle;
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<100; i++)
    {
        angle = 2*3.14*i/100;
        glVertex3f(orbit_radius*cos(angle), 0, orbit_radius*sin(angle));
    }
    glEnd();

    glTranslatef(orbit_radius, 0.0f, 0.0f);

    for (int i=1; i<=moons; i++)
        createPlanet(radius/5, radius+i*1.2, rotation_speed*2, revolution_speed*2, 23*i);

    glRotatef(rot*rotation_speed*10, -1.0f, 1.0f, 0.0f);
    glScalef(1.0f, 1.0f, 1.0f);

    glutWireSphere(radius, 50, 50);
    glPopMatrix();
}

void renderScene(void){
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glColor3f(253/255.0, 184/255.0, 19/255.0); 
    gluLookAt(Vx, Vy, Vz, 0.0, 0.0, 0.0, 1, -2, 1);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);

    glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(0.01*rot, 0.0f, 1.0f, 0.0f);
    glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
    glScalef(1.0f, 1.0f, 1.0f);

    glutWireSphere(7, 50, 50);
    glPopMatrix();

    glColor3f(0.2, 0.8, 0.5); 
    createPlanet(0.8, 24*0.387, 0.05/58.8, 0.1/0.241, 5);			//Mercury

    createPlanet(1.4, 24*0.723, -0.05/244, 0.1/0.615, 5);			//Venus

    createPlanet(1.6, 24, 0.05, 0.1, 5, 1);		//Earth

    createPlanet(1.2, 24*1.52, 0.05/1.03, 0.1/1.88, 5, 2);		//Mars

    createPlanet(5.0, 24*2.2, 0.05/0.415, 0.1/4.9, 5, 6);		//Jupiter

    createPlanet(4.6, 24*4.58, 0.05/0.445, 0.1/6.4, 5, 5);		//Saturn

    createPlanet(2.5, 24*6.20, -0.05/0.720, 0.1/8.7, 25, 3);		//Uranus

    createPlanet(2.5, 24*7.05, 0.05/0.673, 0.1/16.7, 5, 5);		//Neptune

    glFlush();
    glutSwapBuffers();
}

void changeSize(int x, int y){
    if (y == 0 || x == 0) return;

    GLfloat aspect = (GLfloat)(x/y);

    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0, 1, 0);
    gluPerspective(45.0f,aspect,0.1f, 10000.0f);
    glViewport(0,0,x,y);
    glMatrixMode(GL_MODELVIEW);
}

void animate(void){
    rot += 1;
    Vx += 0.059;
    Vy += 0.059;
    Vz += 0.059;
    if (Vx > 240 || Vy > 280 || Vz > 280) exit(0);
    renderScene();
}

int main (int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1920,1024);
    glutInitWindowPosition(0,0);
    glutCreateWindow("The story begins");
    glClearColor(0.0,0.0,0.0,0.0);
    rot = 0;
    Vx = Vy = Vz = 15;
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
        return false;
    sound = Mix_LoadWAV( "scene0.wav" );
    if( Mix_PlayChannel( -1, sound, 0 ) == -1 )
        return 1;
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(animate);					//for animation uncomment
    glutMainLoop();
    Mix_FreeChunk( sound );
    return 0;
}

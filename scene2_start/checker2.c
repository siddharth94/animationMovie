#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

int imageWidth; 
int imageHeight;
double door = 0;
double camx = 0, camz = 250;

static GLuint texName[10];

double rotate_y = 0, rotate_x = 0;
float room_h = 35;
float room_d = 80;
float room_w = 55;
Mix_Music *scratch = NULL;

void makeImage(const char * filename, int width, int height, int k)
{
   GLubyte image[width][height][3];
   FILE * file;
   file = fopen( filename, "rb" );

   if ( file == NULL ) return;
   fread( image, width * height * 3, 1, file );
   fclose( file );

   for(int i = 0; i < width ; ++i)
      for(int j = 0; j < height ; ++j)
      {
         unsigned char B,R;
         B = image[i][j][0];
         R = image[i][j][2];
         image[i][j][0] = R;
         image[i][j][2] = B;
      }
   glBindTexture(GL_TEXTURE_2D, texName[k]);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
}

void init(void)
{    
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);    
   glEnable(GL_DEPTH_TEST); // to enable hidden surface removal
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glGenTextures(10, texName);
   imageWidth = 431;//1022;
   imageHeight = 736;//1063;
   makeImage("dun3.bmp", imageWidth, imageHeight, 0);
   imageWidth = 600;
   imageHeight = 341;
   makeImage("lvl2e.bmp", imageWidth, imageHeight, 2);
}

void display(void)
{
    if(camz<1.2*room_d)
        exit(0);
   if (0.5*room_d <= camz && camz <= 3.0*room_d)
      door = (3.0*room_d-camz)*2*room_w/(9*0.5*room_d);
   glLoadIdentity();
   glPushMatrix();
      gluLookAt(camx, 10.0, camz, 0.0, 10.0, -100.0, 0.0, 1.0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear depth buffer and color buffer
      glEnable(GL_DEPTH_TEST); // to enable hidden surface removal
      glEnable(GL_TEXTURE_2D);
      // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); // DANGER : DO NOT UNCOMMENT. 
      glRotatef(rotate_y, 0.0f, 1.0f, 0.0f);
      glRotatef(rotate_x, 1.0f, 0.0f, 0.0f);
      
      glPushMatrix();
         
         glBindTexture(GL_TEXTURE_2D, texName[2]);
         glBegin(GL_QUADS);
         glNormal3f(0,0,1);//left front
         glTexCoord2f(1.0, 0.0); glVertex3f(-room_w-40, -room_h, room_d);
         glTexCoord2f(0.0, 0.0); glVertex3f(-room_w/3, -room_h, room_d);
         glTexCoord2f(0.0, 1.0); glVertex3f(-room_w/3, room_h, room_d);
         glTexCoord2f(1.0, 1.0); glVertex3f(-room_w-40, room_h, room_d);
         glEnd();

         glBegin(GL_QUADS);
         glNormal3f(0,0,-1);//right front
         glTexCoord2f(0.0, 1.0); glVertex3f(room_w+40, -room_h, room_d);
         glTexCoord2f(1.0, 1.0); glVertex3f(room_w/3, -room_h, room_d);
         glTexCoord2f(1.0, 0.0); glVertex3f(room_w/3, room_h, room_d);
         glTexCoord2f(0.0, 0.0); glVertex3f(room_w+40, room_h, room_d);
         glEnd();

         glBindTexture(GL_TEXTURE_2D, texName[0]);
         glBegin(GL_QUADS);
         glNormal3f(0,0,-1);//door
         glTexCoord2f(1.0, 0.0); glVertex3f(door-room_w/3, -room_h, room_d-4);
         glTexCoord2f(0.0, 0.0); glVertex3f(door+room_w/3, -room_h, room_d-4);
         glTexCoord2f(0.0, 1.0); glVertex3f(door+room_w/3, room_h, room_d-4);
         glTexCoord2f(1.0, 1.0); glVertex3f(door-room_w/3, room_h, room_d-4);
         glEnd();

      glPopMatrix();

   glPopMatrix();

   glFlush();
   glDisable(GL_TEXTURE_2D);
}


void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.0001, 30000.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, -3.6);
}

void animate()
{
    camz=camz-0.1;
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(1920, 1024);
   glutInitWindowPosition(0, 0);
   glutCreateWindow("Dungeon Door");
   init();
   if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
      return false;
   scratch = Mix_LoadMUS( "beat.wav" );
   if( Mix_PlayingMusic() == 0 )
       if( Mix_PlayMusic( scratch, -1 ) == -1 )
           return 1;
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutIdleFunc(animate);
   glutMainLoop();
   return 0; 
}

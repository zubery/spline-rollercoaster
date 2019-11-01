/*
  Ryan Zubery
  CSCI 480
  Assignment 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "pic.h"

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

//window width and height
float width = 640.0; 
float height = 480.0; 

/* total number of splines */
int g_iNumOfSplines;

Pic * groundData;
Pic * skyData;

GLuint gTexName;
GLuint sTexName;

/* represents one control point along the spline */
struct point {
   double x;
   double y;
   double z;
};

/* spline struct which contains how many control points, and an array of control points */
struct spline {
   int numControlPoints;
   struct point *points;
};

/* the spline array */
struct spline *g_Splines;

point catmullRom(double u, struct point p1, struct point p2, struct point p3,
  struct point p4)
{
  point crPoint;

  double x;
  double y; 
  double z;

  double s = 0.5;
  double uCubed = u * u * u;
  double uSquared = u * u;

  double coefficientAndBasis[4] = 
    {(uCubed * -s) + (uSquared * 2 * s) + (u * -s) + (0),
      (uCubed * (2 - s)) + (uSquared * (s - 3)) + (0) + (1), 
      (uCubed * (s - 2)) + (uSquared * (3 - 2 * s)) + (u * s) + (0),
      (uCubed * s) + (uSquared * -s) + (0) + (0)};

  double cb1 = coefficientAndBasis[0];
  double cb2 = coefficientAndBasis[1];
  double cb3 = coefficientAndBasis[2];
  double cb4 = coefficientAndBasis[3];

  double pOfU[3] = 
  {(cb1 * p1.x) + (cb2 * p2.x) + (cb3 * p3.x) + (cb4 * p4.x), 
    (cb1 * p1.y) + (cb2 * p2.y) + (cb3 * p3.y) + (cb4 * p4.y), 
    (cb1 * p1.z) + (cb2 * p2.z) + (cb3 * p3.z) + (cb4 * p4.z)};

  crPoint.x = pOfU[0];
  crPoint.y = pOfU[1];
  crPoint.z = pOfU[2];

  return crPoint;
}

void initTexture()
{
  groundData = jpeg_read("ground.jpg", NULL);

  glGenTextures(1, &gTexName);
  glBindTexture(GL_TEXTURE_2D, gTexName);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, groundData->pix);

  skyData = jpeg_read("sky.jpg", NULL);

  glGenTextures(1, &sTexName);
  glBindTexture(GL_TEXTURE_2D, sTexName);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, skyData->pix);
}

void myinit()
{
  /* setup gl view here */
  glClearColor(0.0, 0.0, 0.0, 0.0);

  //enable depth buffering
  glEnable(GL_DEPTH_TEST); 
}

void display()
{
    //clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

  //clear matrix
  glLoadIdentity(); 

  //look at matrix
  gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  glScalef(g_vLandScale[0], g_vLandScale[1], g_vLandScale[2]); 

  glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2]); 

  glRotatef(g_vLandRotate[0], 1.0, 0.0, 0.0); 
  glRotatef(g_vLandRotate[1], 0.0, 1.0, 0.0); 
  glRotatef(g_vLandRotate[2], 0.0, 0.0, 1.0); 

  //GROUND
  glBindTexture(GL_TEXTURE_2D, gTexName);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glEnable(GL_TEXTURE_2D);

  glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex3d(50.0, 0.0, 50.0);
    glTexCoord2d(0.0, 1.0); glVertex3d(50.0, 0.0, -50.0);
    glTexCoord2d(1.0, 0.0); glVertex3d(-50.0, 0.0, -50.0);
    glTexCoord2d(1.0, 1.0); glVertex3d(-50.0, 0.0, 50.0);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  //GROUND

  //SKY
  glBindTexture(GL_TEXTURE_2D, sTexName);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glEnable(GL_TEXTURE_2D);

  //right side
  glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex3d(50.0, 50.0, 50.0);
    glTexCoord2d(0.0, 1.0); glVertex3d(50.0, 50.0, -50.0);
    glTexCoord2d(1.0, 0.0); glVertex3d(50.0, 0.0, -50.0);
    glTexCoord2d(1.0, 1.0); glVertex3d(50.0, 0.0, 50.0);
  glEnd();

  //left side
  glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex3d(-50.0, 50.0, 50.0);
    glTexCoord2d(0.0, 1.0); glVertex3d(-50.0, 50.0, -50.0);
    glTexCoord2d(1.0, 0.0); glVertex3d(-50.0, 0.0, -50.0);
    glTexCoord2d(1.0, 1.0); glVertex3d(-50.0, 0.0, 50.0);
  glEnd();

  //front side
  glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex3d(50.0, 0.0, 50.0);
    glTexCoord2d(0.0, 1.0); glVertex3d(50.0, 50.0, 50.0);
    glTexCoord2d(1.0, 0.0); glVertex3d(-50.0, 50.0, 50.0);
    glTexCoord2d(1.0, 1.0); glVertex3d(-50.0, 0.0, 50.0);
  glEnd();

  //back side
  glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex3d(50.0, 0.0, -50.0);
    glTexCoord2d(0.0, 1.0); glVertex3d(50.0, 50.0, -50.0);
    glTexCoord2d(1.0, 0.0); glVertex3d(-50.0, 50.0, -50.0);
    glTexCoord2d(1.0, 1.0); glVertex3d(-50.0, 0.0, -50.0);
  glEnd();

  //top size
  glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex3d(50.0, 50.0, 50.0);
    glTexCoord2d(0.0, 1.0); glVertex3d(50.0, 50.0, -50.0);
    glTexCoord2d(1.0, 0.0); glVertex3d(-50.0, 50.0, -50.0);
    glTexCoord2d(1.0, 1.0); glVertex3d(-50.0, 50.0, 50.0);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  //Sky

  glBegin(GL_POLYGON);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.5, 0.0, 0.0);
        glVertex3f(0.5, 0.5, 0.0);
        glVertex3f(0.0, 0.5, 0.0);
    glEnd();


  for(int i = 0; i < g_iNumOfSplines; i++)
  {
    glLineWidth(10.0f);
    glBegin(GL_LINE_STRIP);

    for(int j = 1; j < g_Splines[i].numControlPoints - 2; j++)
    {
      struct point p1 = g_Splines[i].points[j - 1];
      struct point p2 = g_Splines[i].points[j];
      struct point p3 = g_Splines[i].points[j + 1];
      struct point p4 = g_Splines[i].points[j + 2];

      for(double u = 0; u <= 1.0; u += 0.01)
      {
        struct point mPoint = catmullRom(u, p1, p2, p3, p4);
        glVertex3d(mPoint.x, mPoint.y, mPoint.z);
      }
    }
    glEnd();
  }

  //swap buffers
  glutSwapBuffers(); 
}

void reshape(int w, int h)
{
  //set up image size
  glViewport(0, 0, width * 2, height * 2);

  //projection related changes, sets field of view to 60 degrees
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();  
  gluPerspective(60.0, width/height, 0.5, 300.0); 

  //modelview related changes
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); 
}

void doIdle()
{
  /* make the screen update */
  glutPostRedisplay();
}

/* converts mouse drags into information about 
rotation/translation/scaling */
void mousedrag(int x, int y)
{
  int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};
  
  switch (g_ControlState)
  {
    case TRANSLATE:  
      if (g_iLeftMouseButton)
      {
        g_vLandTranslate[0] += vMouseDelta[0]*0.01;
        g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
      }
      break;
    case ROTATE:
      if (g_iLeftMouseButton)
      {
        g_vLandRotate[0] += vMouseDelta[1];
        g_vLandRotate[1] += vMouseDelta[0];
      }
      break;
    case SCALE:
      if (g_iLeftMouseButton)
      {
        g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
        g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
      }
      break;
  }

  glutPostRedisplay(); 
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      g_iLeftMouseButton = (state==GLUT_DOWN);
      break;
    /*case GLUT_RIGHT_BUTTON:
      g_iRightMouseButton = (state==GLUT_DOWN);
      break;*/
  }

  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void keyboard(unsigned char key, int x, int y)
{
  //points on 'a'
  if(key == 'a')
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); 
    glutPostRedisplay(); 
  }
  //wireframe on 's'
  else if(key == 's')
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
    glutPostRedisplay(); 
  }
  //filled triangles on 'd'
  else if(key == 'd')
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
    glutPostRedisplay(); 
  }

  //button controls for mac because CTRL and ALT don't work
  //q to toggle scale, e for rotate, w for translate (then use mouse)
  if(key == 'q')
  {
     g_ControlState = SCALE; 
  }
  else if(key == 'w')
  {
    g_ControlState = ROTATE;
  }
  else if(key == 'e')
  {
    g_ControlState = TRANSLATE;
  }
}

int loadSplines(char *argv) {
  char *cName = (char *)malloc(128 * sizeof(char));
  FILE *fileList;
  FILE *fileSpline;
  int iType, i = 0, j, iLength;


  /* load the track file */
  fileList = fopen(argv, "r");
  if (fileList == NULL) {
    printf ("can't open file\n");
    exit(1);
  }
  
  /* stores the number of splines in a global variable */
  fscanf(fileList, "%d", &g_iNumOfSplines);

  g_Splines = (struct spline *)malloc(g_iNumOfSplines * sizeof(struct spline));

  /* reads through the spline files */
  for (j = 0; j < g_iNumOfSplines; j++) {
    i = 0;
    fscanf(fileList, "%s", cName);
    fileSpline = fopen(cName, "r");

    if (fileSpline == NULL) {
      printf ("can't open file\n");
      exit(1);
    }

    /* gets length for spline file */
    fscanf(fileSpline, "%d %d", &iLength, &iType);

    /* allocate memory for all the points */
    g_Splines[j].points = (struct point *)malloc(iLength * sizeof(struct point));
    g_Splines[j].numControlPoints = iLength;

    /* saves the data to the struct */
    while (fscanf(fileSpline, "%lf %lf %lf", 
	   &g_Splines[j].points[i].x, 
	   &g_Splines[j].points[i].y, 
	   &g_Splines[j].points[i].z) != EOF) {
       std::cout << g_Splines[j].points[i].x << ", " << g_Splines[j].points[i].y << ", " << g_Splines[j].points[i].z << std::endl; 
      i++;
    }
  }

  free(cName);

  return 0;
}


int main (int argc, char ** argv)
{
  if (argc<2)
  {  
  printf ("usage: %s <trackfile>\n", argv[0]);
  exit(0);
  }

  loadSplines(argv[1]);

  glutInit(&argc,argv);

  //creates a window that's double buffered and with depth testing
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
  glutInitWindowPosition(0, 0); 
  glutInitWindowSize(width, height); 
  glutCreateWindow("Ryan's Assignment 2"); 

  initTexture();

  /* tells glut to use a particular display function to redraw */
  glutDisplayFunc(display);

  //tells glut to use a particular reshape function
  glutReshapeFunc(reshape); 

  /* replace with any animate code */
  glutIdleFunc(doIdle);

  /* callback for mouse drags */
  glutMotionFunc(mousedrag);
  /* callback for idle mouse movement */
  glutPassiveMotionFunc(mouseidle);
  /* callback for mouse button changes */
  glutMouseFunc(mousebutton);
  //callback for keyboard input
  glutKeyboardFunc(keyboard);

  /* do initialization */
  myinit();

  glutMainLoop();

  return 0;
}

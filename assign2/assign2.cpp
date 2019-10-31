/*
  Ryan Zubery
  CSCI 480
  Assignment 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "pic.h"

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

/* total number of splines */
int g_iNumOfSplines;

//window width and height
float width = 640.0; 
float height = 480.0; 

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
  gluLookAt(0.0, 0.0, 175.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  //swap buffers
  glutSwapBuffers(); 
}

void reshape(int w, int h)
{
  //set up image size
  glViewport(0, 0, width, height);

  //projection related changes, sets field of view to 60 degrees
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();  
  gluPerspective(60.0, width/height, 0.5, 300.0); 

  //modelview related changes
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); 
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

  /* tells glut to use a particular display function to redraw */
  glutDisplayFunc(display);

  //tells glut to use a particular reshape function
  glutReshapeFunc(reshape); 

  /* do initialization */
  myinit();

  glutMainLoop();

  return 0;
}

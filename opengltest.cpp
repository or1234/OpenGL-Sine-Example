#include <math.h>
#include <stdio.h>
#include <ctime>
#include <stdlib.h>

#include "GL/freeglut.h"
#include "GL/gl.h"

#include "common.h"
#include "plot.h"
#include "plot_op.h"
#include "coord.h"
//width and height of window
int w = 800;
int h = 800;

//debugging file path / printing
const char * dbgpath = "gldump";
FILE * dbgdump;
char sprintstr[200];
void dbgprint(const char * msg)
{
	#ifdef DEBUG
		fputs(msg, dbgdump);
		fflush(dbgdump);
	#endif
}
void dbgprint(char * msg)
{
	#ifdef DEBUG
		fputs(msg, dbgdump);
	#endif
}

//forward glut scheduler to the rest of the application
void doTimerFunc(int delay, void(*f)(int), int arg)
{
	glutTimerFunc(delay, f, arg);
}
//forward glut redrawing requesting to the rest of the application
void redraw()
{
	glutPostRedisplay();
}
//plot information
const float PI = 3.14159;

coord * * sinplots;

//load the coordinates for all of the plots that can be drawn
void init_plot()
{
	dbgprint("starting init plot!\n");
	//sinplots is an array of pointers to all the plots
	sinplots = new coord*[plot_count];
	for(int i = 0; i < plot_count; ++i)
	{
		sinplots[i] = new coord[plot_size];
		for(int x = 0; x < plot_size; ++x)
		{
			//make sure to convert to radians or else the graph won't look correct
			float actual = x * (PI / 48);
			sinplots[i][x].setpolar(sin(actual * (i + 1)), actual);
		}
	}
	dbgprint("finished sin generation\n");
	//initalize all the plots
	initialize_plots();	
	dbgprint("finished with generation!\n");
}
void cleanup()
{
	//delete all the plots
	finalize();
	//delete all the backing graphs
	for(int i = 0; i < plot_count; ++i)
		delete[] sinplots[i];
	delete[] sinplots;
	dbgprint("end session\n");
	fclose(dbgdump);
}
//turn 2d drawing on
void glEnable2D()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, w,h, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}
//draw to the screen
void renderFunction()
{
	//clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//load the identity matrix in
	glLoadIdentity();
	//turn 2d drawing on
	glEnable2D();
	//the graph is white
	glColor3f(1.0,1.0,1.0);
	//for every plot
	for(int x = 0; x < PLOT_COUNT; ++x)
	{
		//begin drawing a strip line
		glBegin(GL_LINE_STRIP);
		//retrieve the coordinate list to display
		coord * display = plots[x]->get_display_plot();
		//for every coordinate in the list
		for(int i = 0; i < plot_size; ++i)
		{
			//draw the coordinate
			display[i].draw();
		}
		//end drawing
		glEnd();
	}
	//flush the drawn output to the physical screen
	glFlush();
}
//when the screen resizes
void resize(int width, int height)
{
	//resize could have been called due to a bug / glitch don't do anything if this is the case
	if(width == w && height == h)
		return;
	//change the viewport to the new width and height
	glViewport(0,0,width, height);
	//load the new width and height into the global width and height variables
	w = width;
	h = height;
	//for every plot
	for(int i = 0; i < PLOT_COUNT; ++i)
		//orient it so it looks normal on the new screen
		plots[i]->orient();
}
//entry point
int main(int argc, char** argv)
{
	//no custom command line arguments
	//initialize the random number generator
	srand((unsigned)time(0));
	//open the debug file for writing and truncate it
	dbgdump = fopen(dbgpath, "w");
	dbgprint("new session\n");
	//make sure the cleanup function is called when the application exits
	atexit(cleanup);
	//initialize glut
	glutInit(&argc, argv);
	dbgprint("initialized\n");
	glutInitWindowSize(w, h);
	glutInitWindowPosition(100,100);
	glutCreateWindow(" ");
	glutDisplayFunc(renderFunction);
	glutReshapeFunc(resize);
	dbgprint("setup finished\n");
	init_plot();
	dbgprint("finished!\n");
	glutMainLoop();
	return 0;
}

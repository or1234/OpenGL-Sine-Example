//#include "GL/freeglut.h"
//#include "GL/gl.h"

#include "plot.h"
#include "plot_op.h"
#include "coord.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
char buf[200];
const int plot_size = 196;
const int plot_count = 100;
//how many plots there are
const int PLOT_COUNT = 1;
//minimum size of a plot
const int MIN_PLOT_SIZE = 96;
const int MAX_SCALE = 700;
const int MIN_SCALE = 100;
const int START_SCALE = 400;
//array of pointers to plots 
plot **plots;
//constructor takes the index of where the plot is in the array, the graphs, the plot size, and passes itself to the plot operation it contains so they can manipulate the plot later on
plot::plot(const coord * original, int plot_size, int _index) : org(original), size(plot_size), x_prop(0), y_prop(0), x(0), y(0), scl(1), index(_index), started(false), scaler(this), transformer(this), scaler_transformer(this)
{
	//make sure the size given is valid
	if(size <= MIN_PLOT_SIZE)
		size = MIN_PLOT_SIZE;
	display = new coord[size];
	//load the display graph from the original
	refresh();
}

//cleanup
plot::~plot()
{
	delete[] display;
}

//change the origin of the graph
void plot::set_center(float _x_prop, float _y_prop)
{
	x_prop = _x_prop;
	y_prop = _y_prop;
	//re-orient the graph
	orient();

}
void plot::orient()
{
	//get the x coordinates from the proportions using the width and height
	float new_x = x_prop * w;
	float new_y = y_prop * h;
	//get the distance from the old x to the new x
	float dist_x = new_x - x;
	float dist_y = new_y - y;
	//for every coordinate
	for(int i = 0; i < size; ++i)
	{
		//move the coordinate by that distance
		display[i].translate(dist_x, dist_y);
	}
	//change the x and y actual coordinates
	x = new_x;
	y = new_y;
}

//change the scale of the graph
void plot::set_scale(int _scl)
{
	//if we aren't changing the scale don't do anything
	if(scl == _scl)
	{
		return;
	}
	//for every coordinate
	for(int i = 0; i < size; ++i)
	{
		//get what the coordinate would be in the old scale WITHOUT any other manipulations and the same for the new scale as well
		float oldx = org[i].getx() * scl;
		float oldy = org[i].gety() * scl;
		float newx = org[i].getx() * _scl;
		float newy = org[i].gety() * _scl;
		//move the dislplay by the distance between these coordinates
		display[i].translate(newx - oldx, newy - oldy);
	}
	//change the scale
	scl = _scl;
}

float plot::get_scale()
{
	return scl;
}

int plot::get_size()
{
	return size;
}

float plot::get_x_prop()
{
	return x_prop;
}

float plot::get_y_prop()
{
	return y_prop;
}

const coord * plot::get_original_plot()
{
	return org;
}

void plot::set_original(const coord * original)
{
	org = original;
	//we must refresh every time the original is changed
	refresh();
}

coord * plot::get_display_plot()
{
	return display;
}

//this reloads the graph from the original plot and changes the origin / scale to match the plot
//neccessary for the transform operation because minor innacuracies on the transformations compound and fuck up the graph
void plot::refresh()
{
	//for every coordinate reload it from the original
	for(int i = 0; i < size; ++i)
		display[i].setcartesian(org[i].getx(), org[i].gety());
	//set the old center to (0,0) because that is what it was from the original graph
	x = 0;
	y = 0;
	//orient so the display graph goes to the correct position
	orient();
	//save the current scale
	int fscl = scl;
	//set the current scale to 1 because we reloaded from the original
	scl = 1;
	//change the scale back to the current scale
	set_scale(fscl);
}

//regenerate the operator for the plot
void plot::regen_op()
{
	dbgprint("regen\n");
	//get a random operator and delay
	op = plot_operators[rand() % PLOT_OP_COUNT];
	delay = DELAY[rand() % DELAY_COUNT];
	dbgprint("init\n");
	//run the intializer
	op.init(this);
	//schedule the current plot so the operation can be run
	schedule(index);
}

//schedule the operator to run
void plot::schedule(int index)
{
	//dbgprint("scheduler run\n");
	//get the plot the index reffered to
	plot * self = plots[index];
	//run the operation and save the return code
	bool cont = self->op.op(self);
	//redraw the screen so changes made by the operator are visually reflected
	redraw();
	//true means keep going, false means complete
	if(cont)
	{
		//not done we have to reschedule the operation to run
		doTimerFunc(self->delay, plot::schedule, index);
	}
	else
	{
		dbgprint("finished with operation!\n");
		//regenerate the operator
		//note that because there is no tail recursion there is a stack overflow potential here if the operator keeps finishing if it finishes after one call for a period of time
		self->regen_op();
	}
}

//notify plot that it has been initalized and it can start scheduling
void plot::notify_started()
{
	//if we haven't already been started
	if(!started)
	{
		//save that we have been started and generate an operator
		dbgprint("notify start\n");
		started = true;
		regen_op();
	}
}

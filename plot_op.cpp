#include "plot.h"
#include "plot_op.h"
#include "common.h"
#include "coord.h"
#include <stdlib.h>
#include <stdio.h>
//buffer for debugging
char buffer[200];

//scale constructor just sets parent - initialzier actually loads it when the time comes
scale::scale(plot * _parent) : parent(_parent), targetscale(0), inc(0)
{
}

//scale the plot
void scale::scale_plot()
{
	//retrieve the current scale
	int currentscale = parent->get_scale();
	//if it is less than the target
	if(currentscale < targetscale)
	{
		//increase the scale
		currentscale += inc;
	}
	else
	{
		//if the scale is greater than the target
		if(currentscale > targetscale)
		{
			//decrease the scale
			currentscale -= inc;
		}
	}
	
	//change the scale
	parent->set_scale(currentscale);

}
//scheduled scale operation function
bool scaleplot(plot * target)
{
	//forward the call to scale_plot of the target's scale operation
	target->scaler.scale_plot();
	//are the scales the same?
	if(target->get_scale() == target->scaler.targetscale)
	{
		//yes - we are done scaling
		return false;
	}
	//no - keep scaling
	return true;
}

//scheduled scale initalizer - picks scales to imitate a sine wave
void initscale(plot * target)
{
	dbgprint("init scale!\n");
	scale & scl = target->scaler;
	//if we started off with the max scale
	if(target->get_scale() == MAX_SCALE)
	{
		//go down to the minimum scale
		scl.targetscale = MIN_SCALE;
	}
	else
	{
		//go up to the maximum scale if we aren't at the bottom of the "wave"
		scl.targetscale = MAX_SCALE;
	}
	int dist = abs(target->get_scale() - scl.targetscale);
	for(int i = 0; i < 5; ++i)
	{
		int check = (rand() % 6) + 1;
		if(dist % check == 0)
		{
			scl.inc = check;
			return;
		}
	}
	scl.inc = 1;
}

//transform constructor just fills in the parent and clears the members
transform::transform(plot * _parent) : parent(_parent), org(0),  dst(0), scl(0), count(0), times(0), slope_l(new slope[plot_size])
{
}

//destructor has to free the slope list
transform::~transform()
{
	//a null vector delete doesn't do anything but this is just good practice
	if(slope_l != 0)
		delete[] slope_l;
}

//load the slope in to the slope list
void transform::load_slope()
{
	//get the size of the parent plot
	int size = parent->get_size();
	//for every coordinate in the plot
	for(int i = 0; i < size; ++i)
	{
		//run is the difference between the x vals and rise is the difference between the y vals
		slope_l[i].run = dst[i].getx() - org[i].getx();
		slope_l[i].rise = dst[i].gety() - org[i].gety();
	}
}

//change the destination graph of the transform operation
void transform::change_dst(const coord * new_dst)
{
	dbgprint("in change dst!\n");
	if(dst != 0)
		org = dst;
	dst = new_dst;
	load_slope();
}

void transform::turn()
{
	//retrieve the display graph
	coord * display = parent->get_display_plot();
	//get the size
	int size = parent->get_size();
	//for every coord in the graph
	for(int i = 0; i < size; ++i)
	{
		//translate it by the slope times the given scale
		display[i].translate(slope_l[i].run * scl, slope_l[i].rise * scl);
	}
}

//scheduled transformer operator
bool transformplot(plot * target)
{
	//if we had gone through the specified number of times
	if(target->transformer.count == target->transformer.times)
	{
		//we have fully changed the display graph - change the original graph,  and exit
		target->set_original(target->transformer.dst);
		return false;
	}
	//no scale on the turn - just a normal turn
	target->transformer.turn();
	//increment the counter
	++target->transformer.count;
	return true;
}
//scheduled transformer initialzier
void inittransform(plot * target)
{
	dbgprint("in init transform!\n");
	//initalize the slope list if it does not exist
	//the transformer hasn't been executed at all - count of 0
	target->transformer.count = 0;
	//figure out a random time factor - larger numbers being faster and smaller numbers being slower
	target->transformer.scl = (rand() % 5) + 1 ;
	//target->transformer.scl = 1;
	//it has to get to the other graph and there is no other manipulation, so it takes however much the scale is to get to the other graph including the extra time
	target->transformer.times = target->get_scale() / target->transformer.scl;
	//target->transformer.times = target->get_scale();
	//store the original plot on the transformer
	target->transformer.org = target->get_original_plot();
	const coord * new_dst = target->transformer.dst;
	const coord * old_dst = target->transformer.dst;
	//if this was the first time nothing was set on the destination - use the original instead
	if(new_dst == 0)
	{
		new_dst = target->transformer.org;
		old_dst = target->transformer.org;
	}
	//get a random new destination with no collision with the old destination
	while(new_dst == old_dst)
		new_dst = sinplots[rand() % plot_count];
	
	target->transformer.change_dst(new_dst);
}

//scale transformer operation constructor just sets the parent and clears members
scale_transform::scale_transform(plot * _parent) : parent(_parent)
{
}

//scheduled scale transform initializer function
void init_scale_transform(plot * target)
{	
	//scale_transform uses both the scaler and transformer on the object
	initscale(target);
	inittransform(target);
	//the number of times the plot needs to go through this operation is the distance between the target and current scales over how much the scale increases on each pass
	target->transformer.times = abs(target->scaler.targetscale - target->get_scale()) / target->scaler.inc;
	//the actual scale of the transformer is whatever the target scale is over the number of times this operation runs
	target->transformer.scl = (float)target->scaler.targetscale / target->transformer.times;
	//although this seems simple looking back, it was a bitch to figure out

}

//scale transform operation function
bool scale_transform_plot(plot * target)
{
//	dbgprint("scale transform!\n");
	//"turn" the display graph using the scale we computed in the intialize function
	target->transformer.turn();
	//see if we are done scaling
	target->scaler.scale_plot();
	//if we are done
	++target->transformer.count;
//	sprintf(sprintstr, "count %d\n", target->transformer.count);
///	dbgprint(sprintstr);
	if(target->transformer.count == target->transformer.times)
	{
		//dbgprint("finished scale transform!\n");
		//we have to do the transformer cleanup here
		target->set_original(target->transformer.dst);
		return false;
	}
	//we aren't done - keep going
	return true;
}

//how many plot operatorators and delays the scheduler can access
const int PLOT_OP_COUNT = 2;
const int DELAY_COUNT = 2;
//array of delays and plot operators
int * DELAY;
plot_oper * plot_operators;

//allocate and initalize the plot operators, delays, and plots
void initialize_plots()
{
	DELAY = new int[DELAY_COUNT];
	DELAY[0] = 20;
	DELAY[1] = 25;
	//DELAY[2] = 40;
	plot_operators = new plot_oper[PLOT_OP_COUNT];
	plot_operators[0].op = transformplot;
	plot_operators[0].init = inittransform;
	plot_operators[1].op = scale_transform_plot;
	plot_operators[1].init = init_scale_transform;
	plots = new plot*[PLOT_COUNT];
	//for every plot that exists
	for(int i = 0; i < PLOT_COUNT; ++i)
	{
		//initalize the plot to a random graph
		plots[i] = new plot(sinplots[rand() % plot_count], plot_size, i);
		//set the center of the graph
		plots[i]->set_center(0.5, 0.5);
		//set the initial scale of the graph
		plots[i]->set_scale(START_SCALE);
		//notify the graph that it can begin scheduling
		plots[i]->notify_started();
	}
}

//cleanup
void finalize()
{
	delete[] DELAY;
	delete[] plot_operators;
	for(int i = 0; i < PLOT_COUNT; ++i)
		delete plots[i];
	delete[] plots;
}

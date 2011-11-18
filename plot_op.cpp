#include "plot.h"
#include "plot_op.h"
#include "common.h"
#include "coord.h"
#include <stdlib.h>
#include <stdio.h>
//buffer for debugging
char buffer[200];

//scale constructor just sets parent - initialzier actually loads it when the time comes
scale::scale(plot * _parent) : parent(_parent), targetscale(0)
{
}

//scale the plot
bool scale::scale_plot()
{
	//retrieve the current scale
	int currentscale = parent->get_scale();
	//if it is less than the target
	if(currentscale < targetscale)
	{
		//increase the scale by 1
		++currentscale;
	}
	else
	{
		//if the scale is greater than the target
		if(currentscale > targetscale)
		{
			//decrease the scale by 1
			--currentscale;
		}
	}
	
	//change the scale
	parent->set_scale(currentscale);

	//if the current scale is the target scale
	if(currentscale == targetscale)
	{
		//we are done 
		return false;
	}
	//scale isn't the same - continue
	return true;

}
//scheduled scale operation function
bool scaleplot(plot * target)
{
	//just forward the call to scale_plot of the target's scale operation
	return target->scaler.scale_plot();
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
}

//transform constructor just fills in the parent and clears the members
transform::transform(plot * _parent) : parent(_parent), org(0),  dst(0),count(0), times(0), slope_l(new slope[plot_size])
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

bool transform::turn(float scl)
{
	//if we had gone through the specified number of times
	if(count == times)
	{
		//we have fully changed the display graph - change the originalgraph,  and exit
		parent->set_original(dst);
		return false;
	}
	//we have gone through an additional time - increment the counter
	++count;
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
	//we are continuing after this
	return true;
}

//scheduled transformer operator
bool transformplot(plot * target)
{
	//forward call to the transformer operation with a scale of 1 - that parameter is used in the compound scale-transform operation
	return target->transformer.turn(1);	
}
//scheduled transformer initialzier
void inittransform(plot * target)
{
	dbgprint("in init transform!\n");
	//initalize the slope list if it does not exist
	//the transformer hasn't been executed at all - count of 0
	target->transformer.count = 0;
	//it has to get to the other graph and there is no other manipulation, so it takes however much the scale is to get to the other graph
	target->transformer.times = target->get_scale();
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
scale_transform::scale_transform(plot * _parent) : parent(_parent), scl(0)
{
}

//scheduled scale transform initializer function
void init_scale_transform(plot * target)
{	
	//scale_transform uses both the scaler and transformer on the object
	initscale(target);
	inittransform(target);
	//disable the transformer's timing mechanism - we decide when to stop running based on scaler only
	target->transformer.times = -1;
	//the scale the transformer needs to apply is what the target scale is over the absolute value of the target scale - the current scale of the plot
	target->scaler_transformer.scl = (float)target->scaler.targetscale / abs(target->scaler.targetscale - target->get_scale());
}

//scale transform operation function
bool scale_transform_plot(plot * target)
{
	//"turn" the display graph using the scale we computed in the intialize function
	target->transformer.turn(target->scaler_transformer.scl);
	//see if we are done scaling
	bool fin = target->scaler.scale_plot();
	//if we are done
	if(!fin)
	{
		//we have to do the transformer cleanup here because we suspended its own counter
		target->set_original(target->transformer.dst);
	}
	//return whether or not we were finished from the scaler's decision
	return fin;
}

//how many plot operatorators and delays the scheduler can access
const int PLOT_OP_COUNT = 2;
const int DELAY_COUNT = 3;
//array of delays and plot operators
int * DELAY;
plot_oper * plot_operators;

//allocate and initalize the plot operators, delays, and plots
void initialize_plots()
{
	DELAY = new int[DELAY_COUNT];
	DELAY[0] = 20;
	DELAY[1] = 30;
	DELAY[2] = 40;
	plot_operators = new plot_oper[PLOT_OP_COUNT];
	//plot_operators[1].op = scaleplot;
	//plot_operators[1].init = initscale;
	plot_operators[0].op = scale_transform_plot;
	plot_operators[0].init = init_scale_transform;
	plot_operators[1].op = transformplot;
	plot_operators[1].init = inittransform;
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

#ifndef PLOT_HEADER
#define PLOT_HEADER
#include "plot_op.h"
#include "common.h"
//we can have pointers to coordinates
class coord;
class plot
{
	private:
		//size of plot
		int size;
		//original graph
		const coord * org;
		//dispay graph
		coord * display;
		//proportion of x center - against width
		float x_prop;
		//proportion of y center - against height
		float y_prop;
		//acutal x coordinate
		float x;
		//actual y coordinate
		float y;
		//scale of plot
		int scl;
		//whether or not the scheduling process has been started
		bool started;
		//delay in between plot operation calls
		int delay;
		//plot's index in the plot array
		const int index;
		//changes plot operator
		void regen_op();
		//current plot operator
		plot_oper op;
		//schedule a plot operator to run
		static void schedule(int index);
	public:
		//constructor
		plot(const coord * original, int plot_size, int _index);
		//destructor
		~plot();
		//change original graph
		void set_original(const coord * original);
		//change center of plot
		void set_center(float _x_prop, float _y_prop);
		//re-orient plot around center
		void orient();
		//set scale of plot
		void set_scale(int _scl);
		//retrieve scale of plot
		float get_scale();
		//get size of plot
		int get_size();
		//get x proportion
		float get_x_prop();
		//get y proportion
		float get_y_prop();
		//retreive display graph
		coord * get_display_plot();
		//get original graph
		const coord * get_original_plot();
		//restore original graph and current scale and center
		void refresh();
		//plot operations
		scale scaler;
		transform transformer;
		scale_transform scaler_transformer;
		//notify scheduling can start
		void notify_started();
};
//array of plots the scheduler can access
extern plot ** plots;
#endif

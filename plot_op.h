#ifndef PLOT_OP_HEADER
#define PLOT_OP_HEADER
//forsward declare plot and coord and scale_transform so we can take pointers to them
class plot;
class coord;
class scale_transform;
//scaler makes plots smaller or larger
class scale
{
	private:
		//target scale we are trying to reach
		int targetscale;
		//parent plot of scale operation
		plot * parent;
		//"constructor" of scale
		void init();
		//scale the plot
		bool scale_plot();
		public:	
		//constructor
		scale(plot * _parent);
	//the initalizer and operation functions that are scheduled can access private members
	friend bool scaleplot(plot * target);
	friend void initscale(plot * target);
	friend void init_scale_transform(plot * target);
	friend bool scale_transform_plot(plot * target);
};
//transformer changes what graph a plot is based off of
class transform
{
	private:
		//graph we are going to
		const coord * dst;
		//original graph of the plot
		const coord * org;
		//structure to hold slope values
		struct slope
		{
			float rise;
			float run;
		};
		//slope between all the points on the destination and original graphs
		slope * slope_l;
		//parent plot of transform operation
		plot * parent;
		//how many times the transform operation has been done
		int count;
		//how many times to do the transform operation
		int times;
		//"turn" a plot's display graph to the destination graph
		bool turn(float scl);
		//change the destination of the transformer operation
		void change_dst(const coord * new_dst);		//load the slope between destination and original graphs
		void load_slope();
	public:
		//constructor
		transform(plot * parent);
		//destructor
		~transform();
	//the operation and initalizer functions have access to the class's internal workings
	friend bool transformplot(plot * target);
	friend void inittransform(plot * target);
	friend void init_scale_transform(plot * target);
	friend bool scale_transform_plot(plot * target);

};

//compound scale / transform operation
class scale_transform
{
	private:
		//scale the transform operation needs to apply to its "turn" operation
		float scl;
		//parent plot of scale_transform operation
		plot * parent;
	public:
		//constructor
		scale_transform(plot * parent);
	//scheduled initalizer / operation functions get access to the class
	friend bool scale_transform_plot(plot * target);
	friend void init_scale_transform(plot * target);

};

//how many operators exist
extern const int PLOT_OP_COUNT;
//how many possible delays exist
extern const int DELAY_COUNT;
//array to store the delays
extern int * DELAY;
//pointer to an operator
//returns true if it needs to continue or false when it finishes - it gets passed the plot it is working on
typedef bool(*plot_op)(plot * target);
//pointer to an operator initializer - gets passed the plot it works on
typedef void(*plot_op_init)(plot * target);
//plot operator consists of a initalizer and operation function to be scheduled
struct plot_oper
{
	plot_op op;
	plot_op_init init;
};
//array of all operators
extern plot_oper * plot_operators;
//"constructor" and "destructor" for the global plot variables
void initialize_plots();
void finalize();
#endif

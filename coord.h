#ifndef COORD_HEADER
#define COORD_HEADER
class coord
{
	//all the members are logically constant when the point is BUT we avoid loading polar / cartesian unless we absolutely have to hence the mutable qualifiers
	private:
		//have we updated the cartesian coords since the last polar assignment?
		mutable bool outdatedcartesian;
		//have we updated the polar coords since last cartesian assignment?
		mutable bool outdatedpolar;
		//x axis coord
		mutable float x;
		//y axis coord
		mutable float y;
		//r
		mutable float r;
		//theta
		mutable float theta;
		//load the polar coordinates from the cartesian ones
		void loadpolar() const;
		//load the cartesian coordiantes from the polar ones
		void loadcartesian() const;
	public:
		//assignment operator
		coord& operator=(const coord& other);
		//default constructor
		coord();
		//change the cartesian coordinates
		void setcartesian(float _x, float _y);
		//change the polar coordiantes
		void setpolar(float r, float theta);
		//translate the coordiante in the cartesian coordinate system
		void translate(float horizontal, float vertical);
		//get methods
		float getx() const;
		float gety() const;
		float getr() const;
		float gettheta() const;
		//draw the coordinate
		void draw() const;
};
#endif

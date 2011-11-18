#include "coord.h"
#include <math.h>
#include "GL/gl.h"

/*
	rather than compute the other coordinate system for every change, it is merely marked as outdated
	we only compute the other unless it is absolutely necessary
*/

coord::coord() : x(0), y(0)
{

}

coord& coord::operator=(const coord& other)
{
	x = other.x;
	y = other.y;
	return *this;
}

//load cartesian coordinates from the polar ones
void coord::loadcartesian() const
{
	//do the math to convert
	x = r * cos(theta);
	y = r * sin(theta);
	//indicate that the cartesian coords are no longer outdated
	outdatedcartesian = false;
}
//load polar coordinates from the cartesian ones
void coord::loadpolar() const
{
	//conversion
	r = sqrt(pow(y, 2) + pow(x, 2));
	theta = atan2(y, x);
	//indicate polar coords aren't outdated anymore
	outdatedpolar = false;
}
//set cartesian coords
void coord::setcartesian(float _x, float _y)
{
	x = _x;
	y = _y;
	//indicate carteisan is updated and polar is out
	outdatedpolar = true;
	outdatedcartesian = false;
}
//set polar coords
void coord::setpolar(float _r, float _theta)
{
	r = _r;
	theta = _theta;
	//polar coords are updated but cartesian aren't
	outdatedpolar = false;
	outdatedcartesian = true;
}

//translate the coordiante in the cartesian system
void coord::translate(float horizontal, float vertical)
{
	//make sure our cartesian coords are valid
	if(outdatedcartesian)
		loadcartesian();
	x += horizontal;
	y += vertical;
	//polar is now outdated
	outdatedpolar = true;
}

//draw on the screen
void coord::draw() const
{
	//make sure we have the right coords
	if(outdatedcartesian)
		loadcartesian();
	glVertex2f(x,y);
}

//all getters check to see if their coord systems are valid before retrieving
float coord::getx() const
{
	if(outdatedcartesian)
		loadcartesian();
	return x;
}

float coord::gety() const
{
	if(outdatedcartesian)
		loadcartesian();
	return y;
}

float coord::getr() const
{
	if(outdatedpolar)
		loadpolar();
	return r;
}

float coord::gettheta() const
{
	if(outdatedpolar)
		loadpolar();
	return theta;
}

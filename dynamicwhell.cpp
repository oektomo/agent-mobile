/**
 * dynamicwhell.cpp
 *
 *  Created on: Jan 24, 2016
 *      Author: otm
 *
 * @file	dynamicwhell.cpp
 * @author	otm
 * @brief	dynamic of the 2 wheeled robot, will include function for odometry also
 */
#include <math.h>

#include "dynamicwhell.hpp"

/**	@brief	Input are speed, and theta in radian
 *
 * @return	wheel[2] speed
 * @bug		need to calibrate the wheel to digital!
 */
void speed2wheel(float* speed, float theta, float* wheel)
{
	wheel[0] = speed[0]*cos(theta) - RWIDE*speed[1]*sin(theta);
	wheel[1] = speed[0]*sin(theta) + RWIDE*speed[1]*cos(theta);
}

/**
 * @brief Computing from wheel encoder to position, input position[2] in radian
 * 			wheelenc[0] = right encoder, wheelenc[1] = left encoder
 * 			position[3]
 * @return updating position[3] state
 *
 * @bug		still implementing
 */
void wheel2position(int* wheelenc, float* position)
{
	float deltaPosition[3];

	deltaPosition[X] = (wheelenc[1]+wheelenc[0]) * cos(position[2]);
	deltaPosition[Y] = (wheelenc[1]+wheelenc[0]) * sin(position[2]);
	deltaPosition[2] = 1/RWIDE*wheelenc[R] - 1/RWIDE*wheelenc[L];

	position[X] = deltaPosition[X];
	position[Y] = deltaPosition[Y];
	position[2] = deltaPosition[2];
}

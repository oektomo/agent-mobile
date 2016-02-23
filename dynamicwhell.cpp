/**
 * dynamicwhell.cpp
 *
 *  Created on: Jan 24, 2016
 *      Author: otm
 *
 * @file	dynamicwhell.cpp
 * @author	otm
 * @brief	dynamic of the 2 wheeled robot, already include function for odometry also
 * @bug		don't know yet
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
 * @brief Computing from wheel (speed) encoder to position, input position[2] in radian
 * 			wheelenc[0] = right encoder, wheelenc[1] = left encoder
 * 			position[3]
 * 			\f$
 * 			\par
 * 			\begin{align}
 * 			\begin{bmatrix}
 * 				\dot{x} \\ \dot{y} \\ \dot{\theta}
 * 			\end{bmatrix} &=&
 * 			\begin{bmatrix}
 * 				-sin(\theta) & 0 \\
 * 				cos(\theta)  & 0 \\
 * 				0			 & 1
 * 			\end{bmatrix}
 * 			\begin{bmatrix}
 * 				v \\ \omega
 * 			\end{bmatrix}	\\
 *
 * 			v &=& \frac{v_R+v_L}{2} = \frac{\omega_R r_R + \omega_L r_L}{l} \\
 * 			\omega &=& \frac{v_R-v_L}{2} = \frac{\omega_R r_R - \omega_L r_L}{l} \\
 *
 * 			\left[ {\begin{array}{*{20}c}
 * 			 	\dot{x} \\ \dot{y} \\ \dot{\theta} \\
 * 			\end{array} } \right] &=&
 * 			\left[ {\begin{array}{*{20}c}
 * 				\frac{r_R}{2}cos(\theta) & \frac{r_L}{2}cos(\theta)\\
 * 				\frac{r_R}{2}sin(\theta) & \frac{r_L}{2}sin(\theta)\\
 * 				-\frac{r_R}{l} & \frac{r_L}{l}\\
 * 			\end{array}} \right] .
 *
 * 			\begin{bmatrix}
 * 				u_R \\ u_L
 * 			\end{bmatrix}
 * 			\end{align}
 * 			\f$
 * @tparam wheelenc variable storing wheel encoder counter
 *
 * @return wheelenc updating position[3] state
 *
 *
 * @bug		already implemented, optimizing and callibrate!, create latex mathematic
 */
void wheel2position(int* wheelenc, float* position)
{
	float deltaPosition[3];
	float wheelencf[2];
	wheelencf[L] = (float)wheelenc[L] * WHEEL_STEP_COEF;
	wheelencf[R] = (float)wheelenc[R] * WHEEL_STEP_COEF;

	deltaPosition[X] = (wheelencf[1]+wheelencf[0])/2 * cos(position[2]);
	deltaPosition[Y] = (wheelencf[1]+wheelencf[0])/2 * sin(position[2]);
	deltaPosition[2] = wheelencf[L]/RWIDE - wheelencf[R]/RWIDE;

	position[X] = deltaPosition[X]/2 + position[X];
	position[Y] = deltaPosition[Y]/2 + position[Y];
	position[2] = deltaPosition[2] + position[2];
}

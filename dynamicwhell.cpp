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
#include <stdio.h>

#include "dynamicwhell.hpp"

/**	@brief	Input are speed, and theta in radian. convert holonomic to non-holonomic
 *
 * @return	wheel[2] speed
 * @bug		need to calibrate the wheel to digital!
 */
void speed2wheel(double* speed, double theta, double* wheel)
{
	//theta = theta*1.5;
/*
	double euclidxy = sqrt(speed[0]*speed[0] + speed[1]*speed[1]);
	wheel[L] = 1/WHEEL_RAD * cos(theta) * speed[X] + RWIDE * sin(theta) /(2*WHEEL_RAD*euclidxy) * speed[X];
	wheel[L] = wheel[L] + sin(theta)/WHEEL_RAD * speed[Y] - RWIDE * cos(theta) /(2*WHEEL_RAD*euclidxy) * speed[Y];
	
	wheel[R] = 1/WHEEL_RAD * cos(theta) * speed[X] - RWIDE * sin(theta) /(2*WHEEL_RAD*euclidxy) * speed[X];
	wheel[R] = wheel[R] + sin(theta)/WHEEL_RAD * speed[Y] + RWIDE * cos(theta) /(2*WHEEL_RAD*euclidxy) * speed[Y];
*/	
	wheel[L] = 1/WHEEL_RAD * (cos(theta) * speed[X] + sin(theta) * speed[X] /2) ;
	wheel[L] = wheel[L] + 1/WHEEL_RAD * (sin(theta) * speed[Y] - cos(theta) * speed[Y] /2);
	
	wheel[R] = 1/WHEEL_RAD * (cos(theta) * speed[X] - sin(theta) * speed[X] /2) ;
	wheel[R] = wheel[R] + 1/WHEEL_RAD * (sin(theta) * speed[Y] + cos(theta) * speed[Y] /2);
/*
	wheel[0] = speed[X]*cos(theta) - RWIDE*speed[Y]*sin(theta);
	wheel[1] = speed[X]*sin(theta) + RWIDE*speed[Y]*cos(theta);
	cos(theta) -sin(theta)
	sin(theta) cos(theta)
*/
}

int saturate(int* dataToSaturate, int saturationValue)
{
	if (*dataToSaturate > saturationValue) *dataToSaturate = saturationValue;
	if (*dataToSaturate < -saturationValue) *dataToSaturate = -saturationValue;
	int sign = (dataToSaturate > 0) ? 1 : ((dataToSaturate < 0) ? -1 : 0);
	return sign;
}

double addingBacklash(double src)
{
	if(src > 0)
		src = src + ADD_P;
	else
		src = src - ADD_P;
	return src;
}

/*
	need to refine the constanta SCALE_P
	maybe try adding to remove the lower part PWM didn't move the motor
	input as a float, output as a int
*/
void saturateKeepVector(double* src, int* dst, int* sign, int saturationValue)
{
//	src[L] = src[L]*SCALE_P;
//	src[R] = src[R]*SCALE_P;
//	src[L] = src[L]+ADD_P;
//	src[R] = src[R]+ADD_P;
	dst[L] = src[L];
	dst[R] = src[R];
	sign[L] = (dst[L] > 0) ? 1 : ((dst[L] < 0) ? -1 : 0);
	sign[R] = (dst[R] > 0) ? 1 : ((dst[R] < 0) ? -1 : 0);
	src[L] = fabs(src[L]);
	src[R] = fabs(src[R]);
	src[L] = src[L] + ADD_P;
	src[R] = src[R] + ADD_P;
	if(src[L] < src[R]) {
		if(src[R] > saturationValue) {
		// abs(src[L]) < abs(src[R])
		src[L] = saturationValue*src[L]/src[R];
		dst[L] = sign[L]*src[L];
		dst[R] = sign[R]*saturationValue;
		} else {
		dst[L] = sign[L]*src[L];
		dst[R] = sign[R]*src[R];
		}
	} else {
		if(src[L] > saturationValue) {
		// abs(src[R]) < abs(src[L])
		src[R] = saturationValue*src[R]/src[L];
		dst[R] = sign[R]*src[R];
		dst[L] = sign[L]*saturationValue;
		} else {
		dst[L] = sign[L]*src[L];
		dst[R] = sign[R]*src[R];
		}
	}
}

double** invmatrix2d(double x, double y, double theta)
{
        double **out=new double*[3];
        for(int i = 0;i<3;i++)
                out[i]=new double[3];

        double out2[3];
        out[0][0]=cos(theta);
        out[0][1]=sin(theta);
        out[0][2]=-sin(theta)*y-(cos(theta)*x);
        out[1][0]=-out[0][1];
        out[1][1]=out[0][0];
        out[1][2]=-(cos(theta)*y-sin(theta)*x);
        out[2][0]=0;
        out[2][1]=0;
        out[2][2]=1;
        return out;
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
void wheel2position(int* wheelenc, double* position)
{
	float deltaPosition[3];
	float wheelencf[2];
	wheelencf[L] = (float)wheelenc[L] * WHEEL_STEP_COEF;
	wheelencf[R] = (float)wheelenc[R] * WHEEL_STEP_COEF;

	deltaPosition[X] = (wheelencf[1]+wheelencf[0])/2 * cos(position[2]);
	deltaPosition[Y] = (wheelencf[1]+wheelencf[0])/2 * sin(position[2]);
	deltaPosition[2] = wheelencf[R]/RWIDE - wheelencf[L]/RWIDE;

	position[X] = deltaPosition[X]/2 + position[X];
	position[Y] = deltaPosition[Y]/2 + position[Y];
	position[2] = deltaPosition[2] + position[2];
}


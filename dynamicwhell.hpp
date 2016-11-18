/*
 * dynamicwhell.hpp
 *
 *  Created on: Jan 24, 2016
 *      Author: otm
 */

#ifndef DYNAMICWHELL_HPP_
#define DYNAMICWHELL_HPP_

//#define WHEEL_RAD 3.39
#define WHEEL_RAD 2.0
//#define RWIDE	14.5
#define RWIDE	11.8
//#define WHEEL_STEP_COEF	2.13 // 4wd rover
//#define WHEEL_STEP_COEF 7.0		// miniQ Agent4
#define WHEEL_STEP_COEF 2.0		// miniQ Agent2
#define R		0
#define L		1
#define X		0
#define Y		1
#define SCALE_P 15
//#define ADD_P 30
#define ADD_P 10
void speed2wheel(double* speed, double theta, double* wheel);
void wheel2position(int* wheelenc, double* position);
int saturate(int *dataToSaturate, int saturationValue);
void saturateKeepVector(double* src, int* dst, int* sign, int saturationValue);
#endif /* DYNAMICWHELL_HPP_ */

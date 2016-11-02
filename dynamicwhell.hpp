/*
 * dynamicwhell.hpp
 *
 *  Created on: Jan 24, 2016
 *      Author: otm
 */

#ifndef DYNAMICWHELL_HPP_
#define DYNAMICWHELL_HPP_

#define WHEEL_RAD 3.39
#define RWIDE	14.5
#define WHEEL_STEP_COEF	2.13
#define R		0
#define L		1
#define X		0
#define Y		1
#define SCALE_P 15
#define ADD_P 70
void speed2wheel(double* speed, double theta, double* wheel);
void wheel2position(int* wheelenc, double* position);
int saturate(int *dataToSaturate, int saturationValue);
void saturateKeepVector(double* src, int* dst, int* sign, int saturationValue);
#endif /* DYNAMICWHELL_HPP_ */

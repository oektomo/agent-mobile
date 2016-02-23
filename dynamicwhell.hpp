/*
 * dynamicwhell.hpp
 *
 *  Created on: Jan 24, 2016
 *      Author: otm
 */

#ifndef DYNAMICWHELL_HPP_
#define DYNAMICWHELL_HPP_

#define RWIDE	14.5
#define WHEEL_STEP_COEF	2.13
#define R		0
#define L		1
#define X		0
#define Y		1
void speed2wheel(float* speed, float theta, float* wheel);
void wheel2position(int* wheelenc, float* position);

#endif /* DYNAMICWHELL_HPP_ */

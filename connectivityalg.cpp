/*
 * connectivityalg.cpp
 *
 *  Created on: Jan 25, 2016
 *      Author: otm
 *
 *  @file	connectivityalg.cpp
 *
 *  @brief	algorithm for connectivity
 */
#include "connectivityalg.hpp"

/**
 * @brief single agent control law navigation function
 *
 * @return control input u[2]
 *
 * @bug		normalize c1, c2, agent speed (pi)
 */
void navigation(float* qi	/// agent position
				,float* qr	/// agent reference
				,float* pi	/// agent speed
				,float* pr	/// agent refrence speed
				,float* u	/// return control input
		)
{
	float c1 = 10, c2 = 10;

	u[0] = -c1*(qi[0] - qr[0]) - c2*(pi[0] - pr[0]);
	u[1] = -c1*(qi[1] - qr[1]) - c2*(pi[1] - pr[1]);
}


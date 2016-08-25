/*
 * mainserial2.cpp
 *
 *  Created on: Feb 2, 2016
 *      Author: otm
 */


#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <math.h>

#include "mainserial2.hpp"
#include "dynamicwhell.hpp"
#include "serial3.hpp"

#define MESSAGESENT1 "S-100&-100E"

int main(int argc, char* argv[])
{
// TODO: use library to parse argument 
	if(argc != 2 && argc!=3){
		printf("%d argument inserted \n", argc-1);
		printf("first argument serial portname\n");
		printf("second argument to stop motor\n");
		return(1);
	} else if (argc == 3){
		int fd = open_port(argv[1]);
		if(fd < 0){
			printf("failed to open serial port\n");
			return(-1);
		}
		char sentstring[20];
		int sizesent = strlen("S0&0E");
		strncpy(sentstring, "S0&0E", sizesent);
		sentstring[sizesent] = '\0';
		write_port(fd, sentstring, sizesent+1);
		return(0);
	}
// Opening port
	int fd = open_port(argv[1]);
	if(fd < 0){
		printf("failed to open serial port\n");
		return(-1);
	}

// writing to serialport
	char sentstring[20];
	strncpy(sentstring, "S-254&254E\0", strlen("S-254&254E\0"));
	int readed = 0;
	char msg[BUFFER_SIZE];	
	double position[3], positionKMin1[3];
	position[X] = 0;
	position[Y] = 0;
	position[2] = 0;
	int pipefd[2];
	int result = pipe(pipefd);
// create pipe	result = fcntl(*pipefd, F_SETFL, (fcntl(*pipefd, F_GETFL, 0) | O_NONBLOCK));
	if(result < 0){
		printf("set pipe nonblocking failed\n");
	}
	int read_process = fork();
	if(read_process == 0) {
/************** child process for receiving data from serial		 */
		if(close(pipefd[0]) == -1) printf("failed to close read end\n");
		while(1) { 
			readparseChar(fd, pipefd); 
		}
		return 0;

	} else if(read_process < 0){
		printf("making child failed\n");
	} else {
		printf("child pid %d\n", read_process);
	}
	FILE* filestate = fopen("mainserial3.txt", "w");
	if(filestate == NULL){
		printf("open file, failed\n");
	}
	fclose(filestate);
// INITIAL CONDITIon
	if(close(pipefd[1]) == -1) printf("failed to close write end\n");
	double targetPosition[2], obstacle1[2], controlInput[2], controlInputWheel[2], Vx, Vy;
	double rObstacle1;
	double attractiveControl[2], obsFunc0, goalFunc0, NowJ, delta_x, delta_y, deltaJx, deltaJy, Ax, Ay, kv = 8000, kf = 1250000;
	targetPosition[0] = 80;
	targetPosition[1] = 40;
	obstacle1[X] = 30;
	obstacle1[Y] = 20;
	rObstacle1 = 5;
	controlInput[0] = 12;
	controlInput[1] = 0;
	int signL = 0, signR =0 ;
	while(1){
		filestate = fopen("mainserial3.txt","a");
		// convert from float to string
		positionKMin1[X] = position[X];
		positionKMin1[Y] = position[Y];
		// convert from position to wheel using invert of SE2 matrix
		speed2wheel(controlInput, position[2], controlInputWheel);
		int controlL = controlInputWheel[L]*SCALE_P;
		if (controlL > 255) controlL = 255;
		if (controlL < -255) controlL = -255;
		signL = (controlL > 0) ? 1 : ((controlL < 0) ? -1 : 0);
		int controlR = controlInputWheel[R]*SCALE_P;
		if (controlR > 255) controlR = 255;
		if (controlR < -255) controlR = -255;
		signR = (controlR > 0) ? 1 : ((controlR < 0) ? -1 : 0);
		snprintf(sentstring, sizeof(sentstring), "S%d&%dE", controlL, controlR);
		int sizeSent = strlen(sentstring);
		int stateReceived[8];
		result=1;
		for(int haha=0; haha<3; haha++){
			result = read(pipefd[0], stateReceived, STATE_AMOUNT*sizeof(int));
			if(result > 0){
				int wheelenc[2];
				wheelenc[L] = signL * stateReceived[1];
				wheelenc[R] = signR * stateReceived[2];
				wheel2position(wheelenc, position);
				// CONTROL ALGORITHM
				attractiveControl[X] = -(position[X] - targetPosition[X]);
				attractiveControl[Y] = -(position[Y] - targetPosition[Y]);
				//controlInput[X] = -(position[X] - targetPosition[X]);
				//controlInput[Y] = -(position[Y] - targetPosition[Y]);
				
				//function O=obstaclefunction(x,w1)
				//O= w1 * (exp(-0.1 * ((x(1,1)-20)^2 + (x(2,1)-20)^2)));
				obsFunc0 = W1 * exp( -0.1 * (pow((position[X] - obstacle1[X]),2) + pow((position[Y] - obstacle1[Y]),2)));
				goalFunc0 = (pow(attractiveControl[X],2) + pow(attractiveControl[Y],2) ) * W2;
				NowJ = obsFunc0 + goalFunc0;

				delta_x = position[X] - positionKMin1[X];
				delta_y = position[Y] - positionKMin1[Y];
				if(delta_x == 0.0) delta_x = 0.01;
				if(delta_y == 0.0) delta_y = 0.01;

				deltaJx = (pow((position[X]-targetPosition[X]+delta_x), 2) + pow((position[Y] - targetPosition[Y]), 2) ) * W2 +
				 W1 * exp( -0.1 * (pow((position[X] - obstacle1[X] + delta_x), 2) + pow((position[Y] - obstacle1[Y]), 2))) - NowJ;
				deltaJy = (pow((position[X]-targetPosition[X]), 2) + pow((position[Y] - targetPosition[Y] + delta_y), 2) ) * W2 +
				 W1 * exp( -0.1 * (pow((position[X] - obstacle1[X]), 2) + pow((position[Y] - obstacle1[Y] + delta_y), 2))) - NowJ;

				Ax = deltaJx / delta_x;
				Ay = deltaJy / delta_y;

				Vx = delta_x / stateReceived[0];
				Vy = delta_y / stateReceived[0];

				//controlInput[X] = - kv * Vx - kf*Ax;
				//controlInput[Y] = - kv * Vy - kf*Ay;
				controlInput[X] = - kf*Ax;
				controlInput[Y] = - kf*Ay;

				// END OF CONTROL ALGORITHM
				fprintf(filestate,"%f %f %f %d %d %d %d %f %f \n\r", position[X], position[Y], position[2], wheelenc[L], wheelenc[R], controlL, controlR, Ax, Ay);
				printf("%f %f %f %d %d %f %f \n\r", 
					position[X], position[Y], position[2], 
					wheelenc[L], wheelenc[R], controlInput[X], controlInput[Y]);
				result = result / sizeof(int); 
			} else {
				printf("pipe no reading\n");
				break;
			}
		}
		write_port(fd, sentstring, sizeSent+1);
		fclose(filestate);
	}

}

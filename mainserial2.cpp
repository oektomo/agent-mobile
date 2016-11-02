/*
 * mainserial2.cpp
 *
 *  Created on: Feb 2, 2016
 *      Author: otm
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <math.h>
#include <libconfig.h>
#include <sys/stat.h>

#include "tools.h"
#include "mainserial2.hpp"
#include "dynamicwhell.hpp"
#include "serial3.hpp"
#include "iicsource/iic_hmc3_lib.h"

#define MESSAGESENT1 "S-100&-100E"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_GREEN	"\x1b[32m"
#define ANSI_COLOR_RED	"\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

int main(int argc, char* argv[])
{
// TODO: use library to parse argument, refining......
// TODO: use cmake and cmakelist for easier library management 
// configuration and reading argument.
	int cgetopt;
	config_t cfg, *cf;
	cf = &cfg;
	config_init(cf);
	uint8_t cflag = 0, pflag = 0, sflag = 0;

	char* serialPort1 = NULL;
	int serialPortfd;
	char serialPortdefault[] = "/dev/ttyUSB0";
	while((cgetopt = getopt (argc, argv, "hc:p:s:")) != -1)
	switch (cgetopt) {
	// stopping motor
	case 's': {
		printf("stopping motor\n");
		serialPortfd = open_port(optarg);
		if(serialPortfd < 0){
			printf("failed to open serial port\n");
			return(-1);
		}
		char sentstring[20];
		int sizesent = strlen("S0&0E");
		strncpy(sentstring, "S0&0E", sizesent);
		sentstring[sizesent] = '\0';
		write_port(serialPortfd, sentstring, sizesent+1);
		close(serialPortfd);
		return(0);
	} break;
	// set port if not default one
	case 'p': {
		pflag = 1;
		serialPortfd = open_port(optarg);
		if(serialPortfd < 0){
			printf("failed to open serial port\n");
			return(-1);
		} else printf("serial port opened\n");
	} break;
	// set config file if not default one
	case 'c':
		cflag=1;
		if(!config_read_file(cf, optarg)) {
			fprintf(stderr, "%s:%d - %s\n",
			config_error_file(cf),
			config_error_line(cf),
			config_error_text(cf));
			config_destroy(cf);
			printf("%s not found or can't be open\n", optarg);
			return(EXIT_FAILURE);
		}
	break;
	// help
	case 'h':
		printf("-s to stop motor\n");
		printf("-p to set serial port\n");
		printf("-c to set config file\n");
		return(0);
	break;
	// to service option without argument
	case '?':
	if(optopt == 's') {
		printf("using default port to stop\n");
		serialPortfd = open_port(serialPortdefault);
		char sentstring[20];
		int sizesent = strlen("S0&0E");
		strncpy(sentstring, "S0&0E", sizesent);
		sentstring[sizesent] = '\0';
		write_port(serialPortfd, sentstring, sizesent+1);
		close(serialPortfd);
		return(0);
	} else if(optopt == 'p') {
		pflag = 1;
		printf("using default port\n");
		serialPortfd = open_port(serialPortdefault);
		if(serialPortfd < 0){
			printf(ANSI_COLOR_RED "[ERROR] failed to open serial port\n" ANSI_COLOR_RESET);
			return(-1);
		} else printf(ANSI_COLOR_GREEN "[INFO] serial port opened\n" ANSI_COLOR_RESET);
	} else if (optopt == 'c') {
		cflag = 1;
		if(!config_read_file(cf, "config.txt")) {
			fprintf(stderr, "%s:%d - %s\n",
			 config_error_file(cf),
			 config_error_line(cf),
			 config_error_text(cf));
			config_destroy(cf);
			printf("config.txt not found or can't be open\n");
			return(EXIT_FAILURE);
		}
	}
	}
	if(!pflag)
		serialPortfd = open_port(serialPortdefault);
	if(!cflag)
		if(!config_read_file(cf, "config.txt")) {
			fprintf(stderr, "%s:%d - %s\n",
			 config_error_file(cf),
			 config_error_line(cf),
			 config_error_text(cf));
			config_destroy(cf);
			printf("config.txt not found or can't be open\n");
			return(EXIT_FAILURE);
		}
		
// TODO : use libconfig to save constant and some configuration. and then start using make to compile

// writing to serialport
	char sentstring[20];
	//strncpy(sentstring, "S-254&254E\0", strlen("S-254&254E\0"));
	int readed = 0;
	char msg[BUFFER_SIZE];	
	int pipefd[2];
	printf(ANSI_COLOR_YELLOW"[INFO] creating pipe\n"ANSI_COLOR_RESET);
	int result = pipe(pipefd);
// create pipe	result = fcntl(*pipefd, F_SETFL, (fcntl(*pipefd, F_GETFL, 0) | O_NONBLOCK));
	if(result < 0){
		printf("set pipe nonblocking failed\n");
	}
	int read_process = fork();
	if(read_process == 0) {
/************** child process for receiving data from serial		 */
		printf("this is child\n");
		umask(0);
		pid_t  sid = setsid();
		if(sid < 0) printf("setsid failed\n");
		else printf("setsid done?\n");
		//close( STDIN_FILENO );
		//close( STDOUT_FILENO );
		int fd = open("/dev/null",O_RDWR, 0);

		if (fd != -1) {
			dup2 (fd, STDIN_FILENO);
			dup2 (fd, STDOUT_FILENO);
			dup2 (fd, STDERR_FILENO);

			if (fd > 2) {
				close (fd);
			}
		}
		if(close(pipefd[0]) == -1) printf("failed to close read end\n");
		while(1) {
		printf("waiting readparseChar()\n");
			readparseChar(serialPortfd, pipefd); 
		}
		return 0;

	} else if(read_process < 0){
		printf("making child failed\n");
	} else {
		printf("child pid %d\n", read_process);
	}
/************** parent process **********************/
// opening file for logging data, also to delete data from previous experiment
	printf("after creating child with pid %d\n",read_process);
	FILE* filestate = fopen("mainserial3.txt", "w");
	if(filestate == NULL){
		printf("open file, failed\n");
	}
	fclose(filestate);
	if(close(pipefd[1]) == -1) printf("failed to close write end\n");
// INITIAL CONDITIon
	double targetPosition[2], obstacle1[2], obsRad = 5; // position
	double rObstacle1 = 5;
	double controlInput[2], controlInputWheel[2]; // variable for state
	//int signL = 0, signR = 0;
	int sign[2], wheelInput[2], control[2];
	// temporary variable for ALGORITHM calculation
	double Vx, Vy;
	double attractiveControl[2], obsFunc0, goalFunc0, NowJ, delta_x, delta_y, deltaJx, deltaJy, Ax, Ay;
	sign[L] = 0; sign[R] = 0;
	double kv = 8, kf = 50;
	double position[3], positionKMin1[3];
	position[X] = 0;
	position[Y] = 0;
	position[2] = 0;
	targetPosition[0] = 80;
	targetPosition[1] = 40;
	obstacle1[X] = 30;
	obstacle1[Y] = 0;
	controlInput[0] = 12;
	controlInput[1] = 0;
	int compassfd = initializeCompass();
	double initAngle = getHeading(compassfd);
// end of INITIAL CONDITIon
// setting, so the esc keystroke will quit the program
	int n, quit = 0;
	unsigned char key;
	struct termios initial_settings;
	tcgetattr(0, &initial_settings);
	unblock_getchar( initial_settings );
	printf(ANSI_COLOR_YELLOW "[INFO] Stoping the print\n" ANSI_COLOR_RESET);
// opening file before starting simulation
	filestate = fopen("mainserial3.txt","a");
	printf(ANSI_COLOR_YELLOW "[INFO] Just Assumed file opened\n" ANSI_COLOR_RESET);
	while(!quit) {
		n = getchar();
		while(n != EOF) {
			key = n;
			/* do something useful here with key */
			printf(ANSI_COLOR_RED "To EXIT please press ESC\n\r"ANSI_COLOR_RESET );
			/* Escape key pressed */
			if(key == 27) {
				quit = 1;
				fprintf(filestate,"\n\n");
				fclose(filestate);
				break;
			}
			n = getchar();
		}
		// convert from float to string
		// convert from position to wheel using invert of SE2 matrix
		speed2wheel(controlInput, position[2], controlInputWheel);
		/*
		int controlL = controlInputWheel[L]*SCALE_P;
		signL = saturate(&controlL, 255);
		int controlR = controlInputWheel[R]*SCALE_P;
		signR = saturate(&controlR, 255);
		*/
		saturateKeepVector(controlInputWheel, control, sign, 255);

		snprintf(sentstring, sizeof(sentstring), "S%d&%dE", control[L], control[R]);
		int sizeSent = strlen(sentstring);
		int stateReceived[8];
		result=1;
		printf(ANSI_COLOR_YELLOW "[INFO] waiting for state received\n" ANSI_COLOR_RESET);
		for(int repeat=0; repeat<1; repeat++) {
			result = read(pipefd[0], stateReceived, STATE_AMOUNT*sizeof(int));
			if(result > 0){
			// get wheelEncoder counter and calculate position
				int wheelenc[2];
				wheelenc[L] = sign[L] * stateReceived[1];
				wheelenc[R] = sign[R] * stateReceived[2];
				wheel2position(wheelenc, position);

				position[2] = getHeading(compassfd);
				if(position[2]<initAngle) {position[2] = 2*M_PI - (initAngle-position[2]);}
				else {position[2] = position[2] - initAngle;}
				if(position[2] < M_PI)
					position[2] = -position[2];
				if(position[2] > M_PI)
					position[2] = (2*M_PI) - position[2];

// CONTROL ALGORITHM
				attractiveControl[X] = -(position[X] - targetPosition[X]);
				attractiveControl[Y] = -(position[Y] - targetPosition[Y]);
				//controlInput[X] = -(position[X] - targetPosition[X]);
				//controlInput[Y] = -(position[Y] - targetPosition[Y]);
				
				//function O=obstaclefunction(x,w1)
				//O= w1 * (exp(-0.1 * ((x(1,1)-20)^2 + (x(2,1)-20)^2)));
				obsFunc0 = W1 * exp( -0.01 * (pow((position[X] - obstacle1[X]),2) + pow((position[Y] - obstacle1[Y]),2))/obsRad);
				goalFunc0 = (pow(attractiveControl[X],2) + pow(attractiveControl[Y],2) ) * W2;
				NowJ = obsFunc0 + goalFunc0;
				//NowJ = goalFunc0;

				delta_x = position[X] - positionKMin1[X];
				delta_y = position[Y] - positionKMin1[Y];
				positionKMin1[X] = position[X];
				positionKMin1[Y] = position[Y];
				if(delta_x == 0.0) delta_x = 0.01;
				if(delta_y == 0.0) delta_y = 0.01;

				// with obstacle
				deltaJx = (pow((position[X]-targetPosition[X]+delta_x), 2) + pow((position[Y] - targetPosition[Y]), 2) ) * W2 +
				 W1 * exp( -0.1 * (pow((position[X] - obstacle1[X] + delta_x), 2) + pow((position[Y] - obstacle1[Y]), 2))) - NowJ;
				deltaJy = (pow((position[X]-targetPosition[X]), 2) + pow((position[Y] - targetPosition[Y] + delta_y), 2) ) * W2 +
				 W1 * exp( -0.1 * (pow((position[X] - obstacle1[X]), 2) + pow((position[Y] - obstacle1[Y] + delta_y), 2))) - NowJ;
/*
				// without obstacle
				deltaJx = (pow((position[X]-targetPosition[X]+delta_x), 2) + pow((position[Y] - targetPosition[Y]), 2) ) * W2 +
				 W1 * exp( -0.1 * (pow((position[X] + delta_x), 2) + pow((position[Y]), 2))) - NowJ;
				deltaJy = (pow((position[X]-targetPosition[X]), 2) + pow((position[Y] - targetPosition[Y] + delta_y), 2) ) * W2 +
				 W1 * exp( -0.1 * (pow((position[X]), 2) + pow((position[Y] + delta_y), 2))) - NowJ;
*/
				Ax = deltaJx / delta_x;
				Ay = deltaJy / delta_y;

				Vx = delta_x / stateReceived[0];
				Vy = delta_y / stateReceived[0];

				controlInput[X] = - kv * Vx - kf*Ax;
				controlInput[Y] = - kv * Vy - kf*Ay;
				//controlInput[X] = - kf*Ax;
				//controlInput[Y] = - kf*Ay;

// END OF CONTROL ALGORITHM
				fprintf(filestate,"%f %f %f %d %d %d %d %f %f %f %f\n\r", position[X], position[Y], position[2], wheelenc[L], wheelenc[R], control[L], control[R], Ax, Ay, obsFunc0, NowJ);
				printf("%f %f %f %d %d %f %f \n\r", 
					position[X], position[Y], position[2], 
					wheelenc[L], wheelenc[R], controlInput[X], controlInput[Y]);
				result = result / sizeof(int); 
			} else {
				printf("pipe no reading\n");
				break;
			}
		}
		write_port(serialPortfd, sentstring, sizeSent+1);
	}
	snprintf(sentstring, sizeof(sentstring), "S%d&%dE", 0, 0);
	write_port(serialPortfd, sentstring, 6);
	// reset Terminal setting
	tcsetattr(0, TCSANOW, &initial_settings);
}


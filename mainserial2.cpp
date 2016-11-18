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
#include <sys/socket.h>

#include "tools.h"
#include "mainserial2.hpp"
#include "dynamicwhell.hpp"
#include "serial3.hpp"
#include "iicsource/iic_hmc3_lib.h"
#include "network/network.hpp"
#include "libconfig/agentconfig.h"

#define MESSAGESENT1 "S-100&-100E"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_GREEN	"\x1b[32m"
#define ANSI_COLOR_RED	"\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

//#define SINGLE_CONSENSUS

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
		if(!config_read_file(cf, "config.cfg")) {
			fprintf(stderr, "%s:%d - %s\n",
			 config_error_file(cf),
			 config_error_line(cf),
			 config_error_text(cf));
			config_destroy(cf);
			printf("config.cfg not found or can't be open\n");
			return(EXIT_FAILURE);
		}
	}
	}
	// default, if no appropriate argument feed to program
	if(!pflag)
		serialPortfd = open_port(serialPortdefault);
	if(!cflag)
		if(!config_read_file(cf, "config.cfg")) {
			fprintf(stderr, "%s:%d - %s\n",
			 config_error_file(cf),
			 config_error_line(cf),
			 config_error_text(cf));
			config_destroy(cf);
			printf("using default config.cfg but not found or can't be open\n");
			return(EXIT_FAILURE);
		}
// get data from config file still unsafe
	const char *agentNumber=NULL;
	config_lookup_string(cf, "agent_num", &agentNumber);
	int agentNum = atoi(agentNumber);

#define ARRAY_CONSENSUS
#ifdef ARRAY_CONSENSUS
	const char *ipAgent1=NULL;
	if( config_lookup_string(cf, "ipAgent1", &ipAgent1) )
		printf("IP Agent1 %s\n", ipAgent1);
	else printf("IP Agent1 not available\n");
	const char *ipAgent2=NULL;
	if( config_lookup_string(cf, "ipAgent2", &ipAgent2) )
		printf("IP Agent2 %s\n", ipAgent2);
	else printf("IP Agent2 not available\n");
	const char *ipAgent3=NULL;
	if( config_lookup_string(cf, "ipAgent3", &ipAgent3) )
		printf("IP Agent3 %s\n", ipAgent3);
	else printf("IP Agent3 not available\n");
	const char *ipAgent4=NULL;
	if( config_lookup_string(cf, "ipAgent4", &ipAgent4) )
		printf("IP Agent4 %s\n", ipAgent4);
	else printf("IP Agent4 not available\n");
// reading 2 dimension array.
	int8_t A_array[25];
	arraycontainTypedef arrayContainer;
	arrayContainer.Array = A_array;
	read2DimArray(cf, &arrayContainer, "Adjacency");
	printMatrix(&arrayContainer);
#endif // #ifdef ARRAY_CONSENSUS

	const char *posX=NULL;
	config_lookup_string(cf, "posX", &posX);
	double posXdouble = atof((char*) posX);
	const char *posY=NULL;
	config_lookup_string(cf, "posY", &posY);
	double posYdouble = atof((char*) posY);
	printf("agent num: %d position[%f, %f]\n", agentNum, posXdouble, posYdouble);
	
	const char *targetX=NULL;
	config_lookup_string(cf, "targetX", &targetX);
	double targetXdouble = atof((char*) targetX);
	const char *targetY=NULL;
	config_lookup_string(cf, "targetY", &targetY);
	double targetYdouble = atof((char*) targetY);
#ifdef CONSENSUS_OLD
//  preparation for NETWORK and create server if it's ok with config.cfg
	int serverfd = 0, connfd = 0;
	double data[4];
	double Rdata[2];
	if(serverBool) {
		serverfd = create_server();
		printf("server created with %d fd, waiting to accept client\n", serverfd);
		connfd = accept(serverfd, (struct sockaddr*) NULL, NULL);
		printf("a client connect with %d file descriptor\n", connfd);

	} else { // this is as client
		connfd = create_client((char *)ipServer);
		printf("client connect with %d file descriptor\n", connfd);
	}
#endif // #ifndef CONSENSUS_OLD

#ifdef ARRAY_CONSENSUS
// creating network connection listed from array
//  preparation for NETWORK and create server if it's ok with config.cfg
	int serverfd = 0, connfd = 0;
	double data[4];
	double Rdata[2];
	int connfdAgentJ[arrayContainer.A_row];
	printf("arrayContainer.A_row = %d\n", arrayContainer.A_row);
	for(int agentJ = 0; agentJ < arrayContainer.A_row; agentJ++) {
		printf("arrayContainer.Array[agentNum*4+agentJ] = %d \n", arrayContainer.Array[(agentNum-1) *4+agentJ]);

		if( arrayContainer.Array[(agentNum-1) *4+agentJ] == 2 ) { // server
			if( serverfd <= 0) {
				serverfd = create_server();
				printf("server created with %d fd, waiting to accept client\n", serverfd);
			}
			connfd = accept(serverfd, (struct sockaddr*) NULL, NULL);
			printf("a client connect with %d file descriptor\n", connfd);
			int recAgentNum = 0;
			int readed = read(connfd, &recAgentNum, 2); //receiving agent number
			printf(ANSI_COLOR_GREEN"[INFO] receiving agent number %d\n"ANSI_COLOR_RESET, recAgentNum);
			if( recAgentNum < (arrayContainer.A_row+1) ) {
				connfdAgentJ[recAgentNum-1] = connfd;
			} else {
				printf(ANSI_COLOR_RED"[ERROR] agent number too big\n"ANSI_COLOR_RESET);
			}


		} else if( arrayContainer.Array[(agentNum-1) *4+agentJ] == 1 ) { // client
			printf("connecting to ipAgentJ = %d\n", agentJ);
			if(agentJ == 0)
				connfd = create_client((char *)ipAgent1);
			if(agentJ == 1)
				connfd = create_client((char *)ipAgent2);
			if(agentJ == 2)
				connfd = create_client((char *)ipAgent3);
			if(agentJ == 3)
				connfd = create_client((char *)ipAgent4);
			printf("client connect with %d file descriptor\n", connfd);
			printf(ANSI_COLOR_GREEN"sending agent number %d\n"ANSI_COLOR_RESET, agentNum);
			write(connfd, &agentNum, 2);// sending agent number
			connfdAgentJ[agentJ] = connfd;
		}
	}

	printf("connection establish between agent\n");

#endif // #ifdef ARRAY_CONSENSUS
		
// preparation FORKING CHILD FOR SERIAL COMM
	char sentstring[20];// write buffer to serialport
	int readed = 0;
	char msg[BUFFER_SIZE];	// read buffer from serialport
	int pipefd[2];
	printf(ANSI_COLOR_YELLOW"[INFO] creating pipe\n"ANSI_COLOR_RESET);
	int result = pipe(pipefd);
// create pipe	result = fcntl(*pipefd, F_SETFL, (fcntl(*pipefd, F_GETFL, 0) | O_NONBLOCK));
	if(result < 0){
		printf("set pipe nonblocking failed\n");
	}
// FORKING CHILD FOR SERIAL COMM
	int read_process = fork();
	if(read_process == 0) {
/************** CHILD process for receiving data from serial		 */
		printf("this is child, blocking output from child\n");
		blockPrintf();

		if(close(pipefd[0]) == -1) printf("failed to close read end\n");
		sleep(2);
		tcflush(serialPortfd, TCIOFLUSH);

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
	double targetPosition[2], obstacle1[2]; // position
	double rObstacle1 = 10, rAgent = 23;
	double controlInput[2], controlInputWheel[2]; // variable for state
	//int signL = 0, signR = 0;
	int sign[2], wheelInput[2], control[2];
	// temporary variable for ALGORITHM calculation
	double Vx, Vy;
	double attractiveControl[2], obsFunc0 = 0, goalFunc0, NowJ, delta_x, delta_y, deltaJx, deltaJy, Ax, Ay;
	sign[L] = 0; sign[R] = 0;
	double kv = 8, kf = 25, Kr = 10;
	double position[3], positionKMin1[3];
	position[X] = posXdouble;
	position[Y] = posYdouble;
	position[2] = 0;
	targetPosition[0] = targetXdouble;
	targetPosition[1] = targetYdouble;
	obstacle1[X] = 80;
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
				break; // quiting from while(n != EOF) 
			}
			n = getchar();
		}

		int stateReceived[STATE_AMOUNT];
		result=1;
		printf(ANSI_COLOR_YELLOW "[INFO] waiting for state received\n" ANSI_COLOR_RESET);
		
		result = read(pipefd[0], stateReceived, STATE_AMOUNT*sizeof(int));
		if(result > 0) {
			//printf("%d byteReceived: %d %d %d %d \n", result, stateReceived[0], stateReceived[1], stateReceived[2], stateReceived[6], stateReceived[7]);
			//printf("rec_data: S%d&%d&%d&%d&%d&%d&%d&%dE\n", stateReceived[0], stateReceived[1], stateReceived[2], stateReceived[3], stateReceived[4], stateReceived[5], stateReceived[6], stateReceived[7]);
		// get wheelEncoder counter and calculate position
			int wheelenc[2];
			wheelenc[L] = sign[L] * stateReceived[1];
			wheelenc[R] = sign[R] * stateReceived[2];
			wheel2position(wheelenc, position);

			// compass
			position[2] = getHeading(compassfd);
			if(position[2]<initAngle) {position[2] = 2*M_PI - (initAngle-position[2]);}
			else {position[2] = position[2] - initAngle;}
			if(position[2] < M_PI)
				position[2] = -position[2];
			if(position[2] > M_PI)
				position[2] = (2*M_PI) - position[2];

// CONTROL ALGORITHM
//#define SIMPLE_ALGORITHM
#ifndef SIMPLE_ALGORITHM
			attractiveControl[X] = -(position[X] - targetPosition[X]);
			attractiveControl[Y] = -(position[Y] - targetPosition[Y]);
#endif
			
			//function O=obstaclefunction(x,w1)
			//O= w1 * (exp(-0.1 * ((x(1,1)-20)^2 + (x(2,1)-20)^2)));
			goalFunc0 = (pow(attractiveControl[X],2) + pow(attractiveControl[Y],2) ) * W2;
//#define OBSTACLE
#ifdef OBSTACLE
			obsFunc0 = W1 * exp( -0.1 * (pow((position[X] - obstacle1[X]),2) + pow((position[Y] - obstacle1[Y]),2))/rObstacle1);
			NowJ = obsFunc0 + goalFunc0;
#else
			NowJ = goalFunc0;
#endif // #ifdef OBSTACLE
// for compute self state
			delta_x = position[X] - positionKMin1[X];
			delta_y = position[Y] - positionKMin1[Y];
			positionKMin1[X] = position[X];
			positionKMin1[Y] = position[Y];
			// first_time initial so delta_x != 0
			if(delta_x == 0.0) delta_x = 0.01;
			if(delta_y == 0.0) delta_y = 0.01;
			Vx = delta_x / stateReceived[0] * 1000; // dx/dt
			Vy = delta_y / stateReceived[0] * 1000; // dy/dt
#ifdef CONSENSUS_OLD
// data communication and consensus algorithm
			data[X] = position[X];
			data[Y] = position[Y];
			data[2] = Vx;
			data[3] = Vy;
			sentData(connfd, data,32);
			int tempstatus = readSocket(connfd, Rdata, 33);
			if(tempstatus < 0) quit = 1;
			printf(ANSI_COLOR_YELLOW"peer data: %f %f %f %f\n"ANSI_COLOR_RESET, Rdata[X], Rdata[Y], Rdata[2], Rdata[3]);
			double vcx = Rdata[2] - Vx;
			double vcy = Rdata[3] - Vy;
// end of data communication and consensus algorithm
#endif //#ifdef CONSENSUS_OLD

#ifdef ARRAY_CONSENSUS
// send data to every agent
	data[X] = position[X];
	data[Y] = position[Y];
	data[2] = Vx;
	data[3] = Vy;
	double stateAgentJ[STATE_NET* arrayContainer.A_row];
	printf("arrayContainer.A_row = %d\n", arrayContainer.A_row);
	for(int agentJ = 0; agentJ < arrayContainer.A_row; agentJ++) {
		if( arrayContainer.Array[(agentNum-1) *4+agentJ] == 2 ) { // server
			sentData(connfdAgentJ[agentJ], data, 32);
		} else if( arrayContainer.Array[(agentNum-1) *4+agentJ] == 1 ) { // client
			sentData(connfdAgentJ[agentJ], data, 32);
		}
	}

// read data from agentJ
	for(int agentJ = 0; agentJ < arrayContainer.A_row; agentJ++) {
		if( arrayContainer.Array[(agentNum-1) *4+agentJ] == 2 ) { // server
			int tempstatus = readSocket(connfdAgentJ[agentJ], Rdata, 33);
		} else if( arrayContainer.Array[(agentNum-1) *4+agentJ] == 1 ) { // client
			int tempstatus = readSocket(connfdAgentJ[agentJ], Rdata, 33);
		}
		// copy from Rdata to stateAgentJ
		for (int stateJ = 0; stateJ < STATE_NET; stateJ++) {
			stateAgentJ[agentJ*4 +stateJ] = Rdata[stateJ];
		}
	}

// compute consensus data from agentJ
	double jarakAgentKuadrat = 0, xrepel = 0, yrepel = 0, vcx = 0, vcy = 0, xcentroid = 0, ycentroid = 0;
	for(int agentJ = 0; agentJ < arrayContainer.A_row; agentJ++) {
		if( arrayContainer.Array[(agentNum-1) *4+agentJ] != 0 ) { 
// REPEL
		jarakAgentKuadrat = (position[X]-stateAgentJ[agentJ*STATE_NET +X])*(position[X]-stateAgentJ[agentJ*STATE_NET +X]) + (position[Y]-stateAgentJ[agentJ*STATE_NET +Y])*(position[Y]-stateAgentJ[agentJ*STATE_NET +Y]);
		xrepel = xrepel + Kr * exp(-jarakAgentKuadrat/rAgent) * (position[X]-stateAgentJ[agentJ*STATE_NET +X]);
		yrepel = yrepel + Kr * exp(-jarakAgentKuadrat/rAgent) * (position[Y]-stateAgentJ[agentJ*STATE_NET +Y]);
// CONSENSUS_VELOCITY
		vcx = vcx + stateAgentJ[agentJ*STATE_NET +2] - Vx;
		vcy = vcy + stateAgentJ[agentJ*STATE_NET +3] - Vy;
// CENTROID
		xcentroid = xcentroid + stateAgentJ[agentJ*STATE_NET +X];
		ycentroid = ycentroid + stateAgentJ[agentJ*STATE_NET +Y];
		}
	}

	xcentroid = position[X] - (xcentroid + position[X]) / arrayContainer.A_row;
	ycentroid = position[Y] - (ycentroid + position[Y]) / arrayContainer.A_row;

#endif // #ifdef ARRAY_CONSENSUS

#ifdef REPEL
			double jarakAgentKuadrat = (position[X]-Rdata[X])*(position[X]-Rdata[X]) + (position[Y]-Rdata[Y])*(position[Y]-Rdata[Y]);
			double xrepel = Kr * exp(-jarakAgentKuadrat/rAgent) * (position[X]-Rdata[X]);
			double yrepel = Kr * exp(-jarakAgentKuadrat/rAgent) * (position[Y]-Rdata[Y]);
#endif // #ifdef REPEL
#ifdef CENTROID


#endif // #ifdef CENTROID

#ifdef OBSTACLE
			// with obstacle
			deltaJx = (pow((position[X]-targetPosition[X]+delta_x), 2) + pow((position[Y] - targetPosition[Y]), 2) ) * W2
				 + W1 * exp( -0.1 * (pow((position[X] - obstacle1[X] + delta_x), 2) + pow((position[Y] - obstacle1[Y]), 2))) 
- NowJ;
			deltaJy = (pow((position[X]-targetPosition[X]), 2) + pow((position[Y] - targetPosition[Y] + delta_y), 2) ) * W2
				 + W1 * exp( -0.1 * (pow((position[X] - obstacle1[X]), 2) + pow((position[Y] - obstacle1[Y] + delta_y), 2))) 
- NowJ;
#else

			// without obstacle
			deltaJx = (pow((position[X]-targetPosition[X]+delta_x), 2) + pow((position[Y] - targetPosition[Y]), 2) ) * W2 
//			 + W1 * exp( -0.1 * (pow((position[X] + delta_x), 2) + pow((position[Y]), 2))) 
			 - NowJ;
			deltaJy = (pow((position[X]-targetPosition[X]), 2) + pow((position[Y] - targetPosition[Y] + delta_y), 2) ) * W2 
//			 + W1 * exp( -0.1 * (pow((position[X]), 2) + pow((position[Y] + delta_y), 2))) 
			 - NowJ;

#endif
			Ax = deltaJx / delta_x;
			Ay = deltaJy / delta_y;

			//controlInput[X] = - kv * Vx - kf*Ax;
			//controlInput[Y] = - kv * Vy - kf*Ay;
#ifdef CONSENSUS_OLD
			controlInput[X] = - kf*Ax + vcx;
			controlInput[Y] = - kf*Ay + vcy;
#endif //#ifdef CONSENSUS_OLD
#ifdef NO_CONSENSUS_OLD
			controlInput[X] = - kf * Ax ;
			controlInput[Y] = - kf * Ay ;
#endif

#ifdef SIMPLE_ALGORITHM
			controlInput[X] = -3* (position[X] - targetPosition[X]);
			controlInput[Y] = -3* (position[Y] - targetPosition[Y]);
#endif

#ifdef ARRAY_CONSENSUS
			controlInput[X] = - kf*Ax + vcx + xrepel - xcentroid;
			controlInput[Y] = - kf*Ay + vcy + yrepel - ycentroid;

#endif // #ifdef ARRAY_CONSENSUS
// END OF CONTROL ALGORITHM
			fprintf(filestate,"%f %f %f %d %d %d %d %f %f %f %f %d %d %d\n\r", position[X], position[Y], position[2], wheelenc[L], wheelenc[R], control[L], control[R], Ax, Ay, obsFunc0, NowJ, stateReceived[0], stateReceived[1], stateReceived[2]);
			printf("%d %f %f %f %f %f %f %f %f %f\n\r", 
				stateReceived[0], position[X], position[Y], position[2], 
				delta_x, delta_y, controlInput[X], controlInput[Y], deltaJx, deltaJy
				);
			result = result / sizeof(int); 
		} else {
			printf("pipe no reading\n");
			break;
		}
	
		// convert from float to string
		// convert from position to wheel using invert of SE2 matrix
		speed2wheel(controlInput, position[2], controlInputWheel);
		printf("%f %f ", controlInputWheel[L], controlInputWheel[R] );
		saturateKeepVector(controlInputWheel, control, sign, 255);
		printf("%d %d\n", control[L], control[R]);

		snprintf(sentstring, sizeof(sentstring), "S%d&%dE", control[L], control[R]);
		int sizeSent = strlen(sentstring);
		write_port(serialPortfd, sentstring, sizeSent+1);
	}
	snprintf(sentstring, sizeof(sentstring), "S%d&%dE", 0, 0);
	write_port(serialPortfd, sentstring, 6);
	write_port(serialPortfd, sentstring, 6);
	fprintf(filestate,"\n\n");
	fclose(filestate);
	// reset Terminal setting
	tcsetattr(0, TCSANOW, &initial_settings);
}


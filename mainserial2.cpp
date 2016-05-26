/*
 * serial2.cpp
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

#define MESSAGESENT1 "S-100&-100E"

int parseChar(char* msg, int size, int* pipefd);
int readparseChar(int portfd, int* pipefd);

#ifdef TESTING
int main(int argc, char* argv[])
{
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
	double position[3];
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
		/*		 * child process		 */
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
	FILE* filestate = fopen("serial2.txt", "w");
	if(filestate == NULL){
		printf("open file, failed\n");
	}
	fprintf(filestate, "file1\n");
	fclose(filestate);
	/*
	std::stringstream sent_string_stream;
	int LeftW = 0, RightW = 0;
	sent_string_stream << "LeftW= "<< LeftW
						<<"RightW= "<< RightW;
	sent_string_stream << "L:" << LeftW
						<< "&R:" << RightW;
*/
	if(close(pipefd[1]) == -1) printf("failed to close write end\n");
	double targetPosition[2], controlInput[2], controlInputWheel[2];
	targetPosition[0] = 10;
	targetPosition[1] = 0;
	controlInput[0] = 0;
	controlInput[1] = 0;
	while(1){
		filestate = fopen("serial2.txt","a");
		fwrite("haha\n", 5, 1, filestate);
		// convert from float to string
		// convert from position to wheel using invert of SE2 matrix
		speed2wheel(controlInput, position[2], controlInputWheel);
		int controlL = controlInputWheel[L]*SCALE_P;
		int controlR = controlInputWheel[R]*SCALE_P;
		snprintf(sentstring, sizeof(sentstring), "S%d&%dE", controlL, controlR);
/*		char controlLchar[10], controlRchar[10];
		itoa(controlL, controlLchar, 10);
		itoa(controlR, controlRchar, 10);
		strcat(sentstring, "S");
		strcat(sentstring, "controlLchar");
		strcat(sentstring, "&");
		strcat(sentstring, "controlRchar");
		strcat(sentstring, "E");
*/
/*
		int sizeSent = strlen(MESSAGESENT1);
		strncpy(sentstring, MESSAGESENT1, sizeSent);
		sentstring[sizeSent] = '\0';
		write_port(fd, sentstring, sizeSent+1);
		sleep(1);

		sizeSent = strlen("S000&000E");
		strncpy(sentstring, "S255&255E", sizeSent);
		sentstring[sizeSent] = '\0';
		write_port(fd, sentstring, sizeSent+1);
		sleep(1);
	*/
		int stateReceived[8];
		result=1;
		for(int haha=0; haha<50; haha++){
			result = read(pipefd[0], stateReceived, 9*sizeof(int));
			//printf("%d reading\n", result);
			if(result > 0){
				int wheelenc[2];
				wheelenc[L] = stateReceived[L+1];
				wheelenc[R] = stateReceived[R+1];
				wheel2position(wheelenc, position);
				controlInput[X] = -(position[X] - targetPosition[X]);
				controlInput[Y] = -(position[Y] - targetPosition[Y]);
				printf("%f cm | %f cm | %f | %f \n\r", position[X], position[Y], position[2], sqrt(position[X]*position[X]+position[Y]*position[Y]));
				result = result / sizeof(int);
				printf("%d ", haha);
				for(int indexprint=0; indexprint<result; indexprint++){
					printf("%d ", stateReceived[indexprint]);
					fprintf(filestate, "%d ", stateReceived[indexprint]);
				}
				printf("\n");
				fprintf(filestate, "\n");
			} else {
				printf("pipe no reading\n");
				break;
			}
		}
		fclose(filestate);
	}

	//close(filedescwrite);
	//close(filedescread);
}
#endif

/**
 * @brief open read only to serial port
 *
 * @tparam port port name
 */
int open_port(char* port)
{
	//int fd = open("/dev/ttyUSB0", O_RDWR);
	int fd = open(port, O_RDWR|O_NOCTTY);
	if (fd == -1){
		perror("open_read_port failed");
		printf("%s\n", port);
	} else {
		printf("open_read_port %s succeed\n", port);
	}
	return fd;
}

/**
 * @brief writing to serial port
 */
int write_port(int filedesc, char* msg, ssize_t size)
{
	int statusport1 = write(filedesc, msg, size);
	if(statusport1 < 0){
		printf("write() of %lu byte failed!",size);
		fputs("\n", stderr);
	} else {
		//printf("message %lu write %d bytes\n",sizeof(msg), statusport1);
		printf("message %s length %lu wrote %d bytes\n", msg, size, statusport1);
	}
	return statusport1;
}

/**
 * @brief reading from serial port
 */
int read_port(int filedesc, char* msg)
{
	int readed = 0;
	readed = read(filedesc, msg, sizeof(msg));
	msg[readed] = '\0';

	return readed;
}

char check_string(char findchar, char* msg)
{
	int index = 0;
	if(msg[index] == findchar){
		return findchar;
	}
	return findchar;
}

void parse_string(char* aochar, int* aoint, int stateSize)
{
	char* aoc1 = strtok(aochar, "E");
	++aoc1;
	aoint[0] = atoi(aoc1);
	char* aoc2 = aoc1;
	int stateReceived[5];
	for(int indexint=1; indexint<stateSize; indexint++){
		aoc2 = strchr(aoc2, '&');
        	*aoc2 = '\0';
	        ++aoc2;
	        aoint[indexint] = atoi(aoc2);
	}
}

int readparseChar(int portfd, int* pipefd)
{
	int readed;
	char readchar[BUFFER_SIZE], msg[BUFFER_SIZE];	
	int dt, LWE, RWE, LU, FU, RU, pointerArray = 0, packageNum = 0;
	int cekS = 0, cekAmpersand = 0, cekE = 0;

	double position[3];
	char receivedArray[32];
	while(cekS == 0 & cekE == 0){
	readed = read_port(portfd, readchar);
	readchar[readed] = '\0';
	for(int i = 0; i < readed; i++) {
		if(readchar[i] == 'S') cekS++;
		if(readchar[i] == '&') cekAmpersand++;
		if(readchar[i] == 'E') cekE++;
	}
	// tambahkan dari readchar ke msg
		strcat(msg, readchar);
	}
	int size = strlen(msg);
	for(int pointerMsg = 0; pointerMsg < size; pointerMsg++){
		if(msg[pointerMsg] == 'S'){
			pointerArray = 0;
			receivedArray[0] = 'S';
			pointerArray++;
		} else if(msg[pointerMsg] == 'E'&& pointerArray != 0){
			receivedArray[pointerArray] = 'E';
			receivedArray[pointerArray+1] = '\0';
			pointerArray = 0;

			int stateReceived[9];
			parse_string(receivedArray, stateReceived, 8);
			stateReceived[8] = packageNum;
			packageNum++;
			int result = write(pipefd[1], stateReceived, (1+STATE_AMOUNT)*sizeof(int));
			//printf("%d byte write to pipe", result);
			int wheelenc[2];
			wheelenc[L] = stateReceived[L+1];
			wheelenc[R] = stateReceived[R+1];
			wheel2position(wheelenc, position);
		} else if(pointerArray < 32 && pointerArray != 0){
			receivedArray[pointerArray] = msg[pointerMsg];
			pointerArray++;
		}
	}
}

int parseChar(char* msg, int size, int* pipefd)
{
	int dt, LWE, RWE, LU, FU, RU, pointerArray = 0, packageNum = 0;
	double position[3];
	char receivedArray[32];
	for(int pointerMsg = 0; pointerMsg < size; pointerMsg++){
		if(msg[pointerMsg] == 'S'){
			pointerArray = 0;
			receivedArray[0] = 'S';
			pointerArray++;
		} else if(msg[pointerMsg] == 'E'&& pointerArray != 0){
			receivedArray[pointerArray] = 'E';
			receivedArray[pointerArray+1] = '\0';
			pointerArray = 0;

			int stateReceived[9];
			parse_string(receivedArray, stateReceived, 8);
			stateReceived[8] = packageNum;
			packageNum++;
			int result = write(pipefd[1], stateReceived, 9*sizeof(int));
			printf("%d byte write to pipe", result);
			int wheelenc[2];
			wheelenc[L] = stateReceived[L+1];
			wheelenc[R] = stateReceived[R+1];
			wheel2position(wheelenc, position);
		} else if(pointerArray < 32 && pointerArray != 0){
			receivedArray[pointerArray] = msg[pointerMsg];
			pointerArray++;
		}
	}
}

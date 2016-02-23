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

#include "serial2.hpp"
#include "dynamicwhell.hpp"

#ifdef TESTING
int main(int argc, char* argv[])
{
	if(argc != 2 && argc!=3){
		printf("%d argc please use 1 or 2 argument as a serial port\n", argc);
		return(1);
	} else if (argc == 3){
		int fd = open_port(argv[1]);
		if(fd < 0){
			printf("failed to open serial port\n");
			return(-1);
		}
		char sentstring[20];
		strncpy(sentstring, "S0&0E\0", strlen("S0&0E\0"));
		write_port(fd, sentstring, strlen(sentstring));
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
	strncpy(sentstring, "S254&254E\0", strlen("S254&254E\0"));
	int readed = 0;
	char msg[BUFFER_SIZE];
	float position[3];
	position[X] = 0;
	position[Y] = 0;
	position[2] = 0;
	int read_process = fork();
	if(read_process == 0){
		/*
		 * child process
		 */
		while(1){
			static char receivedArray[32];
			static int pointerArray = 0;
			readed = 0;
			readed = read_port(fd, msg);
			if(readed > 0){
				//printf("%s", msg);
				int dt, LWE, RWE, LU, FU, RU;
				for(int pointerMsg = 0; pointerMsg < readed; pointerMsg++){
					if(msg[pointerMsg] == 'S'){
						pointerArray = 0;
						receivedArray[0] = 'S';
						pointerArray++;
						//printf("START RECEIVED %s", msg);
					} else if(msg[pointerMsg] == 'E'&& pointerArray != 0){
						receivedArray[pointerArray] = 'E';
						receivedArray[pointerArray+1] = '\0';
						pointerArray = 0;

						char* aoc1 = strtok(receivedArray, "E");
						char* aoc2 = strchr(aoc1, '&');
				        ++aoc1;
				        dt = atoi(aoc1);
				        *aoc2 = '\0';
				        ++aoc2;
				        LWE = atoi(aoc2);

						char* aoc3 = strchr(aoc2, '&');
						*aoc3 = '\0';
						++aoc3;
						RWE = atoi(aoc3);

						char* aoc4 = strchr(aoc3, '&');
						*aoc4 = '\0';
						++aoc4;
						LU = atoi(aoc4);

						char* aoc5 = strchr(aoc4, '&');
						*aoc5 = '\0';
						++aoc5;
						FU = atoi(aoc5);

						char* aoc6 = strchr(aoc5, '&');
						*aoc6 = '\0';
						++aoc6;
						RU = atoi(aoc6);

				        //printf("%s, %s, %s\n", receivedArray, aoc1, aoc2);
						//printf("%d ms, %d step, %d step, %d cm, %d cm, %d cm\n", dt, LWE, RWE, LU, FU, RU);
						int wheelenc[2];
						wheelenc[L] = LWE;
						wheelenc[R] = RWE;
						wheel2position(wheelenc, position);
						printf("%f cm | %f cm | %f | %f \n\r", position[X], position[Y], position[2], sqrt(position[X]*position[X]+position[Y]*position[Y]));

					} else if(pointerArray < 32 && pointerArray != 0){
						receivedArray[pointerArray] = msg[pointerMsg];
						pointerArray++;
					}
				}
			}
		}
		return 0;

	} else if(read_process < 0){
		printf("making child failed\n");
	} else {
		printf("child pid %d\n", read_process);
	}/*
	std::stringstream sent_string_stream;
	int LeftW = 0, RightW = 0;
	sent_string_stream << "LeftW= "<< LeftW
						<<"RightW= "<< RightW;
	sent_string_stream << "L:" << LeftW
						<< "&R:" << RightW;
*/
	while(1){

		strncpy(sentstring, "S255&255E\0", strlen("S100&200E\0"));
		write_port(fd, sentstring, strlen(sentstring));
		sleep(1);

		strncpy(sentstring, "S255&255E\0", strlen("S200&100E\0"));
		write_port(fd, sentstring, strlen(sentstring));
		sleep(1);
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

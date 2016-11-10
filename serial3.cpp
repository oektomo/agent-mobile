/*
 * serial3.cpp
 *
 *  Created on: May 26, 2016
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
	if(statusport1 < 0) {
		printf("write() of %lu byte failed!",size);
		fputs("\n", stderr);
	} else {
		// printf("message %lu write %d bytes\n",sizeof(msg), statusport1);
		// printf("message %s length %lu wrote %d bytes\n", msg, size, statusport1);
	}
	return statusport1;
}

/**
 * @brief reading from serial port and add '\0' at the end of string
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

/**

 to convert? to parse?
* @brief to parse string and convert to array of integer
parse_string(
char* aochar,  
int* aoint, 
int stateSize)
*/
void parse_string(char* aochar, int* aoint, int stateSize)
{
	char* aoc1 = strtok(aochar, "E");
	++aoc1;
	aoint[0] = atoi(aoc1);
	char* aoc2 = aoc1;
	int stateReceived[5];
	for(int indexint=1; indexint<stateSize; indexint++) {
		aoc2 = strchr(aoc2, '&');
        	*aoc2 = '\0';
	        ++aoc2;
	        aoint[indexint] = atoi(aoc2);
	}
}

/**
* read from serial port until header 'S' and tail 'E' received
*
*/
int getFrame(int portfd, char* msg)
{
	int cekS = 0, cekAmpersand = 0, cekE = 0, rec_data = 0;
	char readchar[BUFFER_SIZE];

	while(cekS == 0 | cekE == 0) {
	int readed = read_port(portfd, readchar);
	readchar[readed] = '\0';
	for(int i = 0; i < readed; i++) {
		if(readchar[i] == 'S') cekS++;
		if(readchar[i] == '&' && cekS > 0) cekAmpersand++;
		if(readchar[i] == 'E' && cekS > 0) cekE++;
	}
	//if(readed > 0) printf("rec %d data %s \n",readed, readchar);
	// tambahkan dari readchar ke msg
	if(cekS > 0) {
		strcat(msg, readchar);
		rec_data = rec_data + readed;
	}
	}

	return(rec_data);

}

/**
* @brief input string with single start char, single end char, '&' delimiter
*	stateSize telling how much data being extract
* @bug kemungkinan bug adalah, string yang diinputkan diedit oleh program
*	agak menghawatirkan, jadi balikkan akan menghilangkan delimiter
*/
void parseStrToInt(char* aochar, int* aoint, int stateSize)
{
	//char* aoc1 = strtok(aochar, "E");
		
	char* aoc1 =  strchr(aochar, 'S');
	++aoc1;
	aoint[0] = atoi(aoc1);
	char* aoc2 = aoc1;
	for(int indexint=1; indexint<stateSize; indexint++) {
		aoc2 = strchr(aoc2, '&');
        	//*aoc2 = '\0';
	        ++aoc2;
	        aoint[indexint] = atoi(aoc2);
	}
}

/**
* @brief to read from serial port and sent data to logical pipe
*/
int readparseChar(int portfd, int* pipefd)
{
	char msg[BUFFER_SIZE];	
	int pointerArray = 0, packageNum = 0;

	for (int i=0; i<BUFFER_SIZE; i++) msg[i]='\0';

	int size = getFrame(portfd, msg);
	msg[size] = '\0';
	// done collecting one package data, start parsing
	//printf("parse %d data: %s\n", size, msg);
	int stateReceived[9];
	parseStrToInt(msg, stateReceived, 8);
	//printf("parsed data: S%d&%d&%d&%d&%d&%d&%d&%dE\n", stateReceived[0], stateReceived[1], stateReceived[2], stateReceived[3], stateReceived[4], stateReceived[5], stateReceived[6], stateReceived[7]);
	int result = write(pipefd[1], stateReceived, (STATE_AMOUNT)*sizeof(int));
//	printf("%d byte write to pipe", result);
}

/**
* obsolete, noone use it and can remove safely
*/
int parseChar(char* msg, int size, int* pipefd)
{
	int dt, LWE, RWE, LU, FU, RU, pointerArray = 0, packageNum = 0;
	double position[3];
	char receivedArray[32];
	for(int pointerMsg = 0; pointerMsg < size; pointerMsg++) {
		if(msg[pointerMsg] == 'S') {
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
		} else if(pointerArray < 32 && pointerArray != 0) {
			receivedArray[pointerArray] = msg[pointerMsg];
			pointerArray++;
		}
	}
}

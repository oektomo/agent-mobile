/*
 * serial1.cpp
 *
 *  Created on: Jan 25, 2016
 *      Author: otm
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "serial1.hpp"

#ifdef TESTING
#include "dynamicwhell.hpp"

int main(int argc, char* argv[])
{
	char portname[100];

	printf("opening file ");
	if(argc > 1){
		strncpy(portname, argv[1], sizeof(portname));
		printf("%s", portname);
		/*
	for(int index = 0;;index++){
		portname[index] = argv[1][index];
		if((argv[1][index]) == NULL) break;
		printf("%c",argv[1][index]);
	}*/
	printf("\n");
	} else {
		//portname[0] = "/dev/ttyACM0\0";
		strncpy(portname,"/dev/ttyACM0\0", sizeof(portname));
		printf("/dev/ttyACM0\n");
	}

	// Opening port
	int filedesc = open_port(portname);
	printf("port opened with file descriptor %d\n", filedesc);

	// writing to serialport
	char sentstring[] = "wwwwww\0";
	write_port(filedesc, sentstring);
	sleep(1);
	char sentstring2[] = "xxxxxx\0";
	write_port(filedesc, sentstring2);
	sleep(1);

	// reading serialport
	// try using delay
	char buffer[100];
	int readed;
	while(readed == 0){
		readed = read_port(filedesc, buffer);
	}
	printf("%d character read, %s\n", readed, buffer);

	readed = 0;
	while(readed == 0){
	readed = read(filedesc, buffer, sizeof(buffer));
	}
	readed = read(filedesc, buffer, sizeof(buffer));
	buffer[readed] = '\0';
	printf("%d character read, %s\n", readed, buffer);

	float speed[2], wheel[2], theta = 0.5;
	speed[X] = 0.3;
	speed[Y] = 0.9;
	wheel[R] = 2;
	wheel[L] = 3;

	speed2wheel(speed, theta, wheel);
	printf("speed[X] %f, speed[Y] %f, wheel[R] %f, wheel[L] %f \n"\
			,speed[X],speed[Y],wheel[R],wheel[L]);

	//writing to stop
	write_port(filedesc, sentstring2);

	close(filedesc);
}
#endif

/**
 * @brief to open serial port
 *
 *
 */
int open_port(char* portname)
{
	int fd;

	fd = open(portname, O_RDWR|O_NOCTTY|O_NDELAY);
	if (fd == -1){
		perror("open_port: failed");
	} else {
		printf("open port %s succeed\n", portname);
		fcntl(fd, F_SETFL, 0);
	}

	return (fd);

}

/**
 * @brief writing to serial port
 */
void write_port(int filedesc, char* msg)
{
	int statusport1 = write(filedesc, msg, sizeof(msg));
	if(statusport1 < 0){
		fputs("write() of 4 byte failed!\n", stderr);
	} else {
		printf("write %d bytes\n", statusport1);
	}
}

/**
 * @brief reading from serial port
 */
int read_port(int filedesc, char*msg)
{
	int readed = 0;
	readed = read(filedesc, msg, sizeof(msg));
	msg[readed] = '\0';

	return readed;
}

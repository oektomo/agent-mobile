/*
 * serial1.cpp
 *
 *  Created on: Jan 25, 2016
 *      Author: otm
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "serial1.hpp"

#ifdef TESTING
#include "dynamicwhell.hpp"

int main(int argc, char* argv[])
{
	char portname[100];

	printf("opening file ");
	if(argc > 1){
		strncpy(portname, argv[1], strlen(argv[1]));
		//portname[strlen(argv[1])] == '\0';
		printf("%s argument %ld", portname, strlen(argv[1]));

	printf("\n");
	} else {
		//portname[0] = "/dev/ttyACM0\0";
		strncpy(portname,"/dev/ttyACM0", strlen("/dev/ttyACM0"));
		printf("%s length %ld \n", portname, strlen("/dev/ttyACM0"));
	}


	// Opening port
	//int filedesc = open_port(portname);
	int filedescread = open_read_port(portname);
	if (filedescread < 0) printf("failed opening %s to read\n", portname);
	// reading serialport
	// try using delay
	while(1);
	char buffer[100];
	int readed = 0;
	int pidread = fork();
	if((pidread == 0) && (filedescread > 0)){
		while(1){
		while(readed == 0){
			readed = read_port(filedescread, buffer);
		}
		printf("%d character read, %s\n", readed, buffer);
		readed = 0;
		}
	} else if (pidread > 0) {
		printf("success made child for read serial port\n");
	} else { printf("failed making child for read serial port\n");}

	int filedescwrite = open_write_port(portname);
	//printf("port opened with file descriptor %d\n", filedesc);
	//sleep(2); //required to make flush work, for some reason
	//tcflush(filedesc,TCIOFLUSH);

	// writing to serialport
	char sentstring[20];
	strncpy(sentstring, "wwwwww\0", strlen("wwwwww\0"));

	write_port(filedescwrite, sentstring, strlen(sentstring));
	//sleep(1);
	tcflush(filedescwrite,TCIOFLUSH);
	strncpy(sentstring, "xxxxxx\0", strlen("xxxxxx\0"));
	write_port(filedescwrite, sentstring, strlen(sentstring));
	sleep(1);


	float speed[2], wheel[2], theta = 0.5;
	speed[X] = 0.3;
	speed[Y] = 0.9;
	wheel[R] = 2;
	wheel[L] = 3;

	speed2wheel(speed, theta, wheel);
	printf("speed[X] %f, speed[Y] %f, wheel[R] %f, wheel[L] %f \n"\
			,speed[X],speed[Y],wheel[R],wheel[L]);

	//writing to stop
	tcflush(filedescwrite,TCIOFLUSH);
	write_port(filedescwrite, sentstring, strlen(sentstring));

	close(filedescwrite);
	//close(filedescread);
}
#endif

/**
 * @brief to open write and read serial port
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
 * @brief open write only to serial port
 */
int open_write_port(char* portname)
{
	int fd = open(portname, O_WRONLY|O_NOCTTY);
	if (fd == -1){
		perror("open_write_port: failed");
	} else {
		printf("open_write_port %s succeed\n", portname);
		//fcntl(fd, F_SETFL, 0);
	}

	return fd;
}

/**
 * @brief open read only to serial port
 */
int open_read_port(char* portname)
{
	int fd = open(portname, O_RDONLY|O_NOCTTY);
	if (fd == -1){
		perror("open_read_port: failed");
	} else {
		printf("open_read_port %s succeed\n", portname);
		//fcntl(fd, F_SETFL, 0);
	}

	return fd;
}

/**
 * @brief writing to serial port
 */
void write_port(int filedesc, char* msg, ssize_t size)
{
	int statusport1 = write(filedesc, msg, size);
	if(statusport1 < 0){
		printf("write() of %lu byte failed!",size);
		fputs("\n", stderr);
	} else {
		//printf("message %lu write %d bytes\n",sizeof(msg), statusport1);
		printf("message %s length %lu wrote %d bytes\n", msg, size, statusport1);
	}
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

/*
 * serial2.cpp
 *
 *  Created on: Feb 2, 2016
 *      Author: otm
 */


#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "serial2.hpp"
#include "dynamicwhell.hpp"

#ifdef TESTING
int main(int argc, char* argv[])
{
	if(argc != 2){
		printf("please use 1 argument as a serial port\n");
		return(1);
	}
	// Opening port
	int fd = open_serial(argv[1]);
	if(fd < 0){
		printf("failed to open serial port\n");
		return(-1);
	}

	// writing to serialport
	char sentstring[20];
	strncpy(sentstring, "wwwwww\0", strlen("wwwwww\0"));
	int readed = 0;
	char msg[BUFFER_SIZE];
	int read_process = fork();
	if(read_process == 0){
		/*
		 * child process
		 */
		while(1){
			readed = 0;
			readed = read_port(fd, msg);
			if(readed > 0){
				printf("%s", msg);
			}
		}
		return 0;

	} else if(read_process < 0){
		printf("making child failed\n");
	} else {
		printf("child pid %d\n", read_process);
	}
	std::stringstream sent_string_stream;
	int LeftW = 0, RightW = 0;
	sent_string_stream << "LeftW= "<< LeftW
						<<"RightW= "<< RightW;

	while(1){

		strncpy(sentstring, "wwwwww\0", strlen("wwwwww\0"));
		write_port(fd, sentstring, strlen(sentstring));
		sleep(1);

		strncpy(sentstring, "xxxxxx\0", strlen("xxxxxx\0"));
		write_port(fd, sentstring, strlen(sentstring));
		sleep(1);
	}

	//close(filedescwrite);
	//close(filedescread);
}
#endif

/**
 * @brief open read only to serial port
 */
int open_serial(char* port)
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

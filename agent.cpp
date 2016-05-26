/*
 * agent.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: otm
 */
#include <string>
#include <iostream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <errno.h>

#include "dynamicwhell.cpp"
#include "serial2.hpp"
#include "network.hpp"

#define BUFFER_SPACE 1000



int main(int argc, char** argv)
{
	int socketfd1;	// socket file descriptor
	int statusfd1;	// status for file descriptor
	socketfd1 = creating_socket();
	/*
	 * listen()
	 */
	printf("listening socket\n");
	statusfd1 = listen(socketfd1, 5);
	if(statusfd1 == -1) printf("socket listening failed\n");

	/*
	 * accepting connection
	 */
	int accepted1;	// accepted socket file descriptor
	struct sockaddr_storage their_addr;
	socklen_t addr_size = sizeof(their_addr);

	for(int trynum = 0; trynum<2; trynum++){
	accepted1 = accept(socketfd1, (struct sockaddr *)&their_addr, &addr_size);
	if(accepted1 == -1){
		printf("listen/accept error\n");
	} else {
		printf("connection ACCEPTED\n");
	}

	ssize_t bytes_received, bytes_sent;
	int childrecieve = fork();

	if(childrecieve == 0){
		/*
		 * child process
		 * receiving data
		 */
		printf("Receiving data\n");
		char incoming_buffer[BUFFER_SPACE];
		bytes_received = recv(accepted1, incoming_buffer, (BUFFER_SPACE-1), 0);
		if(bytes_received == 0){
			printf("host shutdown\n");
		} else if(bytes_received == -1) {
			printf("receive error\n");
		}
		incoming_buffer[bytes_received] = '\0';

		printf("received data: ");
		std::cout<<incoming_buffer<<std::endl;


		// opening serial port
		char portname[] = "/dev/ttyUSB0";
		int portfd = open_port(portname);
		// writing serial port
		char msg[100];
		strncpy(msg, "ww", 2);
		write_port(portfd, msg, 2);
		sleep(2);
		strncpy(msg, "xx", 2);
		write_port(portfd, msg, 2);
		close(portfd);		// close serial port


	} else if(childrecieve > 0) {
	/*
	 * parent process
	 * sending data
	 */
	printf("Agent 3 sending data\n");

	bytes_sent = write( accepted1, "Agent sending data", 18 );
	if(bytes_sent < 0){
		printf("ERROR writing to socket");
	}
	printf("%ld bytes sent\n",bytes_sent);
	} else {
		printf("child creation failed\n");
	}

	}

	/*
	 * closing socket
	 */
	close(accepted1);

	return 0;
}

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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define BUFFER_SPACE 1000

int socketfd1;	// socket file descriptor
int newsd;		// accepted socket file descriptor
int statusfd1;	// status when creating file descriptor

int main(int argc, char** argv)
{
	/*
	 * creation addrinfo struct
	 * getaddrinfo() section
	 *
	 */
	struct addrinfo host_info1;	// addrinfo struct from netdb.h
	struct addrinfo *host_info_list1;
	memset(&host_info1, 0, sizeof host_info1);
	host_info1.ai_family = AF_UNSPEC;
	host_info1.ai_socktype = SOCK_STREAM;
	host_info1.ai_flags = AI_PASSIVE;
	statusfd1 = getaddrinfo(NULL, "5556", &host_info1, &host_info_list1);
	if(statusfd1 != 0) std::cout<<"getaddrinfo error"<<gai_strerror(statusfd1);

	/*
	 * socket creation
	 * socket()
	 * setsockopt() to make sure the port is not in use from previous execution
	 * bind()
	 * listen()
	 */
	printf("creating socket\n");
	socketfd1 = socket(host_info_list1->ai_family, host_info_list1->ai_socktype, host_info_list1->ai_protocol);
	if(socketfd1 == -1) printf("socket creation error");
	int yes = 1;
	statusfd1 = setsockopt(socketfd1, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	printf("binding socket\n");
	statusfd1 = bind(socketfd1, host_info_list1->ai_addr, host_info_list1->ai_addrlen);
	if(statusfd1 == -1){
		perror("BIND");
		exit(0);
	}
	printf("listening socket\n");
	statusfd1 = listen(socketfd1, 5);
	if(statusfd1 == -1) printf("socket listening failed\n");

	/*
	 * accepting connection
	 */
	int new_connection;
	struct sockaddr_storage their_addr;
	socklen_t addr_size = sizeof(their_addr);
	new_connection = accept(socketfd1, (struct sockaddr *)&their_addr, &addr_size);
	if(new_connection == -1){
		printf("listen/accept error\n");
	} else {
		printf("connection ACCEPTED\n");
	}

	/*
	 * receiving data
	 */
	ssize_t bytes_received;
	char incoming_buffer[BUFFER_SPACE];
	bytes_received = recv(new_connection, incoming_buffer, (BUFFER_SPACE-1), 0);
	if(bytes_received == 0){
		printf("host shutdown\n");
	} else if(bytes_received == -1) {
		printf("receive error\n");
	}
	incoming_buffer[bytes_received] = '\0';
	std::cout<<incoming_buffer<<std::endl;

	/*
	 * closing socket
	 */
	close(new_connection);

	return 0;
}

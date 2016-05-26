/*
 * network.cpp
 *
 *  Created on: Feb 29, 2016
 *      Author: otm
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stddef.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "network.hpp"

int creating_socket()
{
	int socketfd1;
	int statusfd1;	// status when creating file descriptor

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
	if(statusfd1 != 0) printf("getaddrinfo error %s\n", gai_strerror(statusfd1)); //std::cout<<"getaddrinfo error"<<gai_strerror(statusfd1);

	/*
	 * socket creation
	 * socket()
	 * setsockopt() to make sure the port is not in use from previous execution
	 * bind()
	 *
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
		return(0);
	}

	return socketfd1;
}




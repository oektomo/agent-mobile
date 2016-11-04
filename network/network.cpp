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
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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

int create_server()
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    //char sendBuff[1025];
    //time_t ticks; 
    // creating socket, an endpoint for AF_INET protocol
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    // setting parameter for socket 
    memset(&serv_addr, '0', sizeof(serv_addr));
    //memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 

    // binding the socket and port
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    // server to start listening
    listen(listenfd, 10); 
    return listenfd; 

}

/**
* create and connect client
* return socket file descriptor
*/
int create_client( char* address )
{
    int sockfd = 0;
    struct sockaddr_in serv_addr; 
    // creating socket ( an endpoint for receiving data ) 
    // memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    // inet_pton to convert ip address (array of char) to sockaddr struct
    // because connect() need to receive sockaddr argument and the address used are needed in binary form rather text form
    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); 

    if(inet_pton(AF_INET, address, &serv_addr.sin_addr)<=0) {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    // connect socket file description with serv_addr in sockaddr
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
       printf("\n Error : Connect Failed \n");
       return 1;
    }
    return sockfd; 

}

/**
* generic sent data with data being sent is 4 with double (each 8 byte)
*
*/
void sentData(int sockfd, double* fData, int size)
{
	char dataF[STATE_SENT+3];
	dataF[0] = 'S';
	//memcpy(dataF+1, fData, 16);
	memcpy(dataF+1, fData, STATE_SENT);
	//dataF[17] = 'E';
	dataF[STATE_SENT+1] = 'E';
	dataF[STATE_SENT+2] = '\0';
	printf("raw data %s \n",dataF);
	write(sockfd, dataF, STATE_SENT+2);
}

/**
* generic read from socket
* blocking while socket open
*/
void readSocket(int sockfd, double* fData, int size)
{
	ssize_t readed;
	char readchar[2];
	int cekS = 0, cekE = 0, rec_data = 0;
	char recvBuff[STATE_SENT+3];
    while ( (readed = read(sockfd, readchar, 1)) > 0 ) {
    	// getFrame
	for(int i = 0; i < readed; i++) {
		if(readchar[i] == 'S') cekS++;
		if(readchar[i] == 'E' && cekS > 0) cekE++;
	}
	// cat data into frame
	if(cekS > 0) {
		recvBuff[rec_data] = readchar[0];
		rec_data = rec_data + readed;
	}
	// frame already get then copy into data.

	if( (cekE > 0)&&(rec_data >= (STATE_SENT+2)) ) {
		cekS = 0; cekE = 0;
		printf("readed %d char\n",rec_data);
		recvBuff[rec_data]='\0';
		rec_data = 0;
		memcpy(fData, recvBuff+1, STATE_SENT);
		printf("raw data %s \n",recvBuff);
		printf("data, %f & %f\n", fData[0], fData[1]);
		memset(recvBuff, 0, 34);
		break;
	}
    } 

    if(readed < 0) {
        printf("\n Read error \n");
    } 

}


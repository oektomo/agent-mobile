#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "client_make.h"

/**
* create and connect client
* return socket file descriptor
*/
int create_client( char* address )
{
    int sockfd = 0;
    struct sockaddr_in serv_addr; 
/*
    // to check if ip server is provided
    if(argc != 2) {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    } 
*/
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
* generic read from socket
* blocking while socket open
*/
void readSocket(int sockfd, double* data, int size)
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
		memcpy(data, recvBuff+1, STATE_SENT);
		printf("raw data %s \n",recvBuff);
		printf("data, %f & %f\n", data[0], data[1]);
		memset(recvBuff, 0, 34);
		break;
	}
    } 

    if(readed < 0) {
        printf("\n Read error \n");
    } 

}

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

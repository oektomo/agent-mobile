#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

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

/*
    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

        ticks = time(NULL);
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        write(connfd, sendBuff, strlen(sendBuff)); 

        close(connfd);
        sleep(1);
     }
*/
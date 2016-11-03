#ifndef CLIENT_MAKE_H
#define CLIENT_MAKE_H

int create_client( char* address );

void sentData(int sockfd, double* fData, int size);
void readSocket(int sockfd, char* recvBuff, double* data, int size);

#endif

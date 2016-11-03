#ifndef CLIENT_MAKE_H
#define CLIENT_MAKE_H

#define STATE_SENT 32

int create_client( char* address );

void sentData(int sockfd, double* fData, int size);
void readSocket(int sockfd, double* data, int size);

#endif

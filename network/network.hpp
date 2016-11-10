/*
 * network.hpp
 *
 *  Created on: Feb 29, 2016
 *      Author: otm
 */

#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#define STATE_SENT 32
/**
* @brief to create socket and bind it to network port
*/
int creating_socket();
int create_server();
int create_client( char* address );

void sentData(int sockfd, double* fData, int size);
int readSocket(int sockfd, double* fData, int size);



#endif /* NETWORK_HPP_ */

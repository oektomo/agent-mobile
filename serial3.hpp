/*
 * serial3.hpp
 *
 *  Created on: May 26, 2016
 *      Author: otm
 */

#ifndef SERIAL3_HPP_
#define SERIAL3_HPP_

#define TESTING
#define BUFFER_SIZE 100
#define STATE_AMOUNT 8

int open_port(char* port);
int write_port(int filedesc, char* msg, ssize_t size);
int read_port(int filedesc, char* msg);
void parse_string(char* aochar, int* aoint, int stateSize);
int readparseChar(int portfd, int* pipefd);
int parseChar(char* msg, int size, int* pipefd);

#endif /* SERIAL3_HPP_ */

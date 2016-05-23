/*
 * serial2.hpp
 *
 *  Created on: Feb 2, 2016
 *      Author: otm
 */

#ifndef SERIAL2_HPP_
#define SERIAL2_HPP_

#define TESTING
#define BUFFER_SIZE 100

int open_port(char* port);
int write_port(int filedesc, char* msg, ssize_t size);
int read_port(int filedesc, char* msg);
void parse_string(char* aochar, int* aoint, int stateSize);

#endif /* SERIAL2_HPP_ */

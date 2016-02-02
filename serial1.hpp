/*
 * serial1.hpp
 *
 *  Created on: Jan 25, 2016
 *      Author: otm
 */

#ifndef SERIAL1_HPP_
#define SERIAL1_HPP_
#define TESTING

int open_port(char* portname);
int open_write_port(char* portname);
int open_read_port(char* portname);
void write_port(int filedesc, char* msg, ssize_t size);
int read_port(int filedesc, char*msg);

#endif /* SERIAL1_HPP_ */

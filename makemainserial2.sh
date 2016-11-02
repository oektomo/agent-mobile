#!/bin/bash
g++ serial3.cpp network/server_make.cpp tools.c iicsource/iic_hmc3_lib.cpp dynamicwhell.cpp mainserial2.cpp -lconfig -o mainserial2

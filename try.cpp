/*
 * try.cpp
 *
 *  Created on: Jan 26, 2016
 *      Author: otm
 */

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
char getch();

#ifdef TESTING
int main( ) {

   int c;

   printf( "Enter a value :");
   c = getchar( );

   printf( "\nYou entered: ");
   putchar( c );
   printf("\n");

   while(c != 'x')
   {
	   c=getch();
	   if(c == 'a') printf("a printed ");
   }
   return 0;
}
#endif

/**
 * @brief	get input from terminal directly without pressing ENTER
 */
char getch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}


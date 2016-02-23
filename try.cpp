/*
 * try.cpp
 *
 *  Created on: Jan 26, 2016
 *      Author: otm
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
int main()
{

	char msg[] = "&120&100&112E\n\rS123&4&3&120&100&112E\n\r"
			"S120&2&5&110&300&132E\n\rS123&4&3&120\0";
	//char msg[] = "START RECEIVE\0";
	//char msg[] = "END RECEIVE\n\0";
	int readed = strlen(msg);
	char receivedArray[32];
	int pointerArray = 0;
	int dt, LWE, RWE, LU, FU, RU;
	for(int pointerMsg = 0; pointerMsg < readed; pointerMsg++){
		if(msg[pointerMsg] == 'S'){
			pointerArray = 0;
			receivedArray[0] = 'S';
			pointerArray++;
		} else if(msg[pointerMsg] == 'E' && pointerArray != 0){
			receivedArray[pointerArray] = 'E';
			receivedArray[pointerArray+1] = '\0';
			pointerArray = 0;

			char* aoc1 = strtok(receivedArray, "E");
			char* aoc2 = strchr(aoc1, '&');
	        ++aoc1;
	        dt = atoi(aoc1);
	        *aoc2 = '\0';
	        ++aoc2;
	        LWE = atoi(aoc2);

			char* aoc3 = strchr(aoc2, '&');
			*aoc3 = '\0';
			++aoc3;
			RWE = atoi(aoc3);

			char* aoc4 = strchr(aoc3, '&');
			*aoc4 = '\0';
			++aoc4;
			LU = atoi(aoc4);

			char* aoc5 = strchr(aoc4, '&');
			*aoc5 = '\0';
			++aoc5;
			FU = atoi(aoc5);

			char* aoc6 = strchr(aoc5, '&');
			*aoc6 = '\0';
			++aoc6;
			RU = atoi(aoc6);

	        printf("%s, %s, %s\n", receivedArray, aoc1, aoc2);
			printf("%d | %d | %d | %d | %d | %d \n", dt, LWE, RWE, LU, FU, RU);

		} else if(pointerArray < 32 && pointerArray != 0){
			receivedArray[pointerArray] = msg[pointerMsg];
			pointerArray++;
		}
	}

	return 0;
}
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


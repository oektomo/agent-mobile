#include <stdio.h>
#include <termios.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "tools.h"

/**
* to unblock getchar, probably scanf too, it's also disable echoing in terminal behaviour.
*
*/
void unblock_getchar( struct termios new_settings )
{
  new_settings.c_lflag &= ~ICANON;
  new_settings.c_lflag &= ~ECHO;
  new_settings.c_lflag &= ~ISIG;
  new_settings.c_cc[VMIN] = 0;
  new_settings.c_cc[VTIME] = 0;
 
  tcsetattr(0, TCSANOW, &new_settings);
}

void blockPrintf()
{
		umask(0);
		pid_t  sid = setsid();
		if(sid < 0) printf("setsid failed\n");
		else printf("setsid done?\n");
		//close( STDIN_FILENO );
		//close( STDOUT_FILENO );
		int fd = open("/dev/null",O_RDWR, 0);

		if (fd != -1) {
			dup2 (fd, STDIN_FILENO);
			dup2 (fd, STDOUT_FILENO);
			dup2 (fd, STDERR_FILENO);

			if (fd > 2) {
				close (fd);
			}
		}
}

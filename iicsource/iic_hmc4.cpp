#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "iic_hmc3_lib.h"

int main()
{
	int fd = initializeCompass();
	short Data[3];
	float initAngle = getHeading(fd);

	printf("initial Angle = %0.1f\n", initAngle);
	//while(1)
	for(int i = 0; i<100; i++)
	{
	/*
		readCompassData(Data, fd);
                float angle = atan2(Data[1], Data[0]) * 180 / M_PI;
		if(angle < 0) angle = angle + 360;
	*/
		float angle = getHeading(fd);
		if(angle<initAngle) {angle = 2*M_PI - (initAngle-angle);}
		else {angle = angle - initAngle;}

                :q
                :q
                :x
		usleep(300*1000);
	}

return 0;
}

    #include <stdio.h>
    #include <stdlib.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/ioctl.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <linux/i2c-dev.h>
    #include <math.h>
    #include "iic_hmc3_lib.h"

    void selectDevice(int fd, int addr, char *name)
    {
        if (ioctl(fd, I2C_SLAVE, addr) < 0) {
            fprintf(stderr, "%s not present\n", name);
            //exit(1);
        }
    }

    void writeToDevice(int fd, int reg, int val)
    {
        char buf[2];
        buf[0]=reg;
        buf[1]=val;

        if (write(fd, buf, 2) != 2) {
            fprintf(stderr, "Can't write to ADXL345\n");
            //exit(1);
        }
    }

    int initializeCompass()
    {
    	int fd;
        if ((fd = open("/dev/i2c-1", O_RDWR)) < 0) {
            // Open port for reading and writing
            fprintf(stderr, "Failed to open i2c bus\n");

            return 1;
        }

        selectDevice(fd, HMC5883L_I2C_ADDR, (char *)"HMC5883L");

        //writeToDevice(fd, 0x01, 0);
        writeToDevice(fd, 0x01, 32);
        writeToDevice(fd, 0x02, 0);
	
	return fd;
    }

    int readCompassData(short* Data, int fd)
    {
	    unsigned char buf[16];
            buf[0] = 0x03;
	    int returnData = 0;

            if ((write(fd, buf, 1)) != 1) {
                // Send the register to read from
                fprintf(stderr, "Error writing to i2c slave\n");
            }

            if (read(fd, buf, 6) != 6) {
                fprintf(stderr, "Unable to read from HMC5883L\n");
            } else {
	    	returnData = 1;
                Data[0] = (buf[0] << 8) | buf[1]; // x
                Data[1] = (buf[4] << 8) | buf[5]; // y
                Data[2] = (buf[2] << 8) | buf[3]; // z
               
            }
	    return returnData;
    }

    float getHeading(int fd)
    {

		short Data[3];
		readCompassData(Data, fd);
                float angle = atan2(Data[1], Data[0]);
		 if(angle<0) angle = angle + 2 * M_PI;
		return angle;
    }

